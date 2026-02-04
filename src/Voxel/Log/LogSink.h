#include <deque>
#include <mutex>
#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

struct LogEntry {
    std::chrono::system_clock::time_point time;
    size_t thread_id;
    spdlog::level::level_enum level;
    std::string message;
};

template <typename Mutex> class ImGuiLogSink : public spdlog::sinks::base_sink<Mutex> {
  public:
    std::deque<LogEntry> buffer;
    std::mutex bufferMutex;

    std::vector<LogEntry> GetBufferCopy() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        return {buffer.begin(), buffer.end()};
    }

    void ClearBuffer() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.clear();
    }

  protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        this->formatter_->format(msg, formatted);

        std::lock_guard<Mutex> lock(bufferMutex);
        buffer.push_back({msg.time, msg.thread_id, msg.level,
                          std::string(msg.payload.data(), msg.payload.size())});

        if (buffer.size() > 1000)
            buffer.pop_front();
    }

    void flush_() override {}
};

using ImGuiSinkMT = ImGuiLogSink<std::mutex>;
using ImGuiSinkST = ImGuiLogSink<spdlog::details::null_mutex>;
