#pragma once
#include <plugin.h>

#define MOD_DATA_PATH(x) PLUGIN_PATH((char *)("GrinchPatchSA/" x))
#define MOD_DATA_PATH_S(x) PLUGIN_PATH((char *)("GrinchPatchSA/" + x).c_str())

#define MOD_NAME "GrinchPatchSA"
#define MOD_VERSION "1.0"
#define MOD_VERSION_NUMBER 10000
#define MOD_TITLE MOD_NAME " v" MOD_VERSION "-alpha01"
#define DISCORD_INVITE "https://discord.gg/AduJVdyqCD"
#define GITHUB_LINK "https://github.com/user-grinch/GrinchPatchSA"
#define PATREON_LINK "https://www.patreon.com/grinch_"