#pragma once
#include <thread>
#include <string>
#include <fstream>
#include "workloop.hpp"

class ImageReader
{
public:
  ImageReader(){
    // Initialize(gpu);
  };
  void SetHikReader(const wl::CameraConfig& config);
  void SetDahuaReader(const wl::CameraConfig& config);
  cv::Mat GetNextImage();
  void StreamImage(double refresh_rate);

private:
  std::unique_ptr<wl::OpenCVCameraReader> reader_;
  wl::Frame frame_;
  // whether camera threading is on
};
