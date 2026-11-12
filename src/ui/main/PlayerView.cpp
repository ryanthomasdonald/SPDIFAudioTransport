#include "PlayerView.h"
#include "DisplayUI.h"
#include "TextRenderer.h"
#include "../../../src/engine/AudioEngine.h"
#include "../menu/LibraryCommon.h"
#include <SD.h>

uint16_t activeArtworkCache[40000];
bool activeArtworkLoaded = false;

uint32_t lastUpdatedSecond = 999999;
uint16_t lastProgressPixelWidth = 0;

const int pBarX = 15;
const int pBarY = 231;
const int pBarMaxWidth = 450;
const int pBarHeight = 4;

UI_Button transport[] = {
  { 0, 250, 96, 70, "<<", COLOR_RAMS_CARD, false },
  { 96, 250, 96, 70, ">", COLOR_RAMS_CARD, false },
  { 192, 250, 96, 70, "[]", COLOR_RAMS_CARD, false },
  { 288, 250, 96, 70, ">>", COLOR_RAMS_CARD, false },
  { 384, 250, 96, 70, "MENU", COLOR_RAMS_CARD, false }
};

void resetProgressTrackers() {
  if (!isMediaPlaying && !isMediaPaused) {
    lastUpdatedSecond = 0;
    lastProgressPixelWidth = 0;
  } else {
    lastUpdatedSecond = 999999;
    lastProgressPixelWidth = 0;
  }

  if (currentUIState == STATE_PLAYER) {
    tft.fillRect(pBarX, pBarY, pBarMaxWidth, pBarHeight, COLOR_RAMS_DIVIDER);
  }
}

void updatePlayPauseButtonLabel(const char* newLabel, uint16_t newColor) {
  transport[1].label = newLabel;
  transport[1].color = newColor;
  if (currentUIState == STATE_PLAYER) {
    drawTransportButton(transport[1]);
  }
}

void drawAudioDashboard() {
  tft.fillScreen(COLOR_RAMS_BG);
  tft.fillRect(0, 250, 480, 70, COLOR_RAMS_CARD);
  tft.drawFastHLine(0, 250, 480, COLOR_RAMS_DIVIDER);
  for (int i = 0; i < 5; i++) {
    drawTransportButton(transport[i]);
  }
  drawAlbumArtwork();
}

void drawAlbumArtwork() {
  if (activeArtworkLoaded) {
    tft.writeRect(15, 15, 200, 200, activeArtworkCache);
  } else {
    tft.fillRect(15, 15, 200, 200, COLOR_RAMS_DIVIDER);
  }
}

void updateTrackWindow(int trackNum, const char* trackTitle) {
  if (currentUIState != STATE_PLAYER) return;
  resetProgressTrackers();

  String artistStr = String(currentArtistFolder);
  artistStr.replace("/", "");
  artistStr.toUpperCase();

  String albumStr = String(currentAlbumFolder);
  albumStr.replace("/", "");
  albumStr.toUpperCase();

  tft.fillRect(235, 15, 230, 213, COLOR_RAMS_BG);

  int trackingY = 18;
  int artistNextY = 0;
  drawWrappedTextLine(artistStr.c_str(), 235, trackingY, 230, 2, COLOR_RAMS_TEXT_MUTE, COLOR_RAMS_BG, 4, 2, artistNextY);

  trackingY = artistNextY + 8;
  int albumNextY = 0;
  drawWrappedTextLine(albumStr.c_str(), 235, trackingY, 230, 2, COLOR_RAMS_WHITE, COLOR_RAMS_BG, 4, 2, albumNextY);

  tft.drawFastHLine(235, 62, 230, COLOR_RAMS_ORANGE);

  String cleanName = String(trackTitle);
  if (cleanName.length() > 3) cleanName = cleanName.substring(3);
  if (cleanName.endsWith(".wav") || cleanName.endsWith(".WAV")) { cleanName = cleanName.substring(0, cleanName.length() - 4); }
  cleanName.toUpperCase();

  trackingY = 74;
  int titleNextY = 0;

  tft.fillRect(235, trackingY, 230, 130, COLOR_RAMS_BG);
  drawWrappedTextLine(cleanName.c_str(), 235, trackingY, 230, 3, COLOR_RAMS_WHITE, COLOR_RAMS_BG, 6, 6, titleNextY);

  char countBuf[32];
  sprintf(countBuf, "TRACK %02d OF %02d", trackNum, totalTracks);
  int dummyCountY = 0;
  drawWrappedTextLine(countBuf, 235, 216, 120, 1, COLOR_RAMS_TEXT_MUTE, COLOR_RAMS_BG, 0, 1, dummyCountY);

  if (!isMediaPlaying && !isMediaPaused) {
    int dummyTimeY = 0;
    tft.fillRect(394, 216, 71, 10, COLOR_RAMS_BG);
    drawWrappedTextLine("00:00 / 00:00", 394, 216, 71, 1, COLOR_RAMS_TEXT_MUTE, COLOR_RAMS_BG, 0, 1, dummyTimeY);
  }
}

