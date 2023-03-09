#include "OnvifClientDevice.hpp"
#include "DeviceBinding.nsmap"
#include "glog/logging.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

OnvifClientDevice::OnvifClientDevice()
{
}

OnvifClientDevice::OnvifClientDevice(std::string url, std::string user, std::string password)
{
  _strUrl = "http://" + url + "/onvif/services";
  _user = user;
  _password = password;

  _hasMedia = false;
  _hasPTZ = false;

  proxyDevice.soap_endpoint = _strUrl.c_str();

  soap_register_plugin(proxyDevice.soap, soap_wsse);
  soap = soap_new();

  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, user.c_str(), password.c_str()))
  {
    std::string errorDetail;
    if (soap->fault != NULL)
    {
      errorDetail += "ERROR:\nError Code:";
      if (soap->fault != NULL)
        errorDetail += soap->fault->faultcode;
      errorDetail += "\nFault:";
      if (soap->fault != NULL)
        errorDetail += soap->fault->faultstring;
      errorDetail + "\n";
      throw std::runtime_error(errorDetail);
    }
  }

  _tds__GetCapabilities* tds__GetCapabilities = soap_new__tds__GetCapabilities(soap, -1);
  tds__GetCapabilities->Category.push_back(tt__CapabilityCategory__All);

  _tds__GetCapabilitiesResponse* tds__GetCapabilitiesResponse = soap_new__tds__GetCapabilitiesResponse(soap, -1);

  if (SOAP_OK == proxyDevice.GetCapabilities(tds__GetCapabilities, tds__GetCapabilitiesResponse))
  {
    if (tds__GetCapabilitiesResponse->Capabilities->Media != NULL)
    {
      _hasMedia = true;
    }

    if (tds__GetCapabilitiesResponse->Capabilities->PTZ != NULL)
    {
      _hasPTZ = true;
    }
  }
  else
  {
    std::string errorDetail;
    if (soap->fault != NULL)
    {
      errorDetail += "ERROR:\nError Code: ";
      errorDetail += soap->fault->faultcode;
      errorDetail += "\nFault: ";
      errorDetail += soap->fault->faultstring;
      errorDetail + "\n";
      throw std::runtime_error(errorDetail);
    }
    else
    {
      errorDetail += "Error: ";
      errorDetail += proxyDevice.soap->msgbuf;
      throw std::runtime_error(errorDetail);
    }
  }

  soap_destroy(soap);
  soap_end(soap);
}

OnvifClientDevice::~OnvifClientDevice()
{
}

void OnvifClientDevice::getUsers()
{
  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, _user.c_str(), _password.c_str()))
  {
    throw std::runtime_error(ErrorString());
  }

  _tds__GetUsers* tds_GetUsers = soap_new__tds__GetUsers(soap, -1);
  _tds__GetUsersResponse* tds_GetUsersResponse = soap_new__tds__GetUsersResponse(soap, -1);

  if (SOAP_OK != proxyDevice.GetUsers(tds_GetUsers, tds_GetUsersResponse))
  {
    throw std::runtime_error(ErrorString());
  }
  else
  {
    for (int i = 0; i < (int)tds_GetUsersResponse->User.size(); ++i)
    {
      this->_username.push_back(tds_GetUsersResponse->User[i]->Username);
    }
  }
}

std::vector<std::string> OnvifClientDevice::getUsernames()
{
  return _username;
}

std::string OnvifClientDevice::ErrorString()
{
  std::string result = "";
  result += std::to_string(proxyDevice.soap->error);
  result += " FaultString : ";
  if (*soap_faultstring(proxyDevice.soap))
  {
    std::string faultstring(*soap_faultstring(proxyDevice.soap));
    result += faultstring;
  }
  else
  {
    result += "null";
  }
  result += " FaultCode : ";
  if (*soap_faultcode(proxyDevice.soap))
  {
    std::string faultcode(*soap_faultcode(proxyDevice.soap));
    result += faultcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultSubcode : ";
  if (*soap_faultsubcode(proxyDevice.soap))
  {
    std::string faultsubcode(*soap_faultsubcode(proxyDevice.soap));
    result += faultsubcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultDetail : ";
  if (*soap_faultdetail(proxyDevice.soap))
  {
    std::string faultdetail(*soap_faultdetail(proxyDevice.soap));
    result += faultdetail;
  }
  else
  {
    result += "null";
  }
  return result;
}
