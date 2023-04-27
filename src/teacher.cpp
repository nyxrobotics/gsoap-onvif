#include "teacher.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <thread>

using namespace cv;
using namespace std;
using namespace wl;

void TeaAnalyser::setConfig(const wl::CameraConfig& config)
{
  image_reader_.SetDahuaReader(config);
  ptz_command_.SetPtzConfig(config);
}

void TeaAnalyser::Run()
{
  fs::Timer timer;
  // initialize motors
  ptz_command_.GoHome();
  usleep(1000000);
  while (true)
  {
    timer.tic();
    // set target angle
    ptz_command_.SetTarget(3.14, 0, 1.0);
    // get current angle
    double pan_rad, tilt_rad, zoom_scale;
    ptz_command_.GetCurrent(pan_rad, tilt_rad, zoom_scale);
    // show image
    cv::imshow("onvif_cam_img", image_reader_.GetNextImage());
    cv::waitKey(1000.0 / 30.0);
    timer.toc();
    // LOG(INFO) << "frame : " << frame_.frame_id << " cost: " << timer.total_time();
  }
}

void TeaAnalyser::runThreads()
{
  // initialize motors
  ptz_command_.GoHome();
  usleep(1000000);
  pan_target = M_PI;
  tilt_target = 0;
  zoom_target = 1;
  std::thread th1(&TeaAnalyser::imageThread, this);
  std::thread th2(&TeaAnalyser::ptzThread, this);
  th1.join();
  th2.join();
}

void TeaAnalyser::imageThread()
{
  while (true)
  {
    cv::imshow("onvif_cam_img", image_reader_.GetNextImage());
    cv::waitKey(1000.0 / 30.0);
  }
}

void TeaAnalyser::ptzThread()
{
  int i = 0;
  while (true)
  {
    pan_target = sin(static_cast<double>(i)) * M_PI;
    i++;
    // set target angle
    ptz_command_.SetTarget(pan_target, tilt_target, zoom_target);
    LOG(INFO) << "target : " << pan_target << "," << tilt_target << "," << zoom_target;
    // get current angle
    ptz_command_.GetCurrent(pan_current, tilt_current, zoom_current);
    LOG(INFO) << "current : " << pan_current << "," << tilt_current << "," << zoom_current;
  }
}
