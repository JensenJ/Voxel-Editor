#pragma once
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#define LOG_TRACE(...) Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) Log::GetLogger()->critical(__VA_ARGS__)

class Log {
  public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger();

  private:
    static std::shared_ptr<spdlog::logger> Logger;
};
