#include "DisplayHardware.h"
#include <Wire.h>

#define TFT_DC 8
#define TFT_CS 10
#define TFT_RST 9
#define FT6336U_ADDR 0x38

ST7796_t3 tft = ST7796_t3(TFT_CS, TFT_DC, TFT_RST);

uint16_t touchX = 0;
uint16_t touchY = 0;

void initDisplaySystem() {
  Wire.begin();
  Wire.setClock(400000);
#if defined(TEENSYDUINO)
  Wire.setTimeout(3000);
#endif
  tft.init(320, 480);
  Wire.beginTransmission(FT6336U_ADDR);
  Wire.write(0x00);
  Wire.endTransmission(true);
  tft.invertDisplay(true);
  tft.setRotation(1);
}

void refreshDisplayHardwareState() {
  tft.setRotation(1);
  tft.invertDisplay(true);
}

bool readTouchPanel(uint16_t& x, uint16_t& y) {
  Wire.beginTransmission(FT6336U_ADDR);
  Wire.write(0x02);
  if (Wire.endTransmission(true) != 0) return false;
  if (Wire.requestFrom(FT6336U_ADDR, 5) != 5) return false;
  uint8_t td_status = Wire.read();
  uint8_t p1_xh = Wire.read();
  uint8_t p1_xl = Wire.read();
  uint8_t p1_yh = Wire.read();
  uint8_t p1_yl = Wire.read();
  if ((td_status & 0x0F) == 0) return false;
  uint16_t rawX = ((uint16_t)(p1_xh & 0x0F) << 8) | p1_xl;
  uint16_t rawY = ((uint16_t)(p1_yh & 0x0F) << 8) | p1_yl;
  x = rawY;
  y = 320 - rawX;
  return true;
}
