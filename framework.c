#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>


#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "framework.h"

/*----------------------------------------------------------*/


int IsOnItem(Menu *m, int index)
{
    return (m->state[index] & 1); /* 用位元運算來判斷目前選擇的項目 */
}
int IsItemSelected(Menu *m, int index)
{
    return (m->state[index] & 2);
}
void showMenu(Menu *m)
{
    int i;
    int fg;
    char *str;

    for (i=0; i< m->num_options; i++) {

        if ( !IsOnItem(m, i) ) {
            fg = m->fgcolor;
        } else {
            fg = m->bgcolor;
        }

        if ( !IsItemSelected(m, i) ) {
            str = m->text[i];
        } else {
            str = m->alt_text[i];
        }

        putStringLarge(m->large_font, (m->x)*(m->large_font->width+2),
            (m->y+i)*(m->large_font->height+2), str, fg);
    }
}

void scrollMenu(Menu *m, int diff)
{
    m->state[m->cursor] = m->state[m->cursor] & (~1);  /* 把目前游標所在的選項狀態清除 */
    m->cursor = (m->cursor + diff + m->num_options) % m->num_options; /* 把遊標移到下一個選項 */
    m->state[m->cursor] = m->state[m->cursor] | 1; /* 選擇目前游標所在的選項 */
}

void radioMenu(Menu *m)
{
    int i;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = m->state[i] & (~2); /* 清掉全部的選項 */
    }
    m->state[m->cursor] = m->state[m->cursor] | 2; /* 設定目前游標 */
}
void toggleMenu(Menu *m)
{
    m->state[m->cursor] = m->state[m->cursor] ^ 2;  /* 利用位元運算 產生 toggle 的效果 */
}




