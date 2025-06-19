#pragma once

class CargrpVariationPatch {
public:
	CargrpVariationPatch() = delete;
    CargrpVariationPatch(CargrpVariationPatch&) = delete;

	static void Inject();
};