#ifndef WSSEAPI_EXTRA_HPP
#define WSSEAPI_EXTRA_HPP

#include "gsoap/wsseapi.h"
#include "gsoap/smdevp.h"
#include "gsoap/mecevp.h"
#include "gsoap/threads.h"

#if defined(SOAP_WSA_2003) || defined(SOAP_WSA_2004) || defined(SOAP_WSA_200408) || defined(SOAP_WSA_2005)
#include "wsaapi.h"
#endif

// #ifdef __cplusplus
// extern "C" {
// #endif
/** Size of the random nonce */
#define SOAP_WSSE_NONCELEN (20)
static void calc_digest(struct soap* soap, const char* created, const char* nonce, int noncelen, const char* password,
                        char hash[SOAP_SMD_SHA1_SIZE]);
static void calc_nonce_offset(struct soap* soap, char nonce[SOAP_WSSE_NONCELEN], long offset);
int soap_wsse_add_UsernameTokenDigestOffset(struct soap* soap, const char* id, const char* username,
                                            const char* password, long offset);

#endif
