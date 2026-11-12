#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Arduino.h>
#include "DisplayHardware.h"
#include "TextRenderer.h"
#include "PlayerView.h"

// Global state machine tracking flags
extern UIState currentUIState;
extern MenuLevel currentMenuLevel;
extern int menuScrollOffset;

// Isolated browse register shields
extern int browseArtistIndex;
extern int browseAlbumIndex;

// Active playback memory trackers
extern int selectedArtistIndex;
extern int selectedAlbumIndex;

// Top-Level UI & Menu Routing Signatures
void drawBootLoadingScreen();
void syncGlobalTextFolderPointers();

void drawMenuScreen();
void drawArtistView();
void drawAlbumView();
void drawTrackView();
void processMenuTouch();
void processArtistViewTouch();
void processAlbumViewTouch();
void processTrackViewTouch();

#endif
