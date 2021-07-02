#include <Arduino.h>
#include <HTTPUpdate.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#define ST(A) #A
#define STR(A) ST(A)
#define FIRMWARE_VERSION STR(VERSION)

WiFiMulti WiFiMulti;

const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
    "-----END CERTIFICATE-----\n";

void connectToWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  WiFiMulti.run();
}

void updateFirmware()
{
  if (WiFiMulti.run() == WL_CONNECTED)
  {
    log_v("Trying update firmware");

    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    client.setTimeout(12000 / 1000);

    t_httpUpdate_return ret = httpUpdate.updateSpiffs(client, SPIFSS_URL, FIRMWARE_VERSION);

    if (ret == HTTP_UPDATE_OK)
    {

      t_httpUpdate_return ret = httpUpdate.update(client, FIRMWARE_URL, FIRMWARE_VERSION);

      switch (ret)
      {
      case HTTP_UPDATE_FAILED:
        log_v("Update firmware failed.");
        break;

      case HTTP_UPDATE_NO_UPDATES:
        log_v("No update available");
        break;

      case HTTP_UPDATE_OK:
        log_v("Updated successfully");
        break;
      }
    }
    else if (ret == HTTP_UPDATE_NO_UPDATES)
    {
      log_v("No update available");
    }
    else
    {
      log_v("Update spiffs failed.");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  log_v("Firmware Version: %s", FIRMWARE_VERSION);

  connectToWifi();

  updateFirmware();
}

long last_update;

void loop()
{
  long now = millis();

  if (now >= last_update + 30000)
  {
    last_update += now;
    updateFirmware();
  }
}