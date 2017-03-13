#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "box.h"

void *key_handle(void *ptr)
{
    char key;

    while(1)
    {
        system("stty -icanon -echo");
        key = getchar();

        if((key >= 'a') && (key <= 'z'))
        {
            key = key - 32;
        }

        switch(key)
        {
        case 'A' :
            box_move_left();
            break;
        case 'D' :
            box_move_right();
            break;
        case 'W' :
            box_move_up();
            break;
        case 'S' :
            box_move_down();
            break;
        case 'P' :
            game_pause();
            break;
        case 'E' :
            game_exit();
            break;
        default  :
            break;
        }

    }

}

int main(int argc, const char *argv[])
{
    int res = 0;
    pthread_t box_thread;

    pthread_mutex_init(&mutex, NULL);

    game_init();

    res = pthread_create(&box_thread, NULL, key_handle, NULL);
    if(res != 0)
    {
        perror("Start game error:");
        return false;
    }

    res = true;

    while(1)
    {
        fflush(stdout);
        usleep(sleep_time[level]);
        res = falling_box();//判断方块是否溢出
        if(false == res)//如果溢出
        {
            game_over();
            fflush(stdout);
            sleep(5);
            game_exit();
        }
    }
}

