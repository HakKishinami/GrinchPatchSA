#include "pch.h"
#include "variation.h"
#include <CStreaming.h>
#include <CGeneral.h>

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

// 0x421900
int CCarCtrl_ChooseCarModelToLoad(int groupID)
{
    int numCarsInGroup = *GetCountInGroup(groupID);
    if (!numCarsInGroup)
    {
        return -1;
    }
    const auto model = *GetModel(groupID, CGeneral::GetRandomNumberInRange(0, numCarsInGroup));
    if (CStreaming::ms_aInfoForModel[model].m_nLoadState != 1)
    {
        CStreaming::RequestModel(model, eStreamingFlags::PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
    }

    if (CStreaming::ms_aInfoForModel[model].m_nLoadState)
    {
        return model;
    }
    else
    {
        return -1;
    }
}

// 0x614490
signed int __cdecl CPopulation_PickGangCar(int gangID)
{
    gangID = 0;
    int groupID = gangID + 18; // CARGRP_BALLAS
    const auto numCarsInGroup = *GetCountInGroup(groupID);
    if (!numCarsInGroup)
    {
        return -1;
    }

    const auto model = *GetModel(groupID, CGeneral::GetRandomNumberInRange(0, numCarsInGroup));
    if (CStreaming::ms_aInfoForModel[model].m_nLoadState != 1)
    {

        CStreaming::RequestModel(model, eStreamingFlags::KEEP_IN_MEMORY);
        CStreaming::LoadAllRequestedModels(false);
        plugin::Call<0x408000, int>(model); // CStreaming::AddToLoadedVehiclesList()
    }

    return model;
}

void CargrpVariationPatch::Inject()
{
	patch::ReplaceFunction(0x421900, CCarCtrl_ChooseCarModelToLoad);
    patch::ReplaceFunction(0x614490, CPopulation_PickGangCar);
}