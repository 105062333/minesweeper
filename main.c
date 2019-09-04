#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
/* 音效記得要加入這個 */
#include "audio.h"
/* 另外還有底下的設定 */
/* For sound effect: In [Project Options]->[Parameters]->[Linker] add the parameter -lwinmm */


#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "image.h"
#include "framework.h"

/*
drawChar()  最後兩個參數分別是字元背景和前景的顏色
可以參考下面的顏色值對照表來設定你想要的顏色
   0: 黑     1: 暗藍   2: 暗綠   3: 暗青
   4: 暗紅   5: 暗紫   6: 暗黃   7: 淺灰
   8: 暗灰   9: 亮藍  10: 亮綠  11: 亮青
  12: 亮紅  13: 亮紫  14: 亮黃  15: 白
*/

#define WIDTH     30
#define HEIGHT    20
#define OFFSET_X  5
#define OFFSET_Y  5

#define NUM_KEYS 7
#define REFRESH_RATE 10


int timer(void);

int my_game_one(void);
int my_game_two(void);
int ending(void);
int play_about(void);

void setMainmenu(Menu *m);

void assign_initial(int cells[][21]);                                           //將格子內資訊初始化為0
void print_blanks(void);                                                        //印出遊戲初始的格子
void open_cell(int *op_x, int *op_y);                                           //請使用者輸入要打開的位置
void put_mine(int cells[][21], int op_x, int op_y);                             //放置地雷
void calculate_num_of_cells(int cells[][21]);                                   //計算非地雷區附近有幾顆地雷
void assign_inf_after_open(int cells[][21], int op_x, int op_y);                //改變使用者打開的位置之資訊
void open_surrounding(int cells[][21], int p_x, int op_y);                      //打開附近的區域（當格子附近無地雷時）
void check_no_mine(int cells[][21], int op_x, int op_y);                        //檢查附近的區域有沒有地雷
int  check_over(int cells[][21], int op_x, int op_y);                           //檢查遊戲是否結束(輸或贏)
void print_cells(int cells[][21], int op_x, int op_y, int over);                //印出目前的狀況
void print_condition_playing(int cells[][21]);                                  //印出尚未結束的狀況
void print_condition_over(int cells[][21], int over);                           //印出遊戲結束的狀況(每個格子之資訊)
char play_again(void);
int about(void);
int cur_tick;                                                //詢問是否再玩一次


int main(void)
{
    int IsEnding = 0;
    int k;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN, VK_SPACE};
    int cur_tick, last_tick;
    Menu mainmenu;
    Image* background;
    background = read_image("256.pixel", "256.color");
    Audio audio;
    openAudioFile("metalplate.wav", &audio);

    /* 啟動鍵盤控制 整個程式中只要做一次就行了*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);

    show_image(background, 0, 0);
    saveScreen();

    /* 無窮迴圈  裡面包含了這個程式的主要工作
    而且每次迴圈會不斷把最新的畫面顯示到螢幕上
    像是播動畫一樣 每次迴圈更新一次畫面 */
    while (!IsEnding) {
        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */
        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        sprintf(str, "%10d", cur_tick/1000);

        if (cur_tick-last_tick > REFRESH_RATE) {

            last_tick = cur_tick;


           clearScreen();
           restoreScreen();

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y-1, str, 14);

            /* 把選單畫出來 */
            showMenu(&mainmenu);

            /* 為了要讓一連串 drawChar() 的動作產生效果
               必須要呼叫一次 drawCmdWindow() 把之前畫的全部內容一次顯示到螢幕上 */
            drawCmdWindow();

        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {
            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

        /* 鍵盤控制 處理按著不放的狀況 */
        for (k=0; k<NUM_KEYS; k++) {
            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    scrollMenu(&mainmenu, -1);
                    break;
                case VK_DOWN:
                    scrollMenu(&mainmenu, +1);
                    break;
                case VK_RETURN:
                    if (IsOnItem(&mainmenu, 0)) {
                        if (IsItemSelected(&mainmenu, 0)){
                            playAudio(&audio);
                            my_game_one();
                            ending();
                        }
                    } else if (IsOnItem(&mainmenu, 1)) {
                        IsEnding = 1;


                    } else if (IsOnItem(&mainmenu, 2)) {
                        if (IsItemSelected(&mainmenu, 2)){
                            about();
                        }
                    }
                    break;
                case VK_SPACE:
                    /* 可以試試看底下另一種選單形式
                        同時能有多個項目可以被選到
                        toggleMenu(&mainmenu);
                    */
                    radioMenu(&mainmenu);
                    break;
                }
            }
        }
    } /* while (IsEnding) */
    ending();
    return 0;
}
int timer(void)
{
    return (clock()/(0.001*CLOCKS_PER_SEC));
}
void setMainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 2;
    m->y = 2;
    m->fgcolor = 4;
    m->bgcolor = 10;
    m->num_options = 3;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* 目前選擇的項目 */

    strcpy(m->text[0], "START");
    strcpy(m->text[1], "EXIT");
    strcpy(m->text[2], "ABOUT");
    strcpy(m->alt_text[0], "START *");
    strcpy(m->alt_text[1], "EXIT *");
    strcpy(m->alt_text[2], "ABOUT *");
}

