#include "pch.h"
#include "defines.h"
#include "patchmgr.h"

std::vector<std::string> donators = {
    "Agha",
    "berrymuffin",
    "blackOS"
    "Boris Ilincic",
    "Damix",
    "Dustin Eastwood",
    "Dwolf98",
    "Francisco Flores",
    "KaiQ",
    "MC Silver",
    "Osama aj",
    "Pol3 Million",
    "Seemann",
    "spdfnpe"};

void InitLogFile()
{
    static bool flag = true;
    if (!flag)
    {
        return;
    }
    gLogger->flush_on(spdlog::level::debug);
    spdlog::set_level(spdlog::level::debug);
    gLogger->set_pattern("%v");
    gLogger->info("Starting " MOD_TITLE " (" __DATE__ ")\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nPatreon: " PATREON_LINK "\nMore Info: " GITHUB_LINK "");

    // date time
    SYSTEMTIME st;
    GetSystemTime(&st);
    gLogger->info("Date: {}-{}-{} Time: {}:{}\n", st.wYear, st.wMonth, st.wDay,
                  st.wHour, st.wMinute);
    gLogger->info("\nDonators:");
    for (const auto &name : donators)
    {
        gLogger->info("- {}", name);
    }

    gLogger->set_pattern("[%L] %v");
    flag = false;
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        gVerboseLogging = gConfig.ReadBoolean("CONFIG", "VerboseLogging", false);
        InitLogFile();
        PatchMgr::Initialize();
    }
    return TRUE;
}