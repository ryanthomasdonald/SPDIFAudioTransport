#include "DisplayUI.h"
#include "../../../src/engine/AudioEngine.h"
#include "../menu/LibraryCommon.h"

UIState currentUIState = STATE_MENU;
MenuLevel currentMenuLevel = LEVEL_ARTISTS;
int menuScrollOffset = 0;

// Isolated browse register shields
int browseArtistIndex = 0;
int browseAlbumIndex = 0;

extern int libraryArtistCount;
#define artistCount libraryArtistCount

void drawBootLoadingScreen() {
  tft.fillScreen(COLOR_RAMS_BG);
  int startX = 180;
  int startY = 150;
  int dummyNextY = 0;
  drawWrappedTextLine("LOADING...", startX, startY, 200, 2, COLOR_RAMS_WHITE, COLOR_RAMS_BG, 0, 1, dummyNextY);
}

void syncGlobalTextFolderPointers() {
  if (selectedArtistIndex >= 0 && selectedAlbumIndex >= 0) {
    if (library[selectedArtistIndex].name != NULL) {
      snprintf(currentArtistFolder, PATH_BUFFER_SIZE, "%s", library[selectedArtistIndex].name);
    }
    if (library[selectedArtistIndex].albums[selectedAlbumIndex].name != NULL) {
      snprintf(currentAlbumFolder, PATH_BUFFER_SIZE, "%s", library[selectedArtistIndex].albums[selectedAlbumIndex].name);
    }
  }
}

void drawMenuScreen() {
  if (currentMenuLevel == LEVEL_ARTISTS) {
    drawArtistView();
  } else if (currentMenuLevel == LEVEL_ALBUMS) {
    drawAlbumView();
  } else if (currentMenuLevel == LEVEL_TRACKS) {
    drawTrackView();
  }
}

void processMenuTouch() {
  if (currentMenuLevel == LEVEL_ARTISTS) {
    processArtistViewTouch();
    return;
  } else if (currentMenuLevel == LEVEL_ALBUMS) {
    processAlbumViewTouch();
    return;
  } else if (currentMenuLevel == LEVEL_TRACKS) {
    processTrackViewTouch();
    return;
  }
}
