/**
 * @file storage.cpp
 * @author Jordi Gauchía (jgauchia@gmx.es)
 * @brief  Storage definition and functions
 * @version 0.1.8
 * @date 2024-06
 */

#include "storage.hpp"

bool isSdLoaded = false;
extern const int SD_CS;
extern const int SD_MISO;
extern const int SD_MOSI;
extern const int SD_CLK;

/**
 * @brief SD Card init
 *
 */
void initSD()
{
  bool SDInitOk = false;
  pinMode(SD_CS,OUTPUT);
  digitalWrite(SD_CS,LOW);

  #ifdef SPI_SHARED
  SD.end();
  SDInitOk = SD.begin(SD_CS);
  #endif
  #ifndef SPI_SHARED
  spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  SDInitOk = SD.begin(SD_CS, spiSD, sdFreq);
  #endif
  
  if (!SDInitOk)
  {
    log_e("SD Card Mount Failed");
    return;
  }
  else
  {
    log_v("SD Card Mounted");
    isSdLoaded = true;
  }
 }

/**
 * @brief SPIFFS Init
 *
 */
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    log_e("SPIFFS Mount Failed");
    return;
  }
  else
    log_v("SPIFFS Mounted");
}
