#include <deque>
#include <mutex>
#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

struct LogEntry {
    std::tm time;
    long long ms;
    size_t thread_id;
    spdlog::level::level_enum level;
    std::string message;
};

template <typename Mutex> class ImGuiLogSink : public spdlog::sinks::base_sink<Mutex> {
  public:
    std::deque<LogEntry> buffer;
    std::mutex bufferMutex;
    bool updated = false;

    std::vector<LogEntry> GetBufferCopy() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        return {buffer.begin(), buffer.end()};
    }

    void ClearBuffer() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.clear();
        updated = true;
    }

  protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        this->formatter_->format(msg, formatted);

        std::lock_guard<Mutex> lock(bufferMutex);

        auto tp = msg.time;
        auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - secs).count();

        auto tt = std::chrono::system_clock::to_time_t(secs);
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif

        buffer.push_back({tm, ms, msg.thread_id, msg.level,
                          std::string(msg.payload.data(), msg.payload.size())});

        if (buffer.size() > 1000)
            buffer.pop_front();
        updated = true;
    }

    void flush_() override {}
};

using ImGuiSinkMT = ImGuiLogSink<std::mutex>;
using ImGuiSinkST = ImGuiLogSink<spdlog::details::null_mutex>;
