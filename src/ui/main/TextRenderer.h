#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <Arduino.h>

// Typography & Text Layout Function Signatures
int getBespokeCharWidth(char c);
void drawBespokeChar(char c, int x, int y, int scale, uint16_t fgColor, uint16_t bgColor, int& outRenderedWidth);
void drawWrappedTextLine(const char* text, int startX, int startY, int maxW, int fontScale, uint16_t color, uint16_t bgColor, int lineSpacing, int maxLines, int& outNextY);

#endif
