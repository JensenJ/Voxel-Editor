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

template <size_t N = 120> // last N frames
struct FrameTimer {
  private:
    std::array<double, N> samples{};
    size_t index = 0;
    size_t count = 0;

    void AddSample() {
        samples[index] = lastFrame;
        index = (index + 1) % N;
        if (count < N)
            ++count;
    }

  public:
    double GetAverage() const {
        if (count == 0)
            return 0.0;
        double sum = std::accumulate(samples.begin(), samples.begin() + count, 0.0);
        return sum / count;
    }

    double lastFrame = 0;
    friend class Profiler;
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
    static inline FrameTimer<> system_render_batching;
    static inline FrameTimer<> system_render_draw;
    static inline FrameTimer<> system_transform;
    static inline FrameTimer<> system_visibility;

    static void UpdateAverages() {
        GraphFrameHistory::Add(frame.lastFrame);

        frame.AddSample();

        ui.AddSample();
        ui_profiling.AddSample();
        ui_component.AddSample();
        ui_viewport.AddSample();

        ui_hierarchy.AddSample();
        ui_hierarchy_buildList.AddSample();
        ui_hierarchy_render.AddSample();

        ui_logging.AddSample();
        ui_logging_copyBuffer.AddSample();
        ui_logging_render.AddSample();

        system.AddSample();
        system_render.AddSample();
        system_render_batching.AddSample();
        system_render_draw.AddSample();

        system_transform.AddSample();
        system_visibility.AddSample();
    }
};