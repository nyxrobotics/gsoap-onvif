#include "teacher.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
