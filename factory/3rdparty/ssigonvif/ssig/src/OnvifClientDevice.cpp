#include "OnvifClientDevice.hpp"
#include <iostream>
#include <sstream>
#include "glog/logging.h"
#include "DeviceBinding.nsmap"

OnvifClientDevice::OnvifClientDevice(std::string url, std::string user, std::string password, bool showCapabilities)
{
  syncTime(url, user, password);
  device_url_ = "http://" + url + "/onvif/device_service";
  user_ = user;
  passwd_ = password;

  has_media_ = false;
  has_ptz_ = false;

  proxy_device_.soap_endpoint = device_url_.c_str();

  soap_register_plugin(proxy_device_.soap, soap_wsse);
  soap_ = soap_new();

  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxy_device_.soap, NULL, user.c_str(), password.c_str()))
  {
    LOG(INFO) << "Device binding 1 Error";
  }

  if (SOAP_OK != soap_wsse_add_Timestamp(proxy_device_.soap, "Time", 10))
  {
    LOG(INFO) << "Device binding 2 Error";
  }

  auto* get_cap = soap_new__tds__GetCapabilities(soap_, -1);
  get_cap->Category.push_back(tt__CapabilityCategory__All);
  auto* response = soap_new__tds__GetCapabilitiesResponse(soap_, -1);

  if (SOAP_OK == proxy_device_.GetCapabilities(get_cap, response))
  {
    if (response->Capabilities->Media != NULL)
    {
      has_media_ = true;

      if (showCapabilities)
      {
        LOG(INFO) << "--------------------------Media-----------------------";
        LOG(INFO) << "XAddr : " << response->Capabilities->Media->XAddr;
      }
      media_url_ = response->Capabilities->Media->XAddr;
    }

    if (response->Capabilities->PTZ != NULL)
    {
      has_ptz_ = true;
      if (showCapabilities)
      {
        LOG(INFO) << "--------------------------PTZ-------------------------";
        LOG(INFO) << "XAddr : " << response->Capabilities->PTZ->XAddr;
      }
      ptz_url_ = response->Capabilities->PTZ->XAddr;
    }
  }
  else
  {
    LOG(FATAL) << "Error : " << ErrorString();
  }
  soap_destroy(soap_);
  soap_end(soap_);
}

OnvifClientDevice::~OnvifClientDevice()
{
}

