#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define WIN32_LEAN_AND_MEAN 

#include <plugin.h>
#include "log.h"

using namespace plugin;

#define MOD_NAME "GrinchPatchSA"
#define MOD_VERSION_NUMBER "1.0"
#define MOD_TITLE MOD_NAME " v" MOD_VERSION_NUMBER
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/GrinchPatchSA"
#define PATREON_LINK "https://www.patreon.com/grinch_"

extern std::shared_ptr<Logger> gLogger;