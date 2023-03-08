#include <chrono>
#include <iostream>
#include <sstream>
#include "glog/logging.h"
#include "OnvifClientDevice.hpp"
#include "OnvifClientPTZ.hpp"
#include "OnvifClientMedia.hpp"

using namespace std;
const std::string PROFILE_NAME = "MediaProfile_Channel1_MainStream";
const std::string PROFILE_TOKEN = "MediaProfile00000";
const std::string PTZ_CONF_TOKEN = "conf_token_001";
const std::string SOURCEVIDEO_CONF_TOKEN = "video_token";
const std::string IPV4 = "192.168.128.66";
const std::string PORT = "80";  // HTTP Port (Optional)
const std::string USERNAME = "";
const std::string PASSWORD = "";

int main(int argc, const char* argv[])
{
  printf("Connect ONVIF device\n");
  std::string url = IPV4;
  if (PORT != "")
  {
    url += ":" + PORT;
  }
  std::string name = USERNAME;
  std::string password = PASSWORD;
  OnvifClientPTZ* PTZ = new OnvifClientPTZ(url, name, password);
  OnvifClientMedia* Media = new OnvifClientMedia(url, name, password);
  printf("Get Profiles\n");
  Media->getProfiles();
  for (int i = 0; i < (int)Media->getProfilesTokens().size(); ++i)
  {
    std::cout << Media->getProfilesNames()[i] << "/" << Media->getProfilesTokens()[i] << std::endl;
  }

  // Media->createProfile(PROFILE_NAME, PROFILE_TOKEN);
  // Media->addPTZConfiguration(PROFILE_TOKEN,PTZ_CONF_TOKEN);

  // Media->addVideoSourceConfiguration(PROFILE_TOKEN,SOURCEVIDEO_CONF_TOKEN)
  // Media->addPTZConfiguration(get_profilesResponse[0].profileToken,PTZ_CONF_TOKEN);
  // PTZ->getConfiguration(PTZ_CONF_TOKEN);
  // Media->addPTZConfiguration(PROFILE_TOKEN, "PtzConfiguration");
  // PTZ->getConfiguration(get_profilesResponse[0].profileToken);
  // PTZ->getConfiguration("PtzConfiguration");
  // PTZ->gotoPreset(get_profilesResponse[0].profileToken,3,0.5);
  // PTZ->getPresets(get_profilesResponse[0].profileToken);
  // PTZ->gotoPreset(get_profilesResponse[0].profileToken,3,0.5);
  // Media->createProfile("media_profile_name", get_profilesResponse[0].profileToken);
  // Media->addPTZConfiguration(get_profilesResponse[0].profileToken, "ptz_configuration_token");
  // PTZ->getPresets(get_profilesResponse[0].profileToken);
  // PTZ->getPresets(get_profilesResponse[1].profileToken);
  // PTZ->getPresets(get_profilesResponse[2].profileToken);
  // return 0;
  // Media->deleteProfile("test");
  // std::cout << "Configurations:" << std::endl;
  // Media->addPTZConfiguration("Move", "PTZToken");
  // PTZ->getConfiguration(PROFILE_TOKEN);
  // PTZ->getPTZConfigurations();
  // PTZ->setHomePosition(PROFILE_TOKEN);

  printf("Initialize Joints\n");

  PTZ->continuousMove(PROFILE_TOKEN, 0, -1, 0);
  usleep(7000000);
  PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
  usleep(100000);
  PTZ->continuousMove(PROFILE_TOKEN, 0, -1, 0);
  usleep(7000000);
  PTZ->goToHomePosition(PROFILE_TOKEN);
  usleep(900000);

  printf("Start Continuous Move\n");
  for (int i = 0; i < 1; i++)
  {
    PTZ->continuousMove(PROFILE_TOKEN, 1, 0, 0);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
    PTZ->continuousMove(PROFILE_TOKEN, -1, 0, 0);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 1, 0);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, -1, 0);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 1);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, -1);
    usleep(800000);
    PTZ->continuousMove(PROFILE_TOKEN, 0, 0, 0);
    usleep(200000);
  }
  // PTZ->stop(PROFILE_TOKEN, 0, 0);

  printf("Start Reative Move\n");
  PTZ->relativeMove(PROFILE_TOKEN, 1, 1, 1, 1, 1, 1);
  sleep(1);
  PTZ->relativeMove(PROFILE_TOKEN, 0, 1, 0, 1, 0, 1);
  sleep(2);

  printf("Start Absolute Move\n");
  PTZ->absoluteMove(PROFILE_TOKEN, 0, 1, 0, 1, 0, 1);
  sleep(2);

  // zoom
  // printf("Start zoom\n");
  // PTZ->ZoomIn("Move");
  // sleep(2);
  // PTZ->ZoomOut("Move");
  // sleep(2);

  printf("Go To Home Position\n");
  PTZ->goToHomePosition(PROFILE_TOKEN);
  usleep(100000);
  printf("Getting Status\n");
  PTZ->getStatus(PROFILE_TOKEN);

  return 0;
}
