#include <TFT_eSPI.h>
#include <AnimatedGIF.h>

#include "gif_file/dusel.h"
#define GifData dusel

TFT_eSPI tft = TFT_eSPI();

void setup()
{
  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  
  Serial.println("Testing display...");
  tft.fillScreen(TFT_RED);
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Display test complete");

  AnimatedGIF *gif;
  gif = openGif((uint8_t *)GifData, sizeof(GifData));
  if (gif == NULL)
  {
    Serial.println("Cannot open GIF");
    while (true) {}
  }

  Serial.println("Starting GIF animation...");
  while (true)
  {
    gif->playFrame(false, NULL);
  }
}

void loop()
{
  
  delay(1);
}

AnimatedGIF *openGif(uint8_t *gifdata, size_t gifsize)
{
  AnimatedGIF *gif;
  gif = (AnimatedGIF *)malloc(sizeof(AnimatedGIF));
  if (gif == NULL)
  {
    Serial.println("Not enough memory for GIF structure");
    return NULL;
  }

  gif->begin(GIF_PALETTE_RGB565_LE);  


  if (gif->open(gifdata, gifsize, GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif->getCanvasWidth(), gif->getCanvasHeight());
    Serial.printf("GIF memory size is %ld (%2.2f MB)\n", gifsize, (float)gifsize / (1024 * 1024));
    gif->setDrawType(GIF_DRAW_COOKED);
    if (gif->allocFrameBuf(GIFAlloc) != GIF_SUCCESS)
    {
      Serial.println("Not enough RAM memory for frame buffer");
      return NULL;
    }
    return gif;
  }
  else
  {
    printGifErrorMessage(gif->getLastError());
    return NULL;
  }
}


void *GIFAlloc(uint32_t u32Size)
{
  return malloc(u32Size);
}

void GIFFree(void *p)
{
  free(p);
}


void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0)
  {

    tft.setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }

  tft.pushColors((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}


void printGifErrorMessage(int errorCode)
{
  switch (errorCode)
  {
  case GIF_DECODE_ERROR:
    Serial.println("GIF Decoding Error");
    break;
  case GIF_TOO_WIDE:
    Serial.println("GIF Too Wide");
    break;
  case GIF_INVALID_PARAMETER:
    Serial.println("Invalid Parameter for gif open");
    break;
  case GIF_UNSUPPORTED_FEATURE:
    Serial.println("Unsupported feature in GIF");
    break;
  case GIF_FILE_NOT_OPEN:
    Serial.println("GIF File not open");
    break;
  case GIF_EARLY_EOF:
    Serial.println("GIF early end of file");
    break;
  case GIF_EMPTY_FRAME:
    Serial.println("GIF with empty frame");
    break;
  case GIF_BAD_FILE:
    Serial.println("GIF bad file");
    break;
  case GIF_ERROR_MEMORY:
    Serial.println("GIF memory Error");
    break;
  default:
    Serial.println("Unknown Error");
    break;
  }
}