int play_about(void)
{

    int IsEnding = 0;

    int k;

    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};

    /* 啟動鍵盤控制 整個程式中只要做一次就行了*/
    initializeKeyInput();
/*

     ** 程式設計說明：
     1. 將格子宣告成int cells[20][20]的陣列，存放資訊之用
            0  代表安全的地區，且附近沒地雷
         1~ 19  代表安全的地區，但附近有地雷，數字為地雷數
           -1  代表尚未被踩到的地雷區
        20~39  代表已被選取的安全地區
           -2  代表已被踩到的地雷區，遊戲結束

     2. 在螢幕上印出格子
     3. 請使用者輸入第一個要打開的位置(x, y)
     2. 利用亂數放置地雷十顆，在不重複的位置且不是使用者輸入的位址
     4. 在安全地區計算其資訊(附近幾顆地雷)
     5. 將剛被選取到的安全地區值加10(代表已選取)
     6. 清除螢幕
     7. 顯示cells陣列中，值>10的資訊（>10代表已經選取）
     8. 請使用者輸入接下來要打開的位置(x2, y2)
     9. 計算(x2, y2)的值 (安全-> cells[x2][y2] += 10；地雷-> cells[x2][y2] = -2，遊戲結束)
    10. 如果cells[x2, y2]的值為10，則打開附近的格子
    10. 若未結束則重複step 6 ~ 9，直到贏(全部都>10 or ==-1 )就跳出
    11. 詢問是否再玩一次，若再玩一次則重複2~10
*/


    while (!IsEnding) {
        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */


        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_ESCAPE:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

    }
      /*while (IsEnding)*/
    return 0;
}
/*--------------------------------------------------------------*/
int my_game_one(void)
{
    int  cells[21][21],
         open_x, open_y,
         over;
    char again;

    int IsEnding = 0;

    int k;
    Audio uccu;
    openAudioFile("999.wav", &uccu);

    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};


    initializeKeyInput();


    do {
       playAudio(&uccu);
       system("cls");
       assign_initial(cells);                                                   //將格子內資訊初始化為0
       print_blanks();                                                          //印出遊戲初始的格子
       open_cell(&open_x, &open_y);                                             //請使用者輸入要打開的位置
       put_mine(cells, open_x, open_y);                                         //放置地雷
       calculate_num_of_cells(cells);                                           //計算非地雷區附近有幾彈地雷
       assign_inf_after_open(cells, open_x, open_y);                            //改變使用者打開的位置之資訊
       check_no_mine(cells, open_x, open_y);                                    //檢查附近的區域有沒有地雷，並將無地雷的區域打開
       over = check_over(cells, open_x, open_y);                                //檢查遊戲是否結束(輸或贏)
       print_cells(cells, open_x, open_y, over);                                //印出目前的狀況
       while (over == 0) {
             open_cell(&open_x, &open_y);
             assign_inf_after_open(cells, open_x, open_y);                      //請使用者輸入要打開的位置
             check_no_mine(cells, open_x, open_y);                              //檢查附近的區域有沒有地雷，並將無地雷的區域打開
             over = check_over(cells, open_x, open_y);                          //檢查遊戲是否結束(輸或贏)
             print_cells(cells, open_x, open_y, over);                          //印出目前的狀況
       }
        pauseAudio(&uccu);
        again = play_again();                                          //詢問是否再玩一次
    } while (again=='y'||again=='Y');




 while (!IsEnding) {
        //畫面更新

        //鍵盤控制
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }


    return(0);
}

void assign_initial(int cells[][21]) {                                          //將格子內資訊初始化為0
     int i, j;
     for (i = 0; i < 21; i++)
         for (j = 0; j < 21; j++)
             cells[i][j] = 0;
}

