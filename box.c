#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "box.h"

int box_type_num;//其值即上面枚举的值之一
int next_box_type_num;

/*基本的7种方块定义，方块都是由4个坐标构成*/
int box_image[7][4][2]={
    {{0,1},{1,1},{2,1},{3,1}},
    {{0,0},{0,1},{1,1},{2,1}},
    {{1,0},{0,1},{1,1},{2,1}},
    {{2,0},{0,1},{1,1},{2,1}},
    {{0,0},{1,0},{0,1},{1,1}},
    {{0,0},{0,1},{1,1},{1,2}},
    {{1,0},{0,1},{1,1},{0,2}}
};

/*各种方块具体的形状,每种都分四种，因为都可以旋转四次*/
int box_image_i[4][4][2]={
    {{0,1},{1,1},{2,1},{3,1}},
    {{1,3},{1,2},{1,1},{1,0}},
    {{0,1},{1,1},{2,1},{3,1}},
    {{1,3},{1,2},{1,1},{1,0}}
};//实际仅两种

int box_image_j[4][4][2]={
    {{0,0},{0,1},{1,1},{2,1}},
    {{0,2},{1,2},{1,1},{1,0}},
    {{2,1},{2,0},{1,0},{0,0}},
    {{1,0},{0,0},{0,1},{0,2}}
};

int box_image_t[4][4][2]={
    {{1,0},{0,1},{1,1},{2,1}},
    {{0,1},{1,2},{1,1},{1,0}},
    {{1,1},{2,0},{1,0},{0,0}},
    {{2,1},{1,0},{1,1},{1,2}}
};

int box_image_l[4][4][2]={
    {{2,0},{0,1},{1,1},{2,1}},
    {{0,0},{1,0},{1,1},{1,2}},
    {{0,1},{0,0},{1,0},{2,0}},
    {{1,2},{0,2},{0,1},{0,0}}
};

int box_image_o[4][4][2]={
    {{0,0},{1,0},{0,1},{1,1}},
    {{0,0},{1,0},{0,1},{1,1}},
    {{0,0},{1,0},{0,1},{1,1}},
    {{0,0},{1,0},{0,1},{1,1}}
};//一种

int box_image_z[4][4][2]={
    {{0,0},{0,1},{1,1},{1,2}},
    {{0,1},{1,1},{1,0},{2,0}},
    {{0,0},{0,1},{1,1},{1,2}},
    {{0,1},{1,1},{1,0},{2,0}}
};//两种

int box_image_s[4][4][2]={
    {{1,0},{0,1},{1,1},{0,2}},
    {{0,0},{1,1},{1,0},{2,1}},
    {{1,0},{0,1},{1,1},{0,2}},
    {{0,0},{1,1},{1,0},{2,1}}
};//两种

/*可供方块活动的区域，x轴方向12单位,y轴方向22单位*/
int box_domain[12][22] = {0};//后面填充-1的区域表示合法。

/*用来存储方块的x和y轴坐标，前面已说明方块必须四个坐标表示*/
int box_x[4] = {0};
int box_y[4] = {0};

int box_next_x[4] = {0};
int box_next_y[4] = {0};

/*这是表示方块的三维数组的第三维下标，可以表示旋转位置*/
int shape_position = 0;

int pause_flag = 0;

int score;
int level;
int sleep_time[5] = {500000, 300000, 200000, 100000, 50000};

int rm_i;

pthread_mutex_t mutex;

void cell_print(int x, int y, int color)
{
    SV_CUR();
    RM_CUR(x, y);
    switch(color)
    {
    case BLACK   :
        /*设置打印属性\033 xxx \033xx (e=033)*/
        /*关于VT100,40-47为背景颜色,30-71为字体颜色,0关闭属性*/
        printf("\033[40;30m  \033[0m");
        break;
    case RED     :
        printf("\033[41;31m  \033[0m");
        break;
    case GREEN   :
        printf("\033[42;32m  \033[0m");
        break;
    case YELLOW  :
        printf("\033[43;33m  \033[0m");
        break;
    case BLUE    :
        printf("\033[44;34m  \033[0m");
        break;
    case MAGENTA :
        printf("\033[45;35m  \033[0m");
        break;
    case CYAN    :
        printf("\033[46;36m  \033[0m");
        break;
    case WHITE   :
        printf("\033[47;37m  \033[0m");
        break;
    case CLEAR   :
        printf("\033[8m  \033[0m");
        break;
    default      :
        break;  
    }
    RS_CUR();
}

void box_print()
{
    int i;
    int x,y;

    for(i=0; i<4; i++)
    {
        x = box_x[i];
        y = box_y[i];
        if(y > 0)
        {
            cell_print(x, y, box_type_num);
        }
    }
}

