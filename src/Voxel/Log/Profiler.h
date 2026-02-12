#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct GraphFrameHistory {
  public:
    static constexpr int GRAPH_FRAME_HISTORY_COUNT = 200;

    static inline float values[GRAPH_FRAME_HISTORY_COUNT] = {};
    static inline int index = 0;

    static void Add(float frameTimeMs) {
        values[index] = frameTimeMs;
        index = (index + 1) % GRAPH_FRAME_HISTORY_COUNT;
    }
};

template <size_t N = 60> // last N frames
struct FrameTimer {
    std::array<double, N> samples{};
    size_t index = 0;
    size_t count = 0;

    void AddSample(double ms) {
        samples[index] = ms;
        index = (index + 1) % N;
        if (count < N)
            ++count;
    }

    double GetAverage() const {
        if (count == 0)
            return 0.0;
        double sum = std::accumulate(samples.begin(), samples.begin() + count, 0.0);
        return sum / count;
    }
};

class Profiler {
  public:
    static inline double frame;

    static inline double ui;
    static inline double uiProfiling;
    static inline double uiComponent;
    static inline double uiViewport;
    static inline double uiHierarchy;
    static inline double uiLogging;

    static inline double system;
    static inline double systemRender;
    static inline double systemRenderBatching;
    static inline double systemRenderDraw;
    static inline double systemTransform;
    static inline double systemVisibility;

    static inline FrameTimer<> frameAvg;

    static inline FrameTimer<> uiAvg;
    static inline FrameTimer<> uiProfilingAvg;
    static inline FrameTimer<> uiComponentAvg;
    static inline FrameTimer<> uiViewportAvg;
    static inline FrameTimer<> uiHierarchyAvg;
    static inline FrameTimer<> uiLoggingAvg;

    static inline FrameTimer<> systemAvg;
    static inline FrameTimer<> systemRenderAvg;
    static inline FrameTimer<> systemRenderBatchingAvg;
    static inline FrameTimer<> systemRenderDrawAvg;
    static inline FrameTimer<> systemTransformAvg;
    static inline FrameTimer<> systemVisibilityAvg;

    static void UpdateAverages() {
        GraphFrameHistory::Add(frame);

        frameAvg.AddSample(frame);

        uiAvg.AddSample(ui);
        uiProfilingAvg.AddSample(uiProfiling);
        uiComponentAvg.AddSample(uiComponent);
        uiViewportAvg.AddSample(uiViewport);
        uiHierarchyAvg.AddSample(uiHierarchy);
        uiLoggingAvg.AddSample(uiLogging);

        systemAvg.AddSample(system);
        systemRenderAvg.AddSample(systemRender);
        systemRenderBatchingAvg.AddSample(systemRenderBatching);
        systemRenderDrawAvg.AddSample(systemRenderDraw);

        systemTransformAvg.AddSample(systemTransform);
        systemVisibilityAvg.AddSample(systemVisibility);
    }
};