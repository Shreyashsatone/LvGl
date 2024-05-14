/**
 * @file satInfo.cpp
 * @author Jordi Gauchía (jgauchia@gmx.es)
 * @brief  Satellites info screen functions
 * @version 0.1.8
 * @date 2024-05
 */

#include "satInfo.hpp"

// GSV GPS_GSV; // GPS Satellites in view
// GSV GL_GSV;  // GLONASS Satellites in view
// GSV BD_GSV;  // BEIDOU Satellites in view

SatPos satPos; // Satellite position X,Y in constelation map

TFT_eSprite spriteSNR1 = TFT_eSprite(&tft);       // Sprite for snr GPS Satellite Labels
TFT_eSprite spriteSNR2 = TFT_eSprite(&tft);       // Sprite for snr GPS Satellite Labels
TFT_eSprite spriteSat = TFT_eSprite(&tft);        // Sprite for satellite position in map
TFT_eSprite constelSprite = TFT_eSprite(&tft);    // Double Buffering Sprites for Satellite Constellation
TFT_eSprite constelSpriteBkg = TFT_eSprite(&tft); // Double Buffering Sprites for Satellite Constellation

lv_obj_t *satelliteBar1;               // Satellite Signal Graphics Bars
lv_obj_t *satelliteBar2;               // Satellite Signal Graphics Bars
lv_chart_series_t *satelliteBarSerie1; // Satellite Signal Graphics Bars
lv_chart_series_t *satelliteBarSerie2; // Satellite Signal Graphics Bars

/**
 * @brief Get the Satellite position for constelation map
 *
 * @param elev -> elevation
 * @param azim -> Azimut
 * @return SatPos -> Satellite position
 */
SatPos getSatPos(uint8_t elev, uint16_t azim)
{
  SatPos pos;
  int H = (60 * cos(DEG2RAD(elev)));
  pos.x = 75  + (H * sin(DEG2RAD(azim)));
  pos.y = 75 - (H * cos(DEG2RAD(azim)));
  return pos;
}

/**
 * @brief Delete sat info screen sprites and release PSRAM
 *
 */
void deleteSatInfoSprites()
{
  spriteSNR1.deleteSprite();
  spriteSNR2.deleteSprite();
  spriteSat.deleteSprite();
  constelSprite.deleteSprite();
  constelSpriteBkg.deleteSprite();
}

/**
 * @brief Create Constellation sprite
 *
 * @param spr -> Sprite
 */
void createConstelSprite(TFT_eSprite &spr)
{
  spr.deleteSprite();
  spr.createSprite(150, 150);
  spr.fillScreen(TFT_BLACK);
  spr.drawCircle(75, 75, 60, TFT_WHITE);
  spr.drawCircle(75, 75, 30, TFT_WHITE);
  spr.drawCircle(75, 75, 1, TFT_WHITE);
  spr.setTextFont(2);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("N", 72, 7);
  spr.drawString("S", 72, 127);
  spr.drawString("W", 12, 67);
  spr.drawString("E", 132, 67);
  spr.setTextFont(1);
}

/**
 * @brief Create satellite sprite
 *
 * @param spr -> Sprite
 */
void createSatSprite(TFT_eSprite &spr)
{
  spr.deleteSprite();
  spr.createSprite(8, 8);
  spr.setColorDepth(8);
  spr.fillScreen(TFT_BLACK);
}

/**
 * @brief Create SNR text sprite
 *
 * @param spr -> Sprite
 */
void createSNRSprite(TFT_eSprite &spr)
{
  spr.deleteSprite();
  spr.createSprite(TFT_WIDTH, 10);
  spr.setColorDepth(8);
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
}

/**
 * @brief Draw SNR bar and satellite number
 *
 * @param bar -> Bar Control
 * @param barSer -> Bar Control Serie
 * @param id -> Active Sat
 * @param satNum -> Sat ID
 * @param snr -> Sat SNR
 * @param spr -> Sat number sprite
 */
