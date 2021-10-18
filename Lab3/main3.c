#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;

#define size 10
char g_string[size + 1];

void thread_func(char key)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < size; ++i)
    {
        g_string[i] = key;
        Sleep(50);
    }
    printf("%s\n", g_string);
    pthread_mutex_unlock(&mutex);
}

void* work_for_thread(void* arg)
{
    thread_func((char)arg);
    return NULL;
}

int main()
{
    g_string[size] = '\0';
    int threads_count = 0;
    printf("thread count: ");
    if(scanf("%d", &threads_count) != 1 || threads_count < 0)
    {
        fprintf(stderr, "Bad input!\n");
        return -1;
    }

    pthread_t* threads = (pthread_t*)calloc(threads_count, sizeof(pthread_t));
    if( threads == NULL )
    {
        fprintf( stderr, "Bad alloc!\n" );
        exit(-2);
    }

    pthread_mutex_init(&mutex, NULL);
    for(char i = 0; i < threads_count; ++i)
    {
        int t = pthread_create(&threads[i], NULL, work_for_thread, (void*)('a' + i));
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

    pthread_mutex_destroy(&mutex);
    free(threads);
    return 0;
}
