#include "wsseapi_extra.hpp"

/**
@fn int soap_wsse_add_UsernameTokenDigestOffset(struct soap *soap, const char *id, const char *username, const char
*password, long offset)
@brief Adds UsernameToken element for digest authentication.
@param soap context
@param[in] id string for signature referencing or NULL
@param[in] username string
@param[in] password string
@param[in] time offset from current time in seconds
@return SOAP_OK

Computes SHA1 digest of the time stamp with offset, a nonce, and the password. The digest
provides the authentication credentials. Passwords are NOT sent in the clear.
Note: this release supports the use of at most one UsernameToken in the header.
*/
int soap_wsse_add_UsernameTokenDigestOffset(struct soap* soap, const char* id, const char* username,
                                            const char* password, long offset)
{
  _wsse__Security* security = soap_wsse_add_Security(soap);
  time_t now = time(NULL) + offset;
  const char* created = soap_dateTime2s(soap, now);
  char HA[SOAP_SMD_SHA1_SIZE], HABase64[29];
  char nonce[SOAP_WSSE_NONCELEN], *nonceBase64;
  DBGFUN2("soap_wsse_add_UsernameTokenDigest", "id=%s", id ? id : "", "username=%s", username ? username : "");
  /* generate a nonce */
  calc_nonce_offset(soap, nonce, offset);
  nonceBase64 = soap_s2base64(soap, (unsigned char*)nonce, NULL, SOAP_WSSE_NONCELEN);
  /* The specs are not clear: compute digest over binary nonce or base64 nonce? */
  /* compute SHA1(created, nonce, password) */
  calc_digest(soap, created, nonce, SOAP_WSSE_NONCELEN, password, HA);
  soap_s2base64(soap, (unsigned char*)HA, HABase64, SOAP_SMD_SHA1_SIZE);
  /* populate the UsernameToken with digest */
  soap_wsse_add_UsernameTokenText(soap, id, username, HABase64);
  /* populate the remainder of the password, nonce, and created */
  security->UsernameToken->Password->Type = (char*)wsse_PasswordDigestURI;
  security->UsernameToken->Nonce = nonceBase64;
  security->UsernameToken->wsu__Created = soap_strdup(soap, created);
  return SOAP_OK;
}
/******************************************************************************\
 *
 * Calculate SHA1(created, nonce, password) digest
 *
\******************************************************************************/

/**
@fn static void calc_digest(struct soap *soap, const char *created, const char *nonce, int noncelen, const char
*password, char hash[SOAP_SMD_SHA1_SIZE])
@brief Calculates digest value SHA1(created, nonce, password)
@param soap context
@param[in] created string (XSD dateTime format)
@param[in] nonce value
@param[in] noncelen length of nonce value
@param[in] password string
@param[out] hash SHA1 digest
*/
static void calc_digest(struct soap* soap, const char* created, const char* nonce, int noncelen, const char* password,
                        char hash[SOAP_SMD_SHA1_SIZE])
{
  struct soap_smd_data context;
  /* use smdevp engine */
  soap_smd_init(soap, &context, SOAP_SMD_DGST_SHA1, NULL, 0);
  soap_smd_update(soap, &context, nonce, noncelen);
  soap_smd_update(soap, &context, created, strlen(created));
  soap_smd_update(soap, &context, password, strlen(password));
  soap_smd_final(soap, &context, hash, NULL);
}
/******************************************************************************\
 *
 * Calculate randomized hex nonce with time offset
 *
\******************************************************************************/

/**
@fn static void calc_nonce(struct soap *soap, char nonce[SOAP_WSSE_NONCELEN], long offset)
@brief Calculates randomized nonce (also uses time() + offset in case a poorly seeded PRNG is used)
@param soap context
@param offset offset from current time in seconds
@param[out] nonce value
*/
static void calc_nonce_offset(struct soap* soap, char nonce[SOAP_WSSE_NONCELEN], long offset)
{
  int i;
  time_t r = time(NULL) + offset;
  memcpy(nonce, &r, 4);
  for (i = 4; i < SOAP_WSSE_NONCELEN; i += 4)
  {
    r = soap_random;
    memcpy(nonce + i, &r, 4);
  }
}
