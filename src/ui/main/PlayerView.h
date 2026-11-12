#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include <Arduino.h>
#include "DisplayHardware.h"

// 200x200 16-bit Artwork Cache Matrix Blocks
extern uint16_t activeArtworkCache[40000];
extern bool activeArtworkLoaded;

// Player Dashboard & Transport Function Signatures
void drawAudioDashboard();
void drawAlbumArtwork();
void resetProgressTrackers();
void updateTrackWindow(int trackNum, const char* trackTitle);
void handleLiveTimeAndProgressBar();
void updatePlayPauseButtonLabel(const char* newLabel, uint16_t newColor);
void drawTransportButton(UI_Button btn);
void processTouchControls();

#endif
