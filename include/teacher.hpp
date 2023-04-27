#pragma once
#include <thread>
#include <string>
#include <fstream>
#include "workloop.hpp"
#include "image_reader.hpp"
#include "ptz_command.hpp"

class TeaAnalyser
{
public:
  TeaAnalyser(){
    // Initialize(gpu);
  };
  ImageReader image_reader_;
  PtzCommand ptz_command_;
  void setConfig(const wl::CameraConfig& config);
  void Run();
  std::string ObjectNameProduce(bool realtime);

private:
};
