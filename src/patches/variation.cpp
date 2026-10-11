#include "pch.h"
#include "variation.h"
#include "CargrpSelectionApi.h"
#include <CStreaming.h>
#include <CGeneral.h>
#include <CTheScripts.h>
#include <CModelInfo.h>
#include <cstring>

namespace {
unsigned backend{}; // 0=off, 1=standalone, 2=CargrpLimit, 3=unavailable, 4=pending
bool initialized{};
unsigned standaloneCapacity=23;
CgrVariationPredicate registeredPredicate{};

bool Readable(const void* ptr,size_t size,bool execute=false) {
    auto addr=reinterpret_cast<uintptr_t>(ptr);
    if (!addr || !size || addr+size<addr) return false;
    const auto end=addr+size;
    while (addr<end) {
        MEMORY_BASIC_INFORMATION m{};
        if (!VirtualQuery(reinterpret_cast<void*>(addr),&m,sizeof(m)) || m.State!=MEM_COMMIT || (m.Protect&(PAGE_GUARD|PAGE_NOACCESS))) return false;
        const auto p=m.Protect&0xff;
        if (p!=PAGE_READONLY && p!=PAGE_READWRITE && p!=PAGE_WRITECOPY && p!=PAGE_EXECUTE_READ && p!=PAGE_EXECUTE_READWRITE && p!=PAGE_EXECUTE_WRITECOPY) return false;
        if (execute && p!=PAGE_EXECUTE_READ && p!=PAGE_EXECUTE_READWRITE && p!=PAGE_EXECUTE_WRITECOPY) return false;
        const auto next=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;
        if (next<=addr) return false;
        addr=next;
    }
    return true;
}
bool ValidModel(int model) { return model>=0 && model<20000 && model!=2000; }
CBaseModelInfo* ModelInfo(int model) {
    if (!ValidModel(model)) return nullptr;
    // Resolve limit-adjuster bindings at use time, not during DLL initialization.
    auto* count=reinterpret_cast<const int*>(0x40CD58);
    auto* binding=reinterpret_cast<CBaseModelInfo***>(0x40CD67);
    if (!Readable(count,sizeof(*count)) || !Readable(binding,sizeof(*binding)) || model>=*count) return nullptr;
    auto** models=*binding;
    const auto slot=reinterpret_cast<uintptr_t>(models)+static_cast<unsigned>(model)*sizeof(void*);
    if (!Readable(reinterpret_cast<void*>(slot),sizeof(void*))) return nullptr;
    auto* info=*reinterpret_cast<CBaseModelInfo**>(slot);
    return Readable(info,sizeof(CBaseModelInfo)) ? info : nullptr;
}
unsigned __cdecl Suitable(int model,unsigned purpose) {
    if (purpose!=CGR_SELECT_ORDINARY && purpose!=CGR_SELECT_GANG) return 0;
    auto* info=ModelInfo(model);
    if (!info) return 0;
    if (plugin::CallAndReturn<bool,0x46A810>(model)) return 0;
    if (CTheScripts::HasVehicleModelBeenBlockedByScript(model)) return 0;
    if (CStreaming::WeAreTryingToPhaseVehicleOut(model)) return 0;
    return info->m_nRefCount<=2 ? 1u : 0u;
}
int StandalonePick(int group) {
    if (group<0 || group>=34) return -1;
    auto* countsBinding=reinterpret_cast<short**>(0x421915);
    auto* rowsBinding=reinterpret_cast<short**>(0x421948);
    if (!Readable(countsBinding,sizeof(*countsBinding)) || !Readable(rowsBinding,sizeof(*rowsBinding))) return -1;
    const auto countSlot=reinterpret_cast<uintptr_t>(*countsBinding)+static_cast<unsigned>(group)*sizeof(short);
    if (!Readable(reinterpret_cast<void*>(countSlot),sizeof(short))) return -1;
    const int count=*reinterpret_cast<const short*>(countSlot);
    if (count<=0 || static_cast<unsigned>(count)>standaloneCapacity) return -1;
    const auto row=reinterpret_cast<uintptr_t>(*rowsBinding)+2u*standaloneCapacity*static_cast<unsigned>(group);
    if (!Readable(reinterpret_cast<void*>(row),2u*static_cast<unsigned>(count))) return -1;
    for (unsigned attempt=0;attempt<20;++attempt) {
        // The integer random upper bound is exclusive.
        const auto index=CGeneral::GetRandomNumberInRange(0,count);
        if (index>=static_cast<unsigned>(count)) return -1;
        const int id=reinterpret_cast<const short*>(row)[index];
        if (Suitable(id,CGR_SELECT_ORDINARY)) return id;
    }
    return -1;
}
int __cdecl Ordinary(int group) { return StandalonePick(group); }
bool IsLoaded(int model) {
    if (!ValidModel(model) || !Readable(reinterpret_cast<void*>(0x408B25),sizeof(void*))) return false;
    auto* flags=*reinterpret_cast<unsigned char**>(0x408B25);
    const auto slot=reinterpret_cast<uintptr_t>(flags)+static_cast<unsigned>(model)*20u;
    return Readable(reinterpret_cast<void*>(slot),1) && *reinterpret_cast<unsigned char*>(slot)==1;
}
int __cdecl Gang(int gang) {
    if (gang<0 || gang>=10) return -1;
    const auto model=StandalonePick(gang+18);
    if (model<0) return -1;
    if (!IsLoaded(model)) {
        CStreaming::RequestModel(model,8);
        CStreaming::LoadAllRequestedModels(true);
    }
    // Return exactly the vehicle that was checked and loaded.
    return IsLoaded(model) ? model : -1;
}
bool NativeLayout() {
    static const unsigned char ordinary[]={0x83,0xEC,0x08,0x55,0x56,0x57,0x8B,0x7C};
    static const unsigned char gang[]={0x8B,0x4C,0x24,0x04,0x6B,0xC9,0x2E};
    return reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr))==0x400000 &&
        Readable(reinterpret_cast<void*>(0x421900),sizeof(ordinary),true) &&
        Readable(reinterpret_cast<void*>(0x614490),sizeof(gang),true) &&
        std::memcmp(reinterpret_cast<void*>(0x421900),ordinary,sizeof(ordinary))==0 &&
        std::memcmp(reinterpret_cast<void*>(0x614490),gang,sizeof(gang))==0;
}
void InitializeBackend() {
    if (initialized) return;
    initialized=true;
    const auto owner=GetModuleHandleW(L"CargrpLimit.asi");
    if (owner) {
        using Query=unsigned(__cdecl*)();
        const auto version=reinterpret_cast<Query>(GetProcAddress(owner,"CGR_SelectionApiVersion"));
        const auto reg=reinterpret_cast<CgrRegisterVariation>(GetProcAddress(owner,"CGR_RegisterVariationFilter"));
        const auto state=reinterpret_cast<Query>(GetProcAddress(owner,"CGR_Status"));
        if (version && reg && state && version()==CGR_SELECTION_API_VERSION && (state()==1 || state()==2)) {
            const CgrVariationFilter filter{sizeof(CgrVariationFilter),CGR_SELECTION_API_VERSION,&Suitable,0};
            const auto result=reg(&filter);
            if (result==0 || result==1) {
                registeredPredicate=&Suitable; backend=2;
                gLogger->info("CargrpVariationFix: CargrpLimit backend registered (result={}); no native entry writes",result);
                return;
            }
            gLogger->error("CargrpVariationFix: registration refused (result={})",result);
        } else gLogger->error("CargrpVariationFix: CargrpLimit selection API unavailable; update CargrpLimit");
        // A present but unsupported owner must never trigger old-table fallback.
        backend=3; return;
    }
    if (!NativeLayout()) {
        backend=3; gLogger->error("CargrpVariationFix: unsupported native entries; no hooks installed"); return;
    }
    if (GetModuleHandleA("$fastman92limitAdjuster.asi"))
        standaloneCapacity=GetPrivateProfileIntA("CAR STREAMING","Cargrp cars per group",23,GAME_PATH((char*)"fastman92limitAdjuster_GTASA.ini"));
    if (standaloneCapacity<23 || standaloneCapacity>256) {
        backend=3; gLogger->error("CargrpVariationFix: unsupported standalone capacity"); return;
    }
    patch::ReplaceFunction(0x421900,Ordinary);
    patch::ReplaceFunction(0x614490,Gang);
    backend=1;
    gLogger->info("CargrpVariationFix: standalone backend enabled");
}
}
void CargrpVariationPatch::Inject() {
    if (backend) return;
    backend=4;
    // Defer the decision until pool initialization, before population data.
    Events::initPoolsEvent += [] { InitializeBackend(); };
    gLogger->info("CargrpVariationFix: waiting for pool initialization");
}
extern "C" __declspec(dllexport) unsigned __cdecl GrinchPatchSA_VariationBackend() { return backend; }
extern "C" __declspec(dllexport) unsigned __cdecl GrinchPatchSA_VariationRegistered() { return registeredPredicate ? 1u : 0u; }