void print_blanks(void) {                                                       //印出遊戲初始的格子
     int i, j;
      printf("\n\n\n                        遊戲設計說明：\n");
     printf("             ---------------------------------------\n");
     printf("             1. 踩地雷有19*19的格子，共40顆地雷\n");
     printf("             2. 顯示數字為那一格附近的八個地區共有幾顆地雷)\n");
     printf("             3. 未顯示數字則表示附近的八個地區沒地雷\n");
     printf("             ---------------------------------------\n\n\n\n\n");

    printf("                                  ");
     for (i = 1; i <= 19; i++)
         printf("%2d", i);
     printf("\n");
     for (i = 1; i <= 19; i++) {
         for (j = 0; j <= 19; j++) {
             if (j == 0)
                printf("                              %4d", i);
             else
                 printf("口");
         }
         printf("\n");
     }
}

void open_cell(int *op_x, int *op_y) {                                          //請使用者輸入要打開的位置
        printf("\n\n                              ");
     printf("Please input which site you want >> ");
     scanf("%d%d", op_x, op_y);
}

void put_mine(int cells[][21], int op_x, int op_y) {                            //放置地雷
     srand(time(NULL));          //  亂數種子
     int  n = 0,
          rand_num,
          cell_x,
          cell_y;

     do {
         rand_num = rand() % 361;     // ( rand() % (最大值-最小值+1)) + 最小值
         cell_x = (rand_num % 19) + 1;
         cell_y = ((rand_num - cell_x) / 19) + 1;
         if (cells[cell_x][cell_y] != -1 && !(cell_x == op_x && cell_y == op_y)) {
             cells[cell_x][cell_y] = -1;
             n++;
         }
     } while (n < 40);
}

void calculate_num_of_cells(int cells[][21]) {                                  //計算非地雷區附近有幾彈地雷
     int i, j, u, v;
     for (i = 1; i <= 19; i++)
         for (j = 1; j <= 19; j++)
             if (cells[i][j] == 0)
                for (u = i - 1; u <= i + 1; u++)
                    for (v = j - 1; v <= j + 1; v++)
                        if (cells[u][v] == -1)
                           cells[i][j]++;
}

void assign_inf_after_open(int cells[][21], int op_x, int op_y) {               //改變使用者打開的位置之資訊
     if (cells[op_x][op_y] == -1)
        cells[op_x][op_y] = -2;
     if (cells[op_x][op_y] >= 0 && cells[op_x][op_y] <= 18)
        cells[op_x][op_y] += 20;
}

void open_surrounding(int cells[][21], int op_x, int op_y) {                    //打開附近的區域（當格子附近無地雷時）
     int i, j;
     for (i = op_x - 1; i <= op_x + 1; i++) {
         for (j = op_y - 1; j <= op_y + 1; j++) {
             if (cells[i][j] >= 0 && cells[i][j] <=19 && i % 20 != 0 && j %20 != 0) {
                cells[i][j] += 20;
                check_no_mine(cells, i, j);
             }
         }
     }
}

void check_no_mine(int cells[][21], int op_x, int op_y) {                       //檢查附近的區域有沒有地雷
     if (cells[op_x][op_y] == 20)        //附近沒地雷
        open_surrounding(cells, op_x, op_y);
}

void print_condition_playing(int cells[][21]) {                                 //印出尚未結束的狀況
     int i, j;
     printf("\n\n\n\n\n\n\n\n\n\n");
     printf("                                    ");
     for (i = 1; i <= 19; i++)
         printf("%2d", i);
     printf("\n                                  ┌－－－－－－－－－－－－－－－－－－－┐\n");
     for (i = 1; i <= 19; i++) {
             for (j = 0; j <= 19; j++) {
                 if (j == 0){
                    printf("                              ");
                    printf("%4d｜", i);
                 }
                 else if (cells[i][j] < 20)
                      printf("口");
                 else if (cells[i][j] == 20)
                      printf(" 0");
                 else if (cells[i][j] > 20 && cells[i][j] <= 39)
                      printf(" %d", cells[i][j] - 20);
                 if (j == 19)
                    printf("｜%d", i);
         }
         printf("\n");
     }
     printf("                                  └－－－－－－－－－－－－－－－－－－－┘\n");
     printf("                                    ");
     for (i = 1; i <= 19; i++){

         printf("%2d", i);
     }
     printf("\n");
}