void handleLiveTimeAndProgressBar() {
  if (!isMediaPlaying) return;

  extern volatile uint32_t absoluteTrackBytesPlayed;
  extern int activePlaybackTrackIndex;
  extern int totalTracks;
  extern char trackQueue[30][96];
  extern char currentAlbumAbsolutePath[256];

  uint32_t totalElapsedSeconds = absoluteTrackBytesPlayed / 176400;
  uint32_t elapsedMins = totalElapsedSeconds / 60;
  uint32_t elapsedSecs = totalElapsedSeconds % 60;

  if (elapsedSecs != lastUpdatedSecond) {
    lastUpdatedSecond = elapsedSecs;

    uint32_t totalTrackBytes = 0;
    if (selectedArtistIndex >= 0 && selectedAlbumIndex >= 0 && activePlaybackTrackIndex < totalTracks) {
      char fullTrackPath[384];
      snprintf(fullTrackPath, sizeof(fullTrackPath), "%s/%s",
               currentAlbumAbsolutePath, trackQueue[activePlaybackTrackIndex]);

      File sizeCheckFile = SD.open(fullTrackPath);
      if (sizeCheckFile) {
        totalTrackBytes = sizeCheckFile.size() - 44;
        sizeCheckFile.close();
      }
    }

    if (totalTrackBytes == 0) totalTrackBytes = 176400 * 180;

    uint32_t totalTrackSeconds = totalTrackBytes / 176400;
    uint32_t totalMins = totalTrackSeconds / 60;
    uint32_t totalSecs = totalTrackSeconds % 60;

    char timeBuffer[32];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02u:%02u / %02u:%02u",
             (unsigned int)elapsedMins, (unsigned int)elapsedSecs,
             (unsigned int)totalMins, (unsigned int)totalSecs);

    int dummyTimeY = 0;
    tft.fillRect(394, 216, 71, 10, COLOR_RAMS_BG);
    drawWrappedTextLine(timeBuffer, 394, 216, 71, 1, COLOR_RAMS_TEXT_MUTE, COLOR_RAMS_BG, 0, 1, dummyTimeY);

    if (totalTrackBytes > 0) {
      uint16_t currentProgressPixelWidth = ((uint64_t)absoluteTrackBytesPlayed * pBarMaxWidth) / totalTrackBytes;

      if (currentProgressPixelWidth > pBarMaxWidth) currentProgressPixelWidth = pBarMaxWidth;

      if (currentProgressPixelWidth != lastProgressPixelWidth) {
        lastProgressPixelWidth = currentProgressPixelWidth;

        tft.fillRect(pBarX, pBarY, currentProgressPixelWidth, pBarHeight, COLOR_RAMS_ORANGE);
        tft.fillRect(pBarX + currentProgressPixelWidth, pBarY, pBarMaxWidth - currentProgressPixelWidth, pBarHeight, COLOR_RAMS_DIVIDER);
      }
    }
  }
}

void drawTransportButton(UI_Button btn) {
  uint16_t bg = COLOR_RAMS_CARD;
  uint16_t fg = COLOR_RAMS_WHITE;
  if (btn.isPressed) {
    bg = COLOR_RAMS_DIVIDER;
    fg = COLOR_RAMS_ORANGE;
  } else if (strcmp(btn.label, "||") == 0 || strcmp(btn.label, ">") == 0) {
    fg = COLOR_RAMS_ORANGE;
  }
  tft.fillRect(btn.x, btn.y, btn.w, btn.h, bg);
  tft.drawRect(btn.x, btn.y, btn.w, btn.h, COLOR_RAMS_DIVIDER);
  int cx = btn.x + (btn.w / 2);
  int cy = btn.y + (btn.h / 2);
  if (strcmp(btn.label, ">") == 0) {
    tft.fillTriangle(cx - 8, cy - 12, cx - 8, cy + 12, cx + 12, cy, fg);
  } else if (strcmp(btn.label, "||") == 0) {
    tft.fillRect(cx - 8, cy - 12, 5, 24, fg);
    tft.fillRect(cx + 3, cy - 12, 5, 24, fg);
  } else if (strcmp(btn.label, "[]") == 0) {
    tft.fillRect(cx - 10, cy - 10, 20, 20, fg);
  } else if (strcmp(btn.label, "<<") == 0) {
    tft.fillTriangle(cx, cy - 10, cx, cy + 10, cx - 10, cy, fg);
    tft.fillTriangle(cx + 10, cy - 10, cx + 10, cy + 10, cx, cy, fg);
  } else if (strcmp(btn.label, ">>") == 0) {
    tft.fillTriangle(cx, cy - 10, cx, cy + 10, cx + 10, cy, fg);
    tft.fillTriangle(cx - 10, cy - 10, cx - 10, cy + 10, cx, cy, fg);
  } else {
    int stringWidth = 48;
    int stringHeight = 10;
    int textX = btn.x + ((btn.w - stringWidth) / 2);
    int textY = btn.y + ((btn.h - stringHeight) / 2);
    int dummyNextY = 0;
    drawWrappedTextLine(btn.label, textX, textY, stringWidth + 2, 2, fg, bg, 0, 1, dummyNextY);
  }
}

