#include "OnvifClientPTZ.hpp"
#include "glog/logging.h"

OnvifClientPTZ::OnvifClientPTZ(std::string url, std::string user, std::string password)
  : OnvifClientDevice(url, user, password)
{
  if (_hasPTZ)
  {
    proxyPTZ.soap_endpoint = _strUrl.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
  }
  else
  {
    throw "Camera does not implement PTZ functions";
  }
}

OnvifClientPTZ::~OnvifClientPTZ()
{
}

void OnvifClientPTZ::getConfigurations()
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__GetConfigurations* tptz__GetConfigurations = soap_new__tptz__GetConfigurations(soap, -1);
  _tptz__GetConfigurationsResponse* tptz__GetConfigurationsResponse =
      soap_new__tptz__GetConfigurationsResponse(soap, -1);

  if (SOAP_OK != proxyPTZ.GetConfigurations(tptz__GetConfigurations, tptz__GetConfigurationsResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  else
  {
    for (int i = 0; i < (int)tptz__GetConfigurationsResponse->PTZConfiguration.size(); ++i)
    {
      this->_PTZConfigurationsNames.push_back(tptz__GetConfigurationsResponse->PTZConfiguration[i]->Name);
      this->_PTZConfigurationsTokens.push_back(tptz__GetConfigurationsResponse->PTZConfiguration[i]->token);
    }
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::getStatus(std::string profileToken)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__GetStatus* tptz__GetStatus = soap_new__tptz__GetStatus(soap, -1);
  _tptz__GetStatusResponse* tptz__GetStatusResponse = soap_new__tptz__GetStatusResponse(soap, -1);

  tptz__GetStatus->ProfileToken = profileToken.c_str();

  if (SOAP_OK != proxyPTZ.GetStatus(tptz__GetStatus, tptz__GetStatusResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  else
  {
    this->_position.push_back(tptz__GetStatusResponse->PTZStatus->Position->PanTilt->x);  // pan value
    this->_position.push_back(tptz__GetStatusResponse->PTZStatus->Position->PanTilt->y);  // tilt value
    this->_position.push_back(tptz__GetStatusResponse->PTZStatus->Position->Zoom->x);     // zoom value
  }
  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::gotoPreset(std::string profileToken, int PresetToken, float Speed)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }
  _tptz__GotoPreset* go_preset = soap_new__tptz__GotoPreset(soap, -1);
  _tptz__GotoPresetResponse* go_presetResponse = soap_new__tptz__GotoPresetResponse(soap, -1);
  std::string strPresetToken = std::to_string(PresetToken);
  go_preset->ProfileToken = profileToken;
  go_preset->PresetToken = strPresetToken;
  if (SOAP_OK != proxyPTZ.GotoPreset(go_preset, go_presetResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::getPresets(std::string profileToken)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }
  struct soap* soap = soap_new();
  _tptz__GetPresets* get_presets = soap_new__tptz__GetPresets(soap);
  _tptz__GetPresetsResponse* get_presetsResponse = soap_new__tptz__GetPresetsResponse(soap);
  get_presets->ProfileToken = profileToken.c_str();
  if (SOAP_OK != proxyPTZ.GetPresets(get_presets, get_presetsResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  LOG(INFO) << get_presetsResponse;

  for (int i = 0; i < (int)get_presetsResponse->Preset.size(); ++i)
  {
    LOG(INFO) << " Preset : " << *get_presetsResponse->Preset[i]->Name
              << " Token : " << *get_presetsResponse->Preset[i]->token;
    this->_PTZPresetNames.push_back(*get_presetsResponse->Preset[i]->Name);
    this->_PTZPresetTokens.push_back(*get_presetsResponse->Preset[i]->token);
  }
  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::absoluteMove(std::string profileToken, float pan, float panSpeed, float tilt, float tiltSpeed,
                                  float zoom, float zoomSpeed)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__AbsoluteMove* tptz__AbsoluteMove = soap_new__tptz__AbsoluteMove(soap, -1);
  _tptz__AbsoluteMoveResponse* tptz__AbsoluteMoveResponse = soap_new__tptz__AbsoluteMoveResponse(soap, -1);

  tptz__AbsoluteMove->ProfileToken = profileToken;

  // setting pan and tilt
  tptz__AbsoluteMove->Position = soap_new_tt__PTZVector(soap, -1);
  tptz__AbsoluteMove->Position->PanTilt = soap_new_tt__Vector2D(soap, -1);
  tptz__AbsoluteMove->Speed = soap_new_tt__PTZSpeed(soap, -1);
  tptz__AbsoluteMove->Speed->PanTilt = soap_new_tt__Vector2D(soap, -1);
  // pan
  tptz__AbsoluteMove->Position->PanTilt->x = pan;
  tptz__AbsoluteMove->Speed->PanTilt->x = panSpeed;
  // tilt
  tptz__AbsoluteMove->Position->PanTilt->y = tilt;
  tptz__AbsoluteMove->Speed->PanTilt->y = tiltSpeed;
  // setting zoom
  tptz__AbsoluteMove->Position->Zoom = soap_new_tt__Vector1D(soap, -1);
  tptz__AbsoluteMove->Speed->Zoom = soap_new_tt__Vector1D(soap, -1);
  tptz__AbsoluteMove->Position->Zoom->x = zoom;
  tptz__AbsoluteMove->Speed->Zoom->x = zoomSpeed;

  if (SOAP_OK != proxyPTZ.AbsoluteMove(tptz__AbsoluteMove, tptz__AbsoluteMoveResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::relativeMove(std::string profileToken, float pan, float panSpeed, float tilt, float tiltSpeed,
                                  float zoom, float zoomSpeed)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__RelativeMove* tptz__RelativeMove = soap_new__tptz__RelativeMove(soap, -1);
  _tptz__RelativeMoveResponse* tptz__RelativeMoveResponse = soap_new__tptz__RelativeMoveResponse(soap, -1);

  tptz__RelativeMove->ProfileToken = profileToken;

  // setting pan and tilt
  tptz__RelativeMove->Translation = soap_new_tt__PTZVector(soap, -1);
  tptz__RelativeMove->Translation->PanTilt = soap_new_tt__Vector2D(soap, -1);
  tptz__RelativeMove->Speed = soap_new_tt__PTZSpeed(soap, -1);
  tptz__RelativeMove->Speed->PanTilt = soap_new_tt__Vector2D(soap, -1);
  // pan
  tptz__RelativeMove->Translation->PanTilt->x = pan;
  tptz__RelativeMove->Speed->PanTilt->x = panSpeed;
  // tilt
  tptz__RelativeMove->Translation->PanTilt->y = tilt;
  tptz__RelativeMove->Speed->PanTilt->y = tiltSpeed;
  // setting zoom
  tptz__RelativeMove->Translation->Zoom = soap_new_tt__Vector1D(soap, -1);
  tptz__RelativeMove->Speed->Zoom = soap_new_tt__Vector1D(soap, -1);
  tptz__RelativeMove->Translation->Zoom->x = zoom;
  tptz__RelativeMove->Speed->Zoom->x = zoomSpeed;

  if (SOAP_OK != proxyPTZ.RelativeMove(tptz__RelativeMove, tptz__RelativeMoveResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::continuousMove(std::string profileToken, float panSpeed, float tiltSpeed, float zoomSpeed)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__ContinuousMove* tptz__ContinuousMove = soap_new__tptz__ContinuousMove(soap, -1);
  _tptz__ContinuousMoveResponse* tptz__ContinuousMoveResponse = soap_new__tptz__ContinuousMoveResponse(soap, -1);

  tptz__ContinuousMove->ProfileToken = profileToken;

  // setting pan and tilt speed
  tptz__ContinuousMove->Velocity = soap_new_tt__PTZSpeed(soap, -1);
  tptz__ContinuousMove->Velocity->PanTilt = soap_new_tt__Vector2D(soap, -1);
  tptz__ContinuousMove->Velocity->PanTilt->x = panSpeed;
  tptz__ContinuousMove->Velocity->PanTilt->y = tiltSpeed;

  // setting zoom speed
  tptz__ContinuousMove->Velocity->Zoom = soap_new_tt__Vector1D(soap, -1);
  tptz__ContinuousMove->Velocity->Zoom->x = zoomSpeed;

  // int64_t tempo = 1;
  // tptz__ContinuousMove->Timeout = &tempo;

  if (SOAP_OK != proxyPTZ.ContinuousMove(tptz__ContinuousMove, tptz__ContinuousMoveResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::stop(std::string profileToken, bool panTilt, bool zoom)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__Stop* tptz__Stop = soap_new__tptz__Stop(soap, -1);
  _tptz__StopResponse* tptz__StopResponse = soap_new__tptz__StopResponse(soap, -1);

  tptz__Stop->ProfileToken = profileToken;
  tptz__Stop->PanTilt = &panTilt;
  tptz__Stop->Zoom = &zoom;

  if (SOAP_OK != proxyPTZ.Stop(tptz__Stop, tptz__StopResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::setHomePosition(std::string profileToken)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__SetHomePosition* tptz__SetHomePosition = soap_new__tptz__SetHomePosition(soap, -1);
  _tptz__SetHomePositionResponse* tptz__SetHomePositionResponse = soap_new__tptz__SetHomePositionResponse(soap, -1);

  tptz__SetHomePosition->ProfileToken = profileToken;

  if (SOAP_OK != proxyPTZ.SetHomePosition(tptz__SetHomePosition, tptz__SetHomePositionResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::goToHomePosition(std::string profileToken)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__GotoHomePosition* tptz__GotoHomePosition = soap_new__tptz__GotoHomePosition(soap, -1);
  _tptz__GotoHomePositionResponse* tptz__GotoHomePositionResponse = soap_new__tptz__GotoHomePositionResponse(soap, -1);

  tptz__GotoHomePosition->ProfileToken = profileToken;

  if (SOAP_OK != proxyPTZ.GotoHomePosition(tptz__GotoHomePosition, tptz__GotoHomePositionResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::getConfiguration(std::string configurationToken)
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tptz__GetConfiguration* tptz__GetConfiguration = soap_new__tptz__GetConfiguration(soap, -1);
  _tptz__GetConfigurationResponse* tptz__GetConfigurationResponse = soap_new__tptz__GetConfigurationResponse(soap, -1);

  tptz__GetConfiguration->PTZConfigurationToken = configurationToken;

  if (SOAP_OK != proxyPTZ.GetConfiguration(tptz__GetConfiguration, tptz__GetConfigurationResponse))
  {
    throw std::runtime_error(ErrorString());
  }

  soap_destroy(soap);
  soap_end(soap);
}

void OnvifClientPTZ::panLeft(std::string profileToken, int nDegrees)
{
  float pan = -nDegrees * 0.0027777778;
  return relativeMove(profileToken, pan, 1, 0.0, 0.0, 0.0, 0.0);
}

void OnvifClientPTZ::panRight(std::string profileToken, int nDegrees)
{
  float pan = nDegrees * 0.0027777778;
  return relativeMove(profileToken, pan, 1, 0.0, 0.0, 0.0, 0.0);
}

void OnvifClientPTZ::tiltDown(std::string profileToken, int nDegrees)
{
  float tilt = -nDegrees * 0.0055555556;
  return relativeMove(profileToken, 0.0, 0.0, tilt, 1.0, 0.0, 0.0);
}

void OnvifClientPTZ::tiltUp(std::string profileToken, int nDegrees)
{
  float tilt = nDegrees * 0.0055555556;
  return relativeMove(profileToken, 0.0, 0.0, tilt, 1.0, 0.0, 0.0);
}

void OnvifClientPTZ::zoomIn(std::string profileToken)
{
  return relativeMove(profileToken, 0.0, 0.0, 0.0, 0.0, 0.05, 1.0);
}

void OnvifClientPTZ::zoomOut(std::string profileToken)
{
  return relativeMove(profileToken, 0.0, 0.0, 0.0, 0.0, -0.05, 1.0);
}

std::vector<std::string> OnvifClientPTZ::getPTZConfigurationsNames()
{
  return _PTZConfigurationsNames;
}
std::vector<std::string> OnvifClientPTZ::getPTZConfigurationsTokens()
{
  return _PTZConfigurationsTokens;
}

std::vector<std::string> OnvifClientPTZ::getPTZPresetNames()
{
  return _PTZPresetNames;
}

std::vector<std::string> OnvifClientPTZ::getPTZPresetTokens()
{
  return _PTZPresetTokens;
}

std::vector<float> OnvifClientPTZ::getPosition()
{
  return _position;
}

std::string OnvifClientPTZ::ErrorString()
{
  std::string result = "";
  result += std::to_string(proxyPTZ.soap->error);
  result += " FaultString : ";
  if (*soap_faultstring(proxyPTZ.soap))
  {
    std::string faultstring(*soap_faultstring(proxyPTZ.soap));
    result += faultstring;
  }
  else
  {
    result += "null";
  }
  result += " FaultCode : ";
  if (*soap_faultcode(proxyPTZ.soap))
  {
    std::string faultcode(*soap_faultcode(proxyPTZ.soap));
    result += faultcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultSubcode : ";
  if (*soap_faultsubcode(proxyPTZ.soap))
  {
    std::string faultsubcode(*soap_faultsubcode(proxyPTZ.soap));
    result += faultsubcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultDetail : ";
  if (*soap_faultdetail(proxyPTZ.soap))
  {
    std::string faultdetail(*soap_faultdetail(proxyPTZ.soap));
    result += faultdetail;
  }
  else
  {
    result += "null";
  }
  return result;
}
