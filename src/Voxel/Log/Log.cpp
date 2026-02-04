#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
std::shared_ptr<spdlog::logger> Log::Logger;

void Log::Init() {
    Logger = spdlog::stdout_color_mt("VOXEL");
    const char* formatString = "%^[%Y-%m-%d] [%T.%e] [T%t] [%l] %v%$";
    Logger->set_pattern(formatString);
    imguiSink = std::make_shared<ImGuiSinkMT>();
    imguiSink.get()->set_pattern(formatString);
    Logger->sinks().push_back(imguiSink);
    Logger->set_level(spdlog::level::trace);

    LOG_INFO("Logger has initialised");
}

std::shared_ptr<spdlog::logger>& Log::GetLogger() { return Logger; }

std::shared_ptr<ImGuiSinkMT> Log::GetImGuiLogSink() { return imguiSink; }