void processTouchControls() {
  if (currentUIState == STATE_MENU) {
    processMenuTouch();
    return;
  }

  static bool lastTouchState = false;
  bool currentTouch = readTouchPanel(touchX, touchY);

  if (currentTouch && !lastTouchState) {
    for (int i = 0; i < 5; i++) {
      if (touchX >= transport[i].x && touchX <= (transport[i].x + transport[i].w) && touchY >= transport[i].y && touchY <= (transport[i].y + transport[i].h)) {

        transport[i].isPressed = true;
        drawTransportButton(transport[i]);

        extern EngineLifecycleState activeEngineState;
        extern bool nextTrackPreLaunched;
        extern int activePlaybackTrackIndex;

        if (i == 0) {  // << PREVIOUS TRACK
          if (activePlaybackTrackIndex > 0) {
            isMediaPlaying = false;

            currentTrackIndex--;
            activePlaybackTrackIndex = currentTrackIndex;

            nextTrackPreLaunched = false;
            activeEngineState = ENGINE_IDLE;

            syncGlobalTextFolderPointers();
            playFreshAlbumStart();
            updatePlayPauseButtonLabel("||", COLOR_RAMS_CARD);
          }
        } else if (i == 1) {  // > TOGGLE PLAY / PAUSE
          if (!isMediaPlaying && !isMediaPaused) {
            nextTrackPreLaunched = false;
            syncGlobalTextFolderPointers();

            playFreshAlbumStart();
            updatePlayPauseButtonLabel("||", COLOR_RAMS_CARD);
          } else {
            if (isMediaPlaying) {
              isMediaPlaying = false;
              isMediaPaused = true;
              updatePlayPauseButtonLabel(">", COLOR_RAMS_CARD);
            } else {
              isMediaPlaying = true;
              isMediaPaused = false;
              updatePlayPauseButtonLabel("||", COLOR_RAMS_CARD);
            }
          }
        } else if (i == 2) {  // [] STOP BUTTON
          if (isMediaPlaying || isMediaPaused) {
            isMediaPlaying = false;
            isMediaPaused = false;
            nextTrackPreLaunched = false;
            activeEngineState = ENGINE_IDLE;

            updatePlayPauseButtonLabel(">", COLOR_RAMS_CARD);
            syncGlobalTextFolderPointers();

            extern int lastTrackIndexTracked;
            lastTrackIndexTracked = activePlaybackTrackIndex;

            lastUpdatedSecond = 0;
            lastProgressPixelWidth = 0;

            extern volatile bool uiTrackWindowNeedsRefresh;
            uiTrackWindowNeedsRefresh = true;
          }
        } else if (i == 3) {  // >> NEXT TRACK
          if (activePlaybackTrackIndex < (totalTracks - 1)) {
            isMediaPlaying = false;

            currentTrackIndex++;
            activePlaybackTrackIndex = currentTrackIndex;

            nextTrackPreLaunched = false;
            activeEngineState = ENGINE_IDLE;
            syncGlobalTextFolderPointers();
            playFreshAlbumStart();
            updatePlayPauseButtonLabel("||", COLOR_RAMS_CARD);
          }
        } else if (i == 4) {  // MENU BUTTON
          menuScrollOffset = 0;
          currentUIState = STATE_MENU;
          currentMenuLevel = LEVEL_ARTISTS;
          drawMenuScreen();
        }
      }
    }
  }
  if (!currentTouch && lastTouchState) {
    for (int i = 0; i < 5; i++) {
      if (transport[i].isPressed) {
        transport[i].isPressed = false;
        drawTransportButton(transport[i]);
      }
    }
  }
  lastTouchState = currentTouch;
}
