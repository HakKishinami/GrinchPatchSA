#include "pch.h"
#include "variation.h"
#include <CStreaming.h>
#include <CGeneral.h>
#include <CLoadedCarGroup.h>
#include <CTheScripts.h>
#include <eVehicleClass.h>

short *GetCountInGroup(int groupID) {
    short *&m_nNumCarsInGroup = *(short**)0x421915;
    return &m_nNumCarsInGroup[groupID];
}

static int carsPerGroup = 23;
short *GetModel(int groupID, int idx) {
    if (GetModuleHandle("$fastman92limitAdjuster.asi")) {
        carsPerGroup = GetPrivateProfileIntA("CAR STREAMING", "Cargrp cars per group", 23, GAME_PATH((char*)"fastman92limitAdjuster_GTASA.ini"));
    }
    short** m_CarGroups = *(short***)0x421948;
    return (short*)((int)m_CarGroups + 2 * carsPerGroup * groupID + 2 * idx);
}

bool IsModelSuitable(int model, bool bNotTooManyInTheWorld) {
    if (plugin::CallAndReturn<bool, 0x46A810>(model)) return false;
    if (CTheScripts::HasVehicleModelBeenBlockedByScript(model)) return false;
    if (CStreaming::WeAreTryingToPhaseVehicleOut(model)) return false;
    if (bNotTooManyInTheWorld && CModelInfo::GetModelInfo(model)->m_nRefCount > 2) return false;
    return true;
}

int GetSuitableModel(int groupID) {
    int numCarsInGroup = *GetCountInGroup(groupID);
    if (!numCarsInGroup)
    {
        return -1;
    }

    int model = -1;
    for (int i = 0; i < 20; i++) {
        model = *GetModel(groupID, CGeneral::GetRandomNumberInRange(0, numCarsInGroup+1));
        if (IsModelSuitable(model, true)) {
            break;
        }
    }
    return model;
}

// 0x421900
int CCarCtrl_ChooseCarModelToLoad(int groupID)
{
    return GetSuitableModel(groupID);
}

// 0x614490
signed int __cdecl CPopulation_PickGangCar(int gangID)
{
    int groupID = gangID + 18; // CARGRP_BALLAS
    int model = GetSuitableModel(groupID);

    if (CStreaming::ms_aInfoForModel[model].m_nLoadState != 1)
    {
        CStreaming::RequestModel(model, eStreamingFlags::PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
    }
    return GetSuitableModel(groupID);
}

void CargrpVariationPatch::Inject()
{
    patch::ReplaceFunction(0x421900, CCarCtrl_ChooseCarModelToLoad);
    patch::ReplaceFunction(0x614490, CPopulation_PickGangCar);
}