void drawSNRBar(lv_obj_t *bar, lv_chart_series_t *barSer, uint8_t id, uint8_t satNum, uint8_t snr, TFT_eSprite &spr)
{
  lv_point_t p;
  barSer->y_points[id] = snr;
  lv_chart_get_point_pos_by_id(bar, barSer, id, &p);
  spr.setCursor(p.x - 2, 0);
  spr.print(satNum);
}

/**
 * @brief Clear Satellite in View found
 *
 */
void clearSatInView()
{
  for (int clear = 0; clear < MAX_SATELLITES; clear++)
  {
    satTracker[clear].satNum = 0;
    satTracker[clear].elev = 0;
    satTracker[clear].azim = 0;
    satTracker[clear].snr = 0;
    satTracker[clear].active = false;
  }
}

/**
 * @brief Display satellite in view info
 *
 * @param gsv -> GSV NMEA sentence
 * @param color -> Satellite color in constellation
 */
void fillSatInView(GSV &gsv, int color)
{
  if (gsv.totalMsg.isUpdated())
  {
    lv_chart_refresh(satelliteBar1);
    lv_chart_refresh(satelliteBar2);

    for (int i = 0; i < 4; ++i)
    {
      int no = atoi(gsv.satNum[i].value());
      if (no >= 1 && no <= MAX_SATELLITES)
      {
        satTracker[no - 1].satNum = atoi(gsv.satNum[i].value());
        satTracker[no - 1].elev = atoi(gsv.elev[i].value());
        satTracker[no - 1].azim = atoi(gsv.azim[i].value());
        satTracker[no - 1].snr = atoi(gsv.snr[i].value());
        satTracker[no - 1].active = true;
      }
    }

    uint8_t totalMessages = atoi(gsv.totalMsg.value());
    uint8_t currentMessage = atoi(gsv.msgNum.value());

    if (totalMessages == currentMessage)
    {
      createSNRSprite(spriteSNR1);
      createSNRSprite(spriteSNR2);

      for (int i = 0; i < (MAX_SATELLLITES_IN_VIEW / 2); i++)
      {
        satelliteBarSerie1->y_points[i] = LV_CHART_POINT_NONE;
        satelliteBarSerie2->y_points[i] = LV_CHART_POINT_NONE;
      }

      uint8_t activeSat = 0;
      for (int i = 0; i < MAX_SATELLITES; ++i)
      {
        if (satTracker[i].active) // && satTracker[i].snr > 0)
        {
          if (activeSat < (MAX_SATELLLITES_IN_VIEW / 2))
            drawSNRBar(satelliteBar1, satelliteBarSerie1, activeSat, satTracker[i].satNum, satTracker[i].snr, spriteSNR1);
          else
            drawSNRBar(satelliteBar2, satelliteBarSerie2, (activeSat - (MAX_SATELLLITES_IN_VIEW / 2)), satTracker[i].satNum, satTracker[i].snr, spriteSNR2);

          activeSat++;

          satPos = getSatPos(satTracker[i].elev, satTracker[i].azim);

          spriteSat.fillCircle(4, 4, 2, color);
          spriteSat.pushSprite(&constelSprite, satPos.x, satPos.y, TFT_TRANSPARENT);
          constelSprite.setCursor(satPos.x, satPos.y + 8);
          constelSprite.print(i + 1);

          if (satTracker[i].posX != satPos.x || satTracker[i].posY != satPos.y)
          {
            constelSpriteBkg.pushSprite(150, 30);
          }

          satTracker[i].posX = satPos.x;
          satTracker[i].posY = satPos.y;
        }
      }
      constelSprite.pushSprite(150, 30);
    }

    lv_chart_refresh(satelliteBar1);
    spriteSNR1.pushSprite(0, 260);
    lv_chart_refresh(satelliteBar2);
    spriteSNR2.pushSprite(0, 345);
  }
}