#pragma once
#include <thread>
#include <string>
#include <fstream>
#include "workloop.hpp"

class PtzCommand
{
public:
  PtzCommand(){
    // Initialize(gpu);
  };
  void SetPtzConfig(const wl::CameraConfig& config);
  void GoHome();
  void SetTarget(double pan_rad, double tilt_rad, double zoom_scale);
  void GetCurrent(double& pan_rad, double& tilt_rad, double& zoom_scale);

private:
  std::unique_ptr<wl::Ptz> ptzer_;
};