void box_line_clear()
{
    int x,y;
    int i,j;
    int m[10]={0};
    int n;
    int tmp=0;

    /* check full line */
    for(y=1; y<21; y++)
    {
        for(x=1; x<11; x++)
        {
            if(box_domain[x][y] == -1)
            {
                break;
            }
        }
        if(x == 11)
        {
            m[tmp] = y;
            tmp++;
        }
    }

    if(tmp == 0) 
    {
        return;
    }

    /* delete full line */
    for(i=0; i<tmp; i++){
        n = m[i];
        box_line_delete(n);
    }

    /* calc the score */
    score = score + pow(tmp,2);
    SV_CUR();
    RM_CUR(13,4);
    printf("Score:  %d", score);
    if((score > 30) && (score < 80)){
        level=1;
        printf("Lever:  2");
    }else if((score > 80) && (score < 150)){
        level=2;
        printf("Lever:  3");
    }else if((score > 150) && (score < 250)){
        level=3;
        printf("Lever:  4");
    }else if(score > 250){
        level=4;
        printf("Lever:  5");
    }else{
       // do nothing.
    }
    RS_CUR();

    fflush(stdout);
    return;
}

void box_line_delete(int n)
{
    int x,y;

    for(x=1; x<LENGTH; x++)
    {
        y = n;
        cell_print(x,y,CLEAR);
        box_domain[x][y] =-1;
    }

    for(y=n; y>1; y--){
        for(x=1; x<LENGTH; x++)
        {
            if(box_domain[x][y-1] != -1)
            {
                box_domain[x][y] = box_domain[x][y-1];
                cell_print(x,y,box_domain[x][y]);
            }else
            {
                cell_print(x,y,CLEAR);
                box_domain[x][y] =-1;
            }
        }
    }

    for(x = 1; x < LENGTH; x++)
    {
        cell_print(x,1,CLEAR);
        box_domain[x][1] =-1;
    }

    return;
}

void next_box_print()
{
    int i,j;
    int x,y;

    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            x = i+14;
            y = j+8;
            cell_print(x,y,CLEAR);
        }
    }

    for(i=0; i<4; i++){
        x = box_next_x[i] + LENGTH + 3;
        y = box_next_y[i] + 8;
        cell_print(x, y, next_box_type_num);
    }

    fflush(stdout);
    return;
}

void box_clear()
{
    int i;
    int x,y;

    for(i=0; i<4; i++)
    {
        x = box_x[i];
        y = box_y[i];
        if(y > 0)
        {
            cell_print(x,y,CLEAR);
        }
    }

    return;
}

/*旋转方块*/
int box_roll(int box_image_tmp[4][4][2])
{
    int i;
    int x, y;
    int shape_position_tmp;
    int delta_x, delta_y;

    /*记录方块即将旋转后的位置*/
    if(shape_position >= 3)
    {
        shape_position_tmp = 0;
    }
    else
    {
        shape_position_tmp = shape_position + 1;
    }

    delta_x = box_x[0] - box_image_tmp[shape_position][0][0];
    delta_y = box_y[0] - box_image_tmp[shape_position][0][1];

    /*判断旋转后的位置是否合法*/
    for(i=0; i<4; i++)
    {

        x = delta_x + box_image_tmp[shape_position_tmp][i][0];
        y = delta_y + box_image_tmp[shape_position_tmp][i][1];

        if((y > 0) && (box_domain[x][y] != -1)) 
        {
            return false;
        }
    }

    box_clear();

    shape_position = shape_position_tmp;

    for(i=0; i<4; i++)
    {
        box_x[i] = delta_x + box_image_tmp[shape_position][i][0];
        box_y[i] = delta_y + box_image_tmp[shape_position][i][1];
    }

    box_print();

    return true;
}

/*移动方块*/
void box_move_left()
{
    int i;
    int x,y;
    
    if(pause_flag == 1)//如果暂停，就退出
    {
        return;
    }

    pthread_mutex_lock(&mutex);//上锁

    for(i = 0; i < 4; i++)//4个坐标均要移动，而且需要都要判断
    {
        /*坐标向左移动，先记录移动后的坐标。用以判断合不合法*/
        x = box_x[i] - 1;//用--会改变坐标值，这里只是取值。
        y = box_y[i];

        /*移动后，若方块的坐标不合法，则立即返回*/
        if((y > 0) && (box_domain[x][y] != -1))
        {
            pthread_mutex_unlock(&mutex);
            return;
        }
        /*否则继续判断其他点坐标*/
    }

    box_clear();//清除原来的方块

    for(i = 0; i < 4; i++)//现在可以改变x轴坐标了
    {
        --box_x[i];
    }

    box_print();//打印方块

    fflush(stdout);//避免输出缓存残留信息，影响下次打印
    pthread_mutex_unlock(&mutex);//解锁

}

