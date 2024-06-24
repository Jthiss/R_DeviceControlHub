#include "test_tft.h"

bool tft_invert=false;

void Test_TFT_Init(TFT_eSPI tft)
{
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

    // Set "cursor" at top left corner of display (0,0) and select font 4
    tft.setCursor(0, 4, 4);

    // Set the font colour to be white with a black background
    tft.setTextColor(TFT_WHITE);

    // We can now plot text on screen using the "print" class
    tft.println(" Initialised default\n");
    tft.println(" White text");
    
    tft.setTextColor(TFT_RED);
    tft.println(" Red text");
    
    tft.setTextColor(TFT_GREEN);
    tft.println(" Green text");
    
    tft.setTextColor(TFT_BLUE);
    tft.println(" Blue text");
}

void Test_TFT_Loop(TFT_eSPI tft)
{
    tft.invertDisplay( tft_invert ); // Where i is true or false
 
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

    tft.setCursor(0, 4, 4);

    tft.setTextColor(TFT_WHITE);
    tft.println(" Invert\n");

    tft.println(" White text");
    
    tft.setTextColor(TFT_RED);
    tft.println(" Red text");
    
    tft.setTextColor(TFT_GREEN);
    tft.println(" Green text");
    
    tft.setTextColor(TFT_BLUE);
    tft.println(" Blue text");
    tft_invert=!tft_invert;
}