bool OnvifClientDevice::syncTime(std::string url, std::string user, std::string password)
{
  // Connect camera
  device_url_ = "http://" + url + "/onvif/device_service";
  user_ = user;
  passwd_ = password;
  has_media_ = false;
  has_ptz_ = false;
  proxy_device_.soap_endpoint = device_url_.c_str();
  int soap_result;
  soap_result = soap_wsse_add_UsernameTokenDigest(proxy_device_.soap, NULL, user.c_str(), password.c_str());
  if (SOAP_OK != soap_result)
  {
    printf("ERROR: %d - \nsoap_wsse_add_UsernameTokenDigestOffset: %s", soap_result, proxy_device_.soap_fault_detail());
    return false;
  }
  soap_result = soap_wsse_add_Timestamp(proxy_device_.soap, "Time", 10);
  if (SOAP_OK != soap_result)
  {
    printf("ERROR: %d - \nsoap_wsse_add_Timestamp: %s", soap_result, proxy_device_.soap_fault_detail());
    return false;
  }

  // Get Camera Time
  time_t cam_gmt_time;
  time_t pc_gmt_time;
  std::string cam_time_zone;
  long time_offset = 0;
  _tds__GetSystemDateAndTime get_request;
  _tds__GetSystemDateAndTimeResponse get_response;
  soap_result = proxy_device_.GetSystemDateAndTime(&get_request, &get_response);
  if (soap_result != SOAP_OK)
  {
    printf("ERROR: %d - \nGetSystemDateAndTime: %s", soap_result, proxy_device_.soap_fault_detail());
    return false;
  }
  printf("--- Camera Local Time ---\n");
  printf("DST: %d \n", get_response.SystemDateAndTime->DaylightSavings);
  printf("TZ: %s \n", get_response.SystemDateAndTime->TimeZone->TZ.c_str());
  printf("NTP: %s \n", get_response.SystemDateAndTime->DateTimeType == 0 ? "yes" : "no");
  printf("Date: %4d-%2d-%2d - \n", get_response.SystemDateAndTime->LocalDateTime->Date->Year,
         get_response.SystemDateAndTime->LocalDateTime->Date->Month,
         get_response.SystemDateAndTime->LocalDateTime->Date->Day);
  printf("Time: %2d:%2d:%2d \n", get_response.SystemDateAndTime->LocalDateTime->Time->Hour,
         get_response.SystemDateAndTime->LocalDateTime->Time->Minute,
         get_response.SystemDateAndTime->LocalDateTime->Time->Second);
  printf("--- Camera GMT Time ---\n");
  printf("DST: %d \n", get_response.SystemDateAndTime->DaylightSavings);
  printf("NTP: %s \n", get_response.SystemDateAndTime->DateTimeType == 0 ? "yes" : "no");
  printf("Date: %4d-%2d-%2d - \n", get_response.SystemDateAndTime->UTCDateTime->Date->Year,
         get_response.SystemDateAndTime->UTCDateTime->Date->Month,
         get_response.SystemDateAndTime->UTCDateTime->Date->Day);
  printf("Time: %2d:%2d:%2d \n", get_response.SystemDateAndTime->UTCDateTime->Time->Hour,
         get_response.SystemDateAndTime->UTCDateTime->Time->Minute,
         get_response.SystemDateAndTime->UTCDateTime->Time->Second);
  struct tm cam_gmt_tm;
  cam_gmt_tm.tm_sec = get_response.SystemDateAndTime->UTCDateTime->Time->Second;
  cam_gmt_tm.tm_min = get_response.SystemDateAndTime->UTCDateTime->Time->Minute;
  cam_gmt_tm.tm_hour = get_response.SystemDateAndTime->UTCDateTime->Time->Hour;
  cam_gmt_tm.tm_mday = get_response.SystemDateAndTime->UTCDateTime->Date->Day;
  cam_gmt_tm.tm_mon = get_response.SystemDateAndTime->UTCDateTime->Date->Month - 1;
  cam_gmt_tm.tm_year = get_response.SystemDateAndTime->UTCDateTime->Date->Year - 1900;
  cam_gmt_tm.tm_isdst = get_response.SystemDateAndTime->DaylightSavings;
  cam_gmt_tm.tm_zone = "GMT";
  cam_gmt_tm.tm_gmtoff = 0;
  cam_time_zone = get_response.SystemDateAndTime->TimeZone->TZ;
  cam_gmt_time = mktime(&cam_gmt_tm);
  printf("Cam GMT Time: %ld\n", cam_gmt_time);

  // Get PC GMT time
  time_t pc_local_time = time(0);
  tm* pc_local_tm = localtime(&pc_local_time);
  tm* pc_gmt_tm = gmtime(&pc_local_time);
  pc_gmt_time = mktime(pc_gmt_tm);
  printf("--- PC local Time ---\n");
  printf("DST: %d \n", pc_local_tm->tm_isdst);
  printf("TZ: %s \n", pc_local_tm->tm_zone);
  printf("Date: %4d-%2d-%2d - \n", pc_local_tm->tm_year + 1900, pc_local_tm->tm_mon + 1, pc_local_tm->tm_mday);
  printf("Time: %2d:%2d:%2d \n", pc_local_tm->tm_hour, pc_local_tm->tm_min, pc_local_tm->tm_sec);
  printf("Local GMT Time: %ld\n", pc_gmt_time);
  printf("--- PC GMT Time ---\n");
  printf("DST: %d \n", pc_gmt_tm->tm_isdst);
  printf("Date: %4d-%2d-%2d - \n", pc_gmt_tm->tm_year + 1900, pc_gmt_tm->tm_mon + 1, pc_gmt_tm->tm_mday);
  printf("Time: %2d:%2d:%2d \n", pc_gmt_tm->tm_hour, pc_gmt_tm->tm_min, pc_gmt_tm->tm_sec);
  printf("Local GMT Time: %ld\n", pc_gmt_time);
  printf("--- Time Offset Seconds ---\n");
  time_offset = cam_gmt_time - pc_gmt_time;
  printf("Time Offset: %ld\n", time_offset);
  if (abs(time_offset) < 2)
  {
    printf("Time is already synchronized\n");
    return true;
  }
  // Set camera time
  printf("--- Set Camera Time ---\n");
  soap_result =
      soap_wsse_add_UsernameTokenDigestOffset(proxy_device_.soap, NULL, user.c_str(), password.c_str(), time_offset);
  if (SOAP_OK != soap_result)
  {
    printf("ERROR: %d - \nsoap_wsse_add_UsernameTokenDigestOffset: %s", soap_result, proxy_device_.soap_fault_detail());
    return false;
  }
  soap_result = soap_wsse_add_Timestamp(proxy_device_.soap, "Time", 10);
  if (SOAP_OK != soap_result)
  {
    printf("ERROR: %d - \nsoap_wsse_add_Timestamp: %s", soap_result, proxy_device_.soap_fault_detail());
    return false;
  }
  _tds__SetSystemDateAndTime set_request;
  _tds__SetSystemDateAndTimeResponse set_response;
  set_request.soap = proxy_device_.soap;
  set_request.DateTimeType = tt__SetDateTimeType__NTP;
  set_request.DaylightSavings = pc_local_tm->tm_isdst;
  tt__TimeZone tz;
  tz.TZ = cam_time_zone;
  set_request.TimeZone = &tz;
  tt__DateTime utc_datetime;
  tt__Date utc_date;
  tt__Time utc_time;
  utc_date.Year = pc_gmt_tm->tm_year;
  utc_date.Month = pc_gmt_tm->tm_mon;
  utc_date.Day = pc_gmt_tm->tm_mday;
  utc_time.Hour = pc_gmt_tm->tm_hour;
  utc_time.Minute = pc_gmt_tm->tm_min;
  utc_time.Second = pc_gmt_tm->tm_sec;
  utc_datetime.Time = &utc_time;
  utc_datetime.Date = &utc_date;
  set_request.UTCDateTime = &utc_datetime;
  soap_result = proxy_device_.SetSystemDateAndTime(&set_request, &set_response);
  if (soap_result == SOAP_OK)
  {
    LOG(INFO) << "System Date and Time set successfully";
    sleep(1);
  }
  else
  {
    LOG(INFO) << "ERROR:" << soap_result << "\nSetSystemDateAndTime : " << proxy_device_.soap_fault_detail();
    return false;
  }
  proxy_device_.destroy();
  return true;
}

std::string OnvifClientDevice::ErrorString()
{
  std::string result = "";
  result += std::to_string(proxy_device_.soap->error);
  result += " FaultString : ";
  if (*soap_faultstring(proxy_device_.soap))
  {
    std::string faultstring(*soap_faultstring(proxy_device_.soap));
    result += faultstring;
  }
  else
  {
    result += "null";
  }
  result += " FaultCode : ";
  if (*soap_faultcode(proxy_device_.soap))
  {
    std::string faultcode(*soap_faultcode(proxy_device_.soap));
    result += faultcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultSubcode : ";
  if (*soap_faultsubcode(proxy_device_.soap))
  {
    std::string faultsubcode(*soap_faultsubcode(proxy_device_.soap));
    result += faultsubcode;
  }
  else
  {
    result += "null";
  }
  result += " FaultDetail : ";
  if (*soap_faultdetail(proxy_device_.soap))
  {
    std::string faultdetail(*soap_faultdetail(proxy_device_.soap));
    result += faultdetail;
  }
  else
  {
    result += "null";
  }
  return result;
}
