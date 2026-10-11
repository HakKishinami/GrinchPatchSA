#pragma once
#include <cstdint>

// Optional selection API. The existing catalog API remains version 1.
constexpr unsigned CGR_SELECTION_API_VERSION = 1;
constexpr unsigned CGR_SELECT_ORDINARY = 1, CGR_SELECT_GANG = 2;
// Called on the game thread, at most 20 times per selection. Return 1 to accept.
// Do not reload/mutate the catalog, register filters, or call selection recursively.
using CgrVariationPredicate = unsigned(__cdecl*)(int model, unsigned purpose);
struct CgrVariationFilter {
    std::uint32_t size;
    std::uint32_t version;
    CgrVariationPredicate predicate;
    std::uint32_t reserved;
};
static_assert(sizeof(CgrVariationFilter)==16);
using CgrRegisterVariation = unsigned(__cdecl*)(const CgrVariationFilter*);
using CgrRemoveVariation = unsigned(__cdecl*)(CgrVariationPredicate);
