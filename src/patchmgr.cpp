#include "pch.h"
#include "defines.h"
#include "patchmgr.h"
#include "patches/brokenmodel.h"
#include "patches/helifix.h"
#include "patches/variation.h"

void PatchMgr::Initialize() {
    if (gConfig.ReadBoolean("Fixes", "BrokenModelFix", false)) {
        BrokenModelPatch::Inject();
        gLogger->info("BrokenModelFix enabled");
    }

    if (gConfig.ReadBoolean("Fixes", "CargrpVariationFix", false)) {
        CargrpVariationPatch::Inject();
        gLogger->info("CargrpVariationFix enabled");
    }

    if (gConfig.ReadBoolean("Fixes", "HeliFix", false)) {
        HeliFix::Inject();
        gLogger->info("HeliFix enabled");
    }
}