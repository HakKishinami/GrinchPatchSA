#pragma once
#include <CPools.h>

class HeliFix {
public:
    HeliFix() = delete;
    HeliFix(HeliFix&) = delete;

    static void Inject();
};