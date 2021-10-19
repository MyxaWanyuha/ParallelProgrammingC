#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1
#define N 2
int turn;
int interested[N] = {0,0};
void enter_region(int process)
{
    int other;
    other = 1 - process;
    interested[process] = TRUE;
    turn = process;
    while(turn == process && interested[other] == TRUE);
}

void leave_region(int process)
{
    interested[process] = FALSE;
}

typedef struct
{
    char c;
    int id;
} thread_data;

#define size 10
char g_string[size + 1];

void thread_func(thread_data* data)
{
    enter_region(data->id);
    for(int i = 0; i < size; ++i)
    {
        g_string[i] = data->c;
        Sleep(50);
    }
    printf("%s\n", g_string);
    leave_region(data->id);
}

void* work_for_thread(void* arg)
{
    thread_func((thread_data*)arg);
    return NULL;
}

int main()
{
    g_string[size] = '\0';
    const int threads_count = 2;

    pthread_t* threads = (pthread_t*)calloc(threads_count, sizeof(pthread_t));
    if( threads == NULL )
    {
        fprintf( stderr, "Bad alloc!\n" );
        exit(-2);
    }

    thread_data datas[2];
    datas[0].c = 'a';
    datas[0].id = 0;
    datas[1].c = 'b';
    datas[1].id = 1;

    for(char i = 0; i < threads_count; ++i)
    {
        int t = pthread_create(&threads[i], NULL, work_for_thread, (void*)&datas[i]);
        if(t != 0)
        {
            fprintf(stderr, "Can't create thread!\n");
            return -3;
        }
    }

    for( int i = 0; i < threads_count; ++i )
    {
        pthread_join( threads[ i ], NULL );
    }

    free(threads);
    return 0;
}