void print_condition_over(int cells[][21], int over) {                          //印出遊戲結束的狀況(每個格子之資訊)
     int i, j;
     printf("\n\n\n\n\n\n                                ┌－－－－－－－－－－－－－－－－－－－┐\n");
     for (i = 1; i <= 19; i++) {
         for (j = 0; j <= 20; j++) {
             if (j  == 0)
                printf("                                ｜");
             else if (j == 20)
                  printf("｜");
             else if(cells[i][j] % 20 == 0)
                  printf("．");
             else if (cells[i][j] == -1)
                  printf(" *");
             else if (cells[i][j] == -2)
                  printf("⊕");
             else
                 printf("%2d", cells[i][j] % 20);
         }
         printf("\n");
     }
     printf("                                └－－－－－－－－－－－－－－－－－－－┘\n");
     if (over == 1)
        printf("                               Congratulations~~~~~\n\n");
     else
        printf("                               Boooooooom!\n\n");
     printf("                               the figure represent how many mines are around eight sites\n");
     printf("                                * represent mine\n");
     printf("                               ． represent there didn't have mine around\n");
     printf("                               ⊕ represent the mine which exploded\n");
     printf("                                  thanks for your playing.\n");
}

void print_cells(int cells[][21], int op_x, int op_y, int over) {               //印出目前的狀況
     system("cls");
     printf("\n");
     if (over == 0) {
        print_condition_playing(cells);
     } else
           print_condition_over(cells, over);
}

int  check_over(int cells[][21], int op_x, int op_y) {                          //檢查遊戲是否結束(輸或贏)
    //result == 0 ->尚未結束，result == 1 ->贏，result == -1 ->輸
    int i, j, result = 1;
    if (cells[op_x][op_y] == -2) {
       result = -1;
    } else {
           for (i = 1; i <= 20 && result == 1; i++)
               for (j = 1; j <= 20 && result == 1; j++)
                   if ((cells[i][j] >= 0 && cells[i][j] <= 19))
                      result = 0;
    }
    return(result);
}

char play_again(void) {                                                         //詢問是否再玩一次
     char again;
     printf("\n                               Do you want to play again?");
     do {
        printf("\n                               If you want to play again please input Y\n");
        printf("                               or input N to close the game. >>");
        scanf(" %c", &again);
     } while (again != 'Y' && again != 'y' && again != 'N' && again != 'n');
     return(again);
    return 0;
}


int my_game_two(void)
{
    int IsEnding = 0;
    char logo[] = "MY BRILLIANT (OR FUNNY) GAME";
    int i, j, k;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick;

    /* 啟動鍵盤控制 整個程式中只要做一次就行了*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    while (!IsEnding) {
        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */
        cur_tick = timer();
        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = timer();
            clearScreen();
            i = rand()%15;
            j = rand()%15;
            drawChar(0xA1, j*2 + OFFSET_X, i + OFFSET_Y,  0, 8);
            drawChar(0xBD, j*2+1 + OFFSET_X, i + OFFSET_Y,  0, 8);
            drawString(logo,OFFSET_X, OFFSET_Y-2,3,14);
            sprintf(str, "%10d", timer()/100);
            drawString(str,OFFSET_X, OFFSET_Y-1,1,14);
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_ESCAPE:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }  /*while (IsEnding)*/

    return 0;
}
int ending(){
    int IsEnding = 0;
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick,k,count=0;

    Image* background;
    background = read_image("789.pixel", "789.color");
    show_image(background,0,0);
    saveScreen();
    cur_tick = last_tick = timer();
    while (!IsEnding) {
        //畫面更新
        cur_tick = timer();
        if(cur_tick-last_tick > 100){
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            show_image(background,0,0);

            count ++;
            drawCmdWindow();
        }
        clearScreen();
            restoreScreen();
             drawCmdWindow();

        //鍵盤控制
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }
    return 0;
}

int about(void){
    int IsEnding = 0;
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick,k,count=0;

    Image* background;
    background = read_image("555.pixel", "555.color");
    show_image(background,0,0);
    saveScreen();
    cur_tick = last_tick = timer();
    while (!IsEnding) {
        //畫面更新
        cur_tick = timer();
        if(cur_tick-last_tick > 100){
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            show_image(background,0,0);

            count ++;
            drawCmdWindow();
        }
        clearScreen();
            restoreScreen();
             drawCmdWindow();

        //鍵盤控制
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }
    return 0;



}
