#ifndef BOX_H
#define BOX_H

#include <pthread.h>

#define HIGH 21
#define LENGTH 11

extern rm_i;
extern pthread_mutex_t mutex;

/*������λ��*/
#define SV_CUR() do{printf("\033[s");}while(0)

/*�ָ����λ��*/
#define RS_CUR() do{printf("\033[u");}while(0)

/*nA nB nC nD�ֱ��ʾ�ϡ��¡������ƶ�n��*/
#define RM_CUR(X,Y) do{for(rm_i=0; rm_i<=X; rm_i++){printf("\033[2C");}\
    for(rm_i=0; rm_i<=Y; rm_i++){printf("\033[1B");}}while(0)

/*7�ַ���*/
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

/*��ɫ*/
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


extern int box_type_num;//��ֵ������ö�ٵ�ֵ֮һ
extern int next_box_type_num;

/*������7�ַ��鶨�壬���鶼����4�����깹��*/
extern int box_image[7][4][2];

/*���ַ���������״,ÿ�ֶ������֣���Ϊ��������ת�Ĵ�*/
extern int box_image_i[4][4][2];

extern int box_image_j[4][4][2];

extern int box_image_t[4][4][2];

extern int box_image_l[4][4][2];

extern int box_image_o[4][4][2];

extern int box_image_z[4][4][2];

extern int box_image_s[4][4][2];

/*�ɹ�������������ĳ���㱻ռ�ã��������Ӧ��ֵ*/
extern box_domain[12][22];

/*�����洢�����x��y�����꣬ǰ����˵����������ĸ������ʾ*/
extern int box_x[4];
extern int box_y[4];

extern int box_next_x[4];
extern int box_next_y[4];

/*���Ǳ�ʾ�������ά����ĵ���ά�±꣬���Ա�ʾ��תλ��*/
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
