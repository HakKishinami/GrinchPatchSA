#pragma once
#include <plugin.h>


#include "ini.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "vkeys.h"

using namespace plugin;

extern std::shared_ptr<spdlog::logger> gLogger;
extern CIniReader gConfig;

#define LOG_NO_LEVEL(x)       \
  gLogger->set_pattern("%v"); \
  gLogger->info(x);           \
  gLogger->set_pattern("[%L] %v");

extern bool gVerboseLogging;

#define LOG_VERBOSE(fmt, ...)         \
  if (gVerboseLogging)                \
  {                                   \
    gLogger->debug(fmt, __VA_ARGS__); \
  }