void box_move_right()
{
    int i;
    int x,y;

    if(pause_flag == 1)
    {
        return;
    }

    pthread_mutex_lock(&mutex);

    for(i = 0; i < 4; i++)
    {
        x = box_x[i] + 1;
        y = box_y[i];

        if((y > 0) && (box_domain[x][y] != -1))
        {
            pthread_mutex_unlock(&mutex);
            return;
        }
    }

    box_clear();

    for(i=0; i<4; i++)
    {
        ++box_x[i];
    }

    box_print();

    fflush(stdout);
    pthread_mutex_unlock(&mutex);   
}

void box_move_down()
{
    int i;
    int x, y;

    int future_y, delta_y = HIGH - 1, delta_y_tmp = 0;

    if(pause_flag == 1)
    {
        return;
    }

    pthread_mutex_lock(&mutex);

    for(i = 0; i < 4; i++)//四个坐标，判断四次
    {
        x = box_x[i];
        y = box_y[i];

        //判断下落过程有没有碰撞
        for(future_y = y + 1; future_y <= HIGH; future_y++)
        {
            if((y > 0) && (box_domain[x][future_y] != -1))
            {
                delta_y_tmp = future_y - y -1;
                break;
            }
        }
        if(delta_y_tmp < delta_y)//取最小的delta_y
        {
            delta_y = delta_y_tmp;
        }
    }

    if(delta_y == 0)
    {
        pthread_mutex_unlock(&mutex);
        return;
    }

    box_clear();

    for(i=0; i<4; i++)
    {
        box_y[i] = box_y[i] + delta_y;
    }

    box_print();

    fflush(stdout);
    pthread_mutex_unlock(&mutex);
}

void box_move_up()
{
    int res=0;
    int i;
    int x,y;
    int x1,y1;


    if(pause_flag == 1)
    {
        return;
    }

    pthread_mutex_lock(&mutex);

    switch(box_type_num)
    {
    case BOX_I:
        res = box_roll(box_image_i);
        break;
    case BOX_J :
        res = box_roll(box_image_j);
        break;
    case BOX_T :
        res = box_roll(box_image_t);
        break;
    case BOX_L :
        res = box_roll(box_image_l);
        break;
    case BOX_O :
        res = box_roll(box_image_o);
        break;
    case BOX_Z :
        res = box_roll(box_image_z);
        break;
    case BOX_S :
        res = box_roll(box_image_s);
        break;
    defult:
        break;
    }

    if( res == false)
    {
        pthread_mutex_unlock(&mutex);
        return;
    }

    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    return;
}

/*方块是自动下落的*/
int falling_box()
{
    int i;
    int x,y;

    pthread_mutex_lock(&mutex);

    for(i = 0; i < 4; i++)//整个循环用来判断方块能否继续下落
    {
        x = box_x[i];
        y = box_y[i] + 1;

        if((y > 0) && (box_domain[x][y] != -1))//y=0是顶端边框，要排除。当盒子下落时产生碰撞
        {
            if(false == falling_box_stop())//游戏失败,返回错误，下落停止
            {
                pthread_mutex_unlock(&mutex);
                return false;
            }
            pthread_mutex_unlock(&mutex);//正常碰撞，返回正确，下落停止
            return true;
        }
    }

    box_clear();//每下落一个单位，都要清除上一次方块的位置

    for(i = 0; i < 4; i++)
    {
        box_y[i] = box_y[i] + 1;
    }

    box_print();

    fflush(stdout);

    pthread_mutex_unlock(&mutex);
    return true;
}

/*方块停止下落后的工作*/
int falling_box_stop()
{
    int i;
    int x, y;
    int delta_y = 0, tmp_y = 0;

    for(i = 0; i < 4; i++)
    {
        x = box_x[i];
        y = box_y[i];
        if(y <= 0)
        {
            return false;//因游戏失败停止下落, 返回错误
        }   
            
        box_domain[x][y] = box_type_num;
    }

    /*清除满的一行，并计算分数 */
    box_line_clear();

    /*准备下一个方块*/
    box_type_num = next_box_type_num;
    shape_position = 0;

    for(i=0; i<4; i++)//确定下一个方块的y偏移值
    {
        tmp_y = box_next_y[i];
        if(delta_y < tmp_y)
        {
            delta_y = tmp_y;
        }
    }
    for(i=0; i<4; i++)//确定下一个方块的坐标
    {
        box_x[i] = box_next_x[i] + 5;
        box_y[i] = box_next_y[i] - delta_y;
    }
    box_print();


    next_box_type_num = rand() % 7;
    for(i=0; i<4; i++)
    {
        box_next_x[i] = box_image[next_box_type_num][i][0];
        box_next_y[i] = box_image[next_box_type_num][i][1]; 
    }
    next_box_print();

    fflush(stdout);
    return true;//正常停止下落
}

