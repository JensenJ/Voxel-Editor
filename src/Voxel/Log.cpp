#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> Log::Logger;

void Log::Init() {
    Logger = spdlog::stdout_color_mt("VOXEL");
    Logger->set_pattern("%^[%T.%e] [%n:%l] %v%$");
    Logger->set_level(spdlog::level::trace);

    LOG_TRACE("Logger has initialised");
}

std::shared_ptr<spdlog::logger>& Log::GetLogger() { return Logger; }
