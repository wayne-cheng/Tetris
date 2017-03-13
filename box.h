#ifndef BOX_H
#define BOX_H

#include <pthread.h>

#define HIGH 21
#define LENGTH 11

extern rm_i;
extern pthread_mutex_t mutex;

/*保存光标位置*/
#define SV_CUR() do{printf("\033[s");}while(0)

/*恢复光标位置*/
#define RS_CUR() do{printf("\033[u");}while(0)

/*nA nB nC nD分别表示上、下、左、右移动n行*/
#define RM_CUR(X,Y) do{for(rm_i=0; rm_i<=X; rm_i++){printf("\033[2C");}\
    for(rm_i=0; rm_i<=Y; rm_i++){printf("\033[1B");}}while(0)

/*7种方块*/
enum BOX
{
    BOX_I = 0,
    BOX_J,
    BOX_T,
    BOX_L,
    BOX_O,
    BOX_Z,
    BOX_S
};

/*颜色*/
enum COLOR
{
    RED = 0,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BLACK,
    CLEAR
};


extern int box_type_num;//其值即上面枚举的值之一
extern int next_box_type_num;

/*基本的7种方块定义，方块都是由4个坐标构成*/
extern int box_image[7][4][2];

/*各种方块具体的形状,每种都分四种，因为都可以旋转四次*/
extern int box_image_i[4][4][2];

extern int box_image_j[4][4][2];

extern int box_image_t[4][4][2];

extern int box_image_l[4][4][2];

extern int box_image_o[4][4][2];

extern int box_image_z[4][4][2];

extern int box_image_s[4][4][2];

/*可供方块活动的区域，若某个点被占用，会填充相应的值*/
extern box_domain[12][22];

/*用来存储方块的x和y轴坐标，前面已说明方块必须四个坐标表示*/
extern int box_x[4];
extern int box_y[4];

extern int box_next_x[4];
extern int box_next_y[4];

/*这是表示方块的三维数组的第三维下标，可以表示旋转位置*/
extern int shape_position;

extern int pause_flag;

extern int score;
extern int level;
extern int sleep_time[5];

extern void cell_print(int x, int y, int color);

extern void box_print();

extern int box_roll(int box_image_tmp[4][4][2]);

extern void box_move_left();

extern void box_move_right();

extern void box_move_down();

extern void box_move_up();

extern int falling_box();

extern int falling_box_stop();

extern void game_init();

extern void game_pause();

extern void game_exit();

extern void game_over();

extern void box_line_clear();

extern void box_line_delete(int n);

extern void next_box_print();

extern void box_clear();

#endif
