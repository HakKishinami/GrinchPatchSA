#include <Windows.h>
#include "pch.h"
#include "helifix.h"
#include "SimpleIni.h"

void PlayerModelBrokenFix() {
    if (FindPlayerPed()->m_nModelIndex == 0) {
        Call<0x5A81E0>(0, FindPlayerPed()->m_pPlayerData->m_pPedClothesDesc, 0xBC1C78, false);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        Events::initGameEvent += []() {
            gLogger->Print("Starting " MOD_TITLE " (" __DATE__ ")\nAuthor: Grinch_\nDiscord: "
                                        DISCORD_INVITE "\nPatreon: " PATREON_LINK "\nMore Info: " GITHUB_LINK "\n");
            
            CSimpleIniA ini;
	        SI_Error rc = ini.LoadFile(MOD_NAME".ini");
            if (rc < 0) {
                gLogger->Error("Failed to load configuration file. All fixes will be disabled!");
            }

            if (ini.GetBoolValue("Settings", "Helicopter peds fix", false)) {
                HeliFix::InjectHooks();
                gLogger->Info("Helicopter peds fix is applied.");
            }

            if (ini.GetBoolValue("Settings", "Broken player model fix", false)) {
                patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);
                gLogger->Info("Broken player model fix is applied.");
            }
        };
    }
    
    return TRUE; 
}