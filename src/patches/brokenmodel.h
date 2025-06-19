#pragma once

class BrokenModelPatch {
public:
	BrokenModelPatch() = delete;
    BrokenModelPatch(BrokenModelPatch&) = delete;

	static void Inject();
};