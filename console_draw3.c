#include <windows.h>
#include <string.h>
#include "console_draw3.h"



COORD dwBufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
COORD dwBufferCoord = { 0, 0 };
SMALL_RECT rcRegion = { 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1 };
CHAR_INFO screen_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
CHAR_INFO background_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
CHAR_INFO clean_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

int Double_Buffer_Switch;

HANDLE hOutput[2];

int Touched_Global = 0;
int Initialized_Clean = 0;


static void putASCII2( int x, int y, int ch, int fgcolor, int bgcolor )
{
    if ( Touched_Global == 0) {
        hOutput[0] = (HANDLE) GetStdHandle( STD_OUTPUT_HANDLE );
        SetConsoleScreenBufferSize(hOutput[0], dwBufferSize);

        hOutput[1] = CreateConsoleScreenBuffer(
                          GENERIC_READ |           // read/write access
                          GENERIC_WRITE,
                          FILE_SHARE_READ |
                          FILE_SHARE_WRITE,        // shared
                          NULL,                    // default security attributes
                          CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
                          NULL);                   // reserved; must be NULL
        SetConsoleScreenBufferSize(hOutput[1], dwBufferSize);

        SetConsoleActiveScreenBuffer(hOutput[0]);
        Double_Buffer_Switch = 1;
        Touched_Global = 1;
    }

    if (x<SCREEN_WIDTH && x>=0 && y<SCREEN_HEIGHT && y>=0) {
        screen_buffer[y][x].Char.AsciiChar = ch;
        screen_buffer[y][x].Attributes = (fgcolor & mod) | (bgcolor & mod)<<4 | swap;
    }
}

void drawChar(int ch, int x, int y, int bgcolor, int fgcolor){
    putASCII2( x, y, ch, fgcolor, bgcolor );
}


/* 在 (x, y) 座標的位置顯示字串 p 的內容  fg_color 是前景的顏色  bg_color 則是背景的顏色 */
static void putString(int x, int y, char *p, int fg_color, int bg_color)
{
    int i;
    for(i=0; (unsigned int) i<strlen(p); i++) {
        putASCII2(x+i, y, p[i], fg_color, bg_color);
    }
}


void drawString(char *p, int x, int y, int bg_color, int fg_color){
    putString(x, y, p, fg_color, bg_color);
}

void drawCmdWindow()
{
    if (Touched_Global) {
        WriteConsoleOutput( hOutput[Double_Buffer_Switch], (CHAR_INFO *)screen_buffer, dwBufferSize,
                            dwBufferCoord, &rcRegion );
        SetConsoleActiveScreenBuffer(hOutput[Double_Buffer_Switch]);
        SetConsoleCursorPosition( hOutput[Double_Buffer_Switch], dwBufferSize );
        Double_Buffer_Switch = !Double_Buffer_Switch;
    }
}


void clearScreen(void)
{
    int i, j;
    if (Initialized_Clean == 0) {
        for (i=0; i<SCREEN_HEIGHT; i++) {
            for (j=0; j<SCREEN_WIDTH; j++) {
                screen_buffer[i][j].Char.AsciiChar = ' ';
                screen_buffer[i][j].Attributes = 0;
            }
        }
        memcpy(clean_buffer, screen_buffer, sizeof(CHAR_INFO)*SCREEN_HEIGHT*SCREEN_WIDTH);
        Initialized_Clean = 1;
    } else {
        memcpy(screen_buffer, clean_buffer, sizeof(CHAR_INFO)*SCREEN_HEIGHT*SCREEN_WIDTH);
    }
}

void saveScreen(void)
{
    memcpy(background_buffer, screen_buffer, sizeof(CHAR_INFO)*SCREEN_HEIGHT*SCREEN_WIDTH);
}
void restoreScreen(void)
{
    memcpy(screen_buffer, background_buffer, sizeof(CHAR_INFO)*SCREEN_HEIGHT*SCREEN_WIDTH);
}
