#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

template <size_t N = 120> // last N frames
struct FrameTimer {
  private:
    std::array<float, N> samples{};
    size_t index = 0;
    size_t count = 0;
    float runningSum = 0.0;

  public:
    FrameTimer() {
        samples.fill(0.0f);
        count = N;
    }

    void UpdateAverage() {
        if (count < N) {
            samples[index] = lastFrame;
            runningSum += lastFrame;
            ++count;
        } else {
            runningSum -= samples[index];
            samples[index] = lastFrame;
            runningSum += lastFrame;
        }

        index = (index + 1) % N;
    }

    float GetAverage() const {
        if (count == 0)
            return 0.0;
        return runningSum / count;
    }

    float lastFrame = 0;

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
        entry.lastFrame = ms;
        entry.UpdateAverage();
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
};