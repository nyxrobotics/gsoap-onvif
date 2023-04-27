#include "teacher.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
using namespace wl;

void TeaAnalyser::SetHikReader(const wl::CameraConfig& config)
{
  LOG(INFO) << "Test HIKIPCameraReader.";
  std::string url = "rtsp://" + config.username + ":" + config.password + "@" + config.ip + "/mpeg4/ch1/sub/av_stream";
  reader_.reset(new OpenCVCameraReader(url));
  ptzer_.reset(new Ptz(config));
}

void TeaAnalyser::SetDahuaReader(const wl::CameraConfig& config)
{
  LOG(INFO) << "OnvifCameraReader.";
  std::string url = "rtsp://" + config.username + ":" + config.password + "@" + config.ip + "/live";
  reader_.reset(new OpenCVCameraReader(url));
  ptzer_.reset(new Ptz(config));
}

void TeaAnalyser::Run()
{
  fs::Timer timer;
  std::vector<int> PresetTokens = { 1, 2 };
  // initialize motors
  ptzer_->ReturnOrigin();
  usleep(1000000);
  float pan_rad, tilt_rad, zoom_scale;
  while (!cam_is_stop_)
  {
    timer.tic();
    for (auto& PresetToken : PresetTokens)
    {
      // get current angle
      ptzer_->GetPTZ(pan_rad, tilt_rad, zoom_scale);
      // get one frame
      reader_->Next(&frame_);
      // show image
      cv::imshow("onvif_cam_img", frame_.img);
      cv::waitKey(3.3);
      // save one frame
      // std::string frame_path = "~/blackboard.jpg";
      // cv::imwrite(frame_path, frame_.img);
      timer.toc();
      LOG(INFO) << "frame : " << frame_.frame_id << " cost: " << timer.total_time();
    }
  }
}
