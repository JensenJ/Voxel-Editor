#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

template <size_t N = 300> // last N frames
struct FrameTimer {
  private:
    std::array<float, N> samples{};
    size_t index = 0;
    size_t count = 0;
    float runningSum = 0.0;
    std::deque<std::pair<size_t, float>> maxDeque;
    size_t totalSamples = 0;

    static std::vector<FrameTimer*>& GetRegistry() {
        static std::vector<FrameTimer*> registry;
        return registry;
    }

  public:
    FrameTimer() {
        samples.fill(0.0f);
        count = N;
        GetRegistry().push_back(this);
    }

    void UpdateValues() {
        previousFrame = thisFrame;

        if (count < N) {
            samples[index] = thisFrame;
            runningSum += thisFrame;
            ++count;
        } else {
            runningSum -= samples[index];
            samples[index] = thisFrame;
            runningSum += thisFrame;
        }

        index = (index + 1) % N;

        size_t currentIndex = totalSamples++;

        while (!maxDeque.empty() && maxDeque.back().second <= thisFrame)
            maxDeque.pop_back();
        maxDeque.emplace_back(currentIndex, thisFrame);
        size_t windowStart = (totalSamples > N) ? totalSamples - N : 0;

        while (!maxDeque.empty() && maxDeque.front().first < windowStart)
            maxDeque.pop_front();
    }

    float GetAverage() const {
        if (count == 0)
            return 0.0;
        return runningSum / count;
    }

    float GetMax() const {
        if (maxDeque.empty())
            return 0.0f;
        return maxDeque.front().second;
    }

    float thisFrame = 0;
    float previousFrame = 0.0f;

    static void StartFrame() {
        for (FrameTimer* timer : GetRegistry())
            timer->thisFrame = 0.0f;
    }

    static void EndFrame() {
        for (FrameTimer* timer : GetRegistry())
            timer->UpdateValues();
    }

    // For ImGUI Rendering
    const float* GetBuffer() const { return samples.data(); }
    int GetCount() const { return static_cast<int>(count); }
    int GetOffset() const { return static_cast<int>(index); }
};

class ScopedTimer {
  public:
    ScopedTimer(FrameTimer<>& entry)
        : entry(entry), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        entry.thisFrame = ms;
    }

  private:
    FrameTimer<>& entry;
    std::chrono::high_resolution_clock::time_point start;
};

class Profiler {
  public:
    static inline FrameTimer<> frame;

    static inline FrameTimer<> ui;
    static inline FrameTimer<> ui_profiling;
    static inline FrameTimer<> ui_component;
    static inline FrameTimer<> ui_viewport;

    static inline FrameTimer<> ui_hierarchy;
    static inline FrameTimer<> ui_hierarchy_buildList;
    static inline FrameTimer<> ui_hierarchy_render;

    static inline FrameTimer<> ui_logging;
    static inline FrameTimer<> ui_logging_copyBuffer;
    static inline FrameTimer<> ui_logging_render;

    static inline FrameTimer<> system;
    static inline FrameTimer<> system_render;
    static inline FrameTimer<> system_transform;
    static inline FrameTimer<> system_visibility;

    static void StartFrame() { FrameTimer<>::StartFrame(); }
    static void EndFrame() { FrameTimer<>::EndFrame(); }
};