#pragma once
#include <thread>
#include <string>
#include <fstream>
#include "workloop.hpp"

class StableLocation
{
public:
  int x_ = -1;
  int y_ = -1;
  int frame_num_ = -1;
};

class TeaAnalyser
{
public:
  TeaAnalyser(){
    // Initialize(gpu);
  };
  void SetHikReader(const wl::CameraConfig& config);
  void SetDahuaReader(const wl::CameraConfig& config);
  void Run();
  std::string ObjectNameProduce(bool realtime);

private:
  std::unique_ptr<wl::OpenCVCameraReader> reader_;
  std::unique_ptr<wl::Ptz> ptzer_;
  wl::Frame frame_;
  // cv::Rect bbox_new_;
  //  Frame count of last human detected
  int last_person_frame_ = -1;
  // Frame count of last hand raised
  int raising_frame_ = -1;
  // Stable position
  StableLocation last_stable_location_;
  // Whether to pull the lens closer, for the case of people standing still
  bool zoom_in_ = false;
  // Whether to pull back the lens
  bool zoom_out_ = false;
  // whether camera threading is on
  bool cam_is_stop_ = false;
  // upload file oss link
};
