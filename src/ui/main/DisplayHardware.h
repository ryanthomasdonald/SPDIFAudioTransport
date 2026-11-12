#ifndef DISPLAY_HARDWARE_H
#define DISPLAY_HARDWARE_H

#include <Arduino.h>
#include <ST7796_t3.h>

// Structural button footprint for the transport bar layout
struct UI_Button {
  int x;
  int y;
  int w;
  int h;
  const char* label;
  uint16_t color;
  bool isPressed;
};

enum UIState {
  STATE_PLAYER,
  STATE_MENU
};

enum MenuLevel {
  LEVEL_ARTISTS,
  LEVEL_ALBUMS,
  LEVEL_TRACKS
};

enum EngineLifecycleState {
  ENGINE_IDLE,
  ENGINE_PRELOADING,
  ENGINE_STAGED,
  ENGINE_WAKING_UP,
  ENGINE_ACTIVE_PLAYING
};

// Color Tokens
#define COLOR_RAMS_BG 0x0841
#define COLOR_RAMS_CARD 0x18C3
#define COLOR_RAMS_DIVIDER 0x3186
#define COLOR_RAMS_WHITE 0xFFFF
#define COLOR_RAMS_ORANGE 0xFD00
#define COLOR_RAMS_TEXT_MUTE 0x7BEF

// Hardware graphics & touch core mappings
extern ST7796_t3 tft;
extern uint16_t touchX;
extern uint16_t touchY;

// Hardware Driver Function Signatures
void initDisplaySystem();
void refreshDisplayHardwareState();
bool readTouchPanel(uint16_t& x, uint16_t& y);

#endif
