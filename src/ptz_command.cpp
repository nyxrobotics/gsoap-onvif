#include "ptz_command.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
using namespace wl;

void PtzCommand::SetPtzConfig(const wl::CameraConfig& config)
{
  ptzer_.reset(new Ptz(config));
}

void PtzCommand::GoHome()
{
  ptzer_->ReturnOrigin();
}

void PtzCommand::SetTarget(double pan_rad, double tilt_rad, double zoom_scale)
{
  float pan_abs, tilt_abs, zoom_abs;
  pan_abs = static_cast<float>(min(1.0, max(-1.0, pan_rad / M_PI)));
  tilt_abs = static_cast<float>(min(1.0, max(-1.0, (tilt_rad + M_PI * 0.2083) / (0.2917 * M_PI))));
  zoom_abs = static_cast<float>(min(1.0, max(0.0, (zoom_scale - 1.0) / 37.5 + 0.04)));

  ptzer_->SetPTZ(pan_abs, tilt_abs, zoom_abs);
}

void PtzCommand::GetCurrent(double& pan_rad, double& tilt_rad, double& zoom_scale)
{
  float pan_abs, tilt_abs, zoom_abs;
  ptzer_->GetPTZ(pan_abs, tilt_abs, zoom_abs);
  pan_rad = static_cast<double>(M_PI * pan_abs);
  tilt_rad = static_cast<double>(M_PI * 0.2917 * tilt_abs - M_PI * 0.2083);
  zoom_scale = static_cast<double>(1.0 + 37.5 * (zoom_abs - 0.04));
  LOG(INFO) << "pan = " << pan_abs;
  LOG(INFO) << "tilt = " << tilt_abs;
  LOG(INFO) << "zoom = " << zoom_abs;
}
