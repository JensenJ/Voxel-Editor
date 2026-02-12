#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

class ScopedTimer {
  public:
    ScopedTimer(double& entry) : entry(entry), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration<double, std::milli>(end - start).count();
        entry = ms;
    }

  private:
    double& entry;
    std::chrono::high_resolution_clock::time_point start;
};