void game_init()
{
    int i, j;
    int delta_y, tmp_y;

    delta_y = 0;
    tmp_y = 0;

    system("clear");

    /*对方块活动区域进行初始化*/
    for(i = 0; i <= LENGTH; i++)
    {
        for(j = 0; j <= HIGH; j++)
        {
            box_domain[i][j] = -1;//可用区域都设置为-1
        }
    }

    /*隐藏光标*/
    printf("\033[?25l");

    /*打印边框*/
    for(i = 0; i <= LENGTH; i++)
    {
        cell_print(i, 0, WHITE);
        cell_print(i, HIGH, WHITE);
    }
    for(i = 0; i <= HIGH; i++)
    {
        cell_print(0, i, WHITE);
        cell_print(LENGTH, i, WHITE);

    }

    /*打印提示信息*/
    SV_CUR();
    RM_CUR(13,2);
    printf("Level:  1");
    RS_CUR();

    SV_CUR();
    RM_CUR(13,4);
    printf("Score:  0");
    RS_CUR();

    SV_CUR();
    RM_CUR(13,6);
    printf("Next:");
    RS_CUR();

    SV_CUR();
    RM_CUR(13,12);
    printf("Instructions:");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,13);
    printf("A: go to left");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,14);
    printf("D: go to rgiht");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,15);
    printf("W: roll cube");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,16);
    printf("S: go to down");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,17);
    printf("P: run or pause");
    RS_CUR();

    SV_CUR();
    RM_CUR(14,18);
    printf("E: exit the game");
    RS_CUR();

    fflush(stdout);

    /*上下边框都被占用，记录为WHITE */
    for(i = 0; i <= LENGTH; i++)
    {
        box_domain[i][0] = WHITE;
        box_domain[i][HIGH] = WHITE;
    }
    for(i = 0; i <=HIGH; i++)
    {
        box_domain[0][i] = WHITE;
        box_domain[LENGTH][i] = WHITE;
    }

    srand((unsigned)time(NULL));

    /* 获得当前的方块，*/
    box_type_num = rand() % 7;
    for(i=0; i<4; i++)
    {
        tmp_y = box_image[box_type_num][i][1];
        if(delta_y < tmp_y)
        {
            delta_y = tmp_y;
        }//逐个判断方块每个点的y坐标，找最大值。
    }
    for(i=0; i<4; i++)
    {
        box_x[i] = box_image[box_type_num][i][0] + 5;
        box_y[i] = box_image[box_type_num][i][1] - delta_y; 
    }
    box_print();
    
    next_box_type_num = rand() % 7;
    for(i=0; i<4; i++)
    {
        box_next_x[i] = box_image[next_box_type_num][i][0];//刚开始，方块处于顶端中间位置
        box_next_y[i] = box_image[next_box_type_num][i][1]; 
    }
    next_box_print();//打印用来提示的下一个方块
}

void game_pause()
{
    if(pause_flag == 0)
    {
        pause_flag = 1;
        pthread_mutex_lock(&mutex);
    }
    else
    {
        pause_flag = 0;
        pthread_mutex_unlock(&mutex);
    }
}

void game_exit()
{
    system("clear");

    /*stty用来设置终端打印 stty -a可查看终端打印设置情况
     有-的表示已设置*/
    system("stty icanon");//取消一次性读完操作
    system("stty echo");//取消不显示命令

    printf("\033[?25h");
    exit(0);
}

void game_over()
{
    int x,y;

    for(x = 3; x < 10; x++)
    {
        for(y = 9; y < 13; y++)
        {
            cell_print(x, y, WHITE);
        }
    }

    SV_CUR();
    RM_CUR(4,10);
    printf("\033[1;31m");
    printf("GAME OVER \033[0m");
    RS_CUR();

    SV_CUR();
    RM_CUR(4,LENGTH);
    printf("\033[1;34m Score:%3d \033[0m", score);
    RS_CUR();

    return;
}






#if 0
/*打印图案*/
void box_print(int box[], int len)
{
    int x = 0, y = 0;
    int i;
    for(i = 0; i < len; i = i + 2)
    {
        x = left + 2 * box[i];
        y = top + box[i+1];


        printf("\033[%d;%dH", y, x);//设置光标位置
        printf("\033[;42m");//设置字体颜色

        printf("[*]");//打印图案
    }

    printf("\033[0m");//关闭所有属性；
}
#endif

#if 0
int main(int argc, const char *argv[])
{
    int i = 0;

    //计算数组长度
    int len = sizeof(box0) / sizeof(box0[0]);
    
    print_box(box0, len);

    return 0;
}
#endif
