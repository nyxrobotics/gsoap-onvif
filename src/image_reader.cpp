#include "image_reader.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
using namespace wl;

void ImageReader::SetHikReader(const wl::CameraConfig& config)
{
  LOG(INFO) << "Test HIKIPCameraReader.";
  std::string url = "rtsp://" + config.username + ":" + config.password + "@" + config.ip + "/mpeg4/ch1/sub/av_stream";
  reader_.reset(new OpenCVCameraReader(url));
}

void ImageReader::SetDahuaReader(const wl::CameraConfig& config)
{
  LOG(INFO) << "OnvifCameraReader.";
  std::string url = "rtsp://" + config.username + ":" + config.password + "@" + config.ip + "/live";
  reader_.reset(new OpenCVCameraReader(url));
}

cv::Mat ImageReader::GetNextImage()
{
  // get one frame
  reader_->Next(&frame_);
  return frame_.img;
}

void ImageReader::StreamImage(double refresh_rate)
{
  fs::Timer timer;
  while (true)
  {
    timer.tic();
    // get one frame
    reader_->Next(&frame_);
    // show image
    cv::imshow("onvif_cam_img", frame_.img);
    cv::waitKey(1000.0 / refresh_rate);
    timer.toc();
    LOG(INFO) << "frame : " << frame_.frame_id << " cost: " << timer.total_time();
  }
}
