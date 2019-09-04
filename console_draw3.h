#ifndef CONSOLE_DRAW3_
#define CONSOLE_DRAW3_
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 180
#ifdef check
#define swap 0x4000
#else
#define swap 0x0000
#endif // check
#define mod 0x000F
extern void drawChar(int ch, int x, int y, int bgcolor, int fgcolor);
extern void drawString(char *p, int x, int y, int bg_color, int fg_color);
extern void drawCmdWindow();
extern void clearScreen();
extern void saveScreen();
extern void restoreScreen();
#endif
/*
drawChar()  最後兩個參數分別是字元背景和前景的顏色
可以參考下面的顏色值對照表來設定你想要的顏色

   0: 黑     1: 暗藍   2: 暗綠   3: 暗青
   4: 暗紅   5: 暗紫   6: 暗黃   7: 淺灰
   8: 暗灰   9: 亮藍  10: 亮綠  11: 亮青
  12: 亮紅  13: 亮紫  14: 亮黃  15: 白
*/
