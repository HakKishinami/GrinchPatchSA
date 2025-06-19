#include "pch.h"
#include "brokenmodel.h"

constexpr int32_t PLAYER_MODELID = 0;

void __cdecl hkCClothes__ConstructPedModel(uint32_t modelID, CPedClothesDesc* newClothes, 
	CPedClothesDesc* oldClothes, bool bCutscenePlayer) {
	if (modelID == PLAYER_MODELID) {
		Call<0x5A81E0>(modelID, newClothes, oldClothes, bCutscenePlayer); //CClothes::ConstructPedModel
	}
}

void BrokenModelPatch::Inject()
{
	patch::ReplaceFunctionCall(0x5A834D, hkCClothes__ConstructPedModel);
}