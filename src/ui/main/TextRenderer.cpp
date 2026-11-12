#include "TextRenderer.h"
#include "DisplayHardware.h"
#include "../util/BespokeFont.h"

int getBespokeCharWidth(char c) {
  if (c == ' ') return 3;
  if (c == '.' || c == '!' || c == '\'' || c == ':' || c == ';') return 1;
  if (c == '(' || c == ')') return 2;
  if (c == '<' || c == '>') return 3;
  return 5;
}

void drawBespokeChar(char c, int x, int y, int scale, uint16_t fgColor, uint16_t bgColor, int& outRenderedWidth) {
  if (c >= 'a' && c <= 'z') c -= 32;
  int contentWidth = getBespokeCharWidth(c);
  int totalWidthWithPad = (contentWidth + 1) * scale;
  if (c < 32 || c > 90) {
    tft.fillRect(x, y, totalWidthWithPad, 5 * scale, bgColor);
    outRenderedWidth = totalWidthWithPad;
    return;
  }
  int charIndex = c - 32;
  int bitmapOffset = charIndex * 5;
  for (int row = 0; row < 5; row++) {
    uint8_t rowBits = pgm_read_byte(&Bespoke5x5Bitmaps[bitmapOffset + row]);
    for (int col = 0; col < contentWidth; col++) {
      bool bitIsActive = (rowBits & (0x80 >> col));
      uint16_t activePixelColor = bitIsActive ? fgColor : bgColor;
      tft.fillRect(x + (col * scale), y + (row * scale), scale, scale, activePixelColor);
    }
  }
  tft.fillRect(x + (contentWidth * scale), y, scale, 5 * scale, bgColor);
  outRenderedWidth = totalWidthWithPad;
}

void drawWrappedTextLine(const char* text, int startX, int startY, int maxW, int fontScale, uint16_t color, uint16_t bgColor, int lineSpacing, int maxLines, int& outNextY) {
  String source = String(text);
  int currentX = startX;
  int currentY = startY;
  int lineCount = 1;
  int charH = 5 * fontScale;
  int spaceWidth = getBespokeCharWidth(' ') * fontScale;
  int dotWidth = (getBespokeCharWidth('.') + 1) * fontScale;
  int wordStart = 0;
  bool firstWordOnLine = true;
  while (wordStart < (int)source.length()) {
    if (maxLines > 0 && lineCount >= maxLines && currentX + (dotWidth * 3) > startX + maxW) {
      for (int e = 0; e < 3; e++) {
        int pad = 0;
        drawBespokeChar('.', currentX, currentY, fontScale, color, bgColor, pad);
        currentX += pad;
      }
      break;
    }
    int wordEnd = source.indexOf(' ', wordStart);
    if (wordEnd == -1) wordEnd = source.length();
    String word = source.substring(wordStart, wordEnd);
    int wordW = 0;
    for (int i = 0; i < (int)word.length(); i++) { wordW += (getBespokeCharWidth(word[i]) + 1) * fontScale; }
    if (word.length() > 0) wordW -= fontScale;
    int requiredWidth = wordW + (firstWordOnLine ? 0 : spaceWidth + fontScale);
    if (currentX + requiredWidth > startX + maxW) {
      if (!firstWordOnLine || wordW <= maxW) {
        if (maxLines > 0 && lineCount >= maxLines) {
          for (int e = 0; e < 3; e++) {
            int pad = 0;
            drawBespokeChar('.', currentX, currentY, fontScale, color, bgColor, pad);
            currentX += pad;
          }
          break;
        }
        currentX = startX;
        currentY += charH + lineSpacing;
        lineCount++;
        firstWordOnLine = true;
        requiredWidth = wordW;
      } else {
        for (int c = 0; c < (int)word.length(); c++) {
          int charPadW = 0;
          int nextCharW = (getBespokeCharWidth(word[c]) + 1) * fontScale;
          if (maxLines > 0 && lineCount >= maxLines && currentX + nextCharW > startX + maxW) {
            drawBespokeChar('.', currentX, currentY, fontScale, color, bgColor, charPadW);
            wordStart = source.length();
            break;
          }
          if (currentX + nextCharW > startX + maxW) {
            if (maxLines > 0 && lineCount >= maxLines) {
              drawBespokeChar('.', currentX, currentY, fontScale, color, bgColor, charPadW);
              wordStart = source.length();
              break;
            }
            currentX = startX;
            currentY += charH + lineSpacing;
            lineCount++;
          }
          drawBespokeChar(word[c], currentX, currentY, fontScale, color, bgColor, charPadW);
          currentX += charPadW;
        }
        wordStart = wordEnd + 1;
        firstWordOnLine = false;
        continue;
      }
    }
    if (!firstWordOnLine) {
      int spacePadW = 0;
      drawBespokeChar(' ', currentX, currentY, fontScale, color, bgColor, spacePadW);
      currentX += spacePadW;
    }
    for (int i = 0; i < (int)word.length(); i++) {
      int charPadW = 0;
      drawBespokeChar(word[i], currentX, currentY, fontScale, color, bgColor, charPadW);
      currentX += charPadW;
    }
    firstWordOnLine = false;
    wordStart = wordEnd + 1;
  }
  outNextY = currentY + charH;
}
