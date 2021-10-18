#include <stdio.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

size_t get_threads_count()
{
    long count = sysconf( _SC_NPROCESSORS_ONLN );
    if ( count < 1 )
    {
        fprintf(stderr, "Error!\n");
        abort();
    }
    return count;
}

void* work_for_thread( void* arg_data )
{
    long long data = (long long)arg_data;
    data *= 1000;
    return (void*)data;
}

int main( int argc, char ** argv )
{
    const size_t arr_count = get_threads_count();
    pthread_t* threads = (pthread_t*)calloc( arr_count, sizeof( pthread_t ) );
    if( threads == NULL )
    {
        fprintf( stderr, "Bad alloc!\n" );
        abort();
    }

    for( ptrdiff_t i = 0; i < arr_count; ++i )
    {
        int t = pthread_create( &threads[i], NULL, work_for_thread, (void*)i );
        if( t != 0 )
        {
            fprintf( stderr, "Can't create thread!\n" );
            return -1;
        }
    }

    for( int i = 0; i < arr_count; ++i )
    {
        void* res;
        pthread_join( threads[ i ], &res );
        printf( "Thread %i return %i\n", i, (long long)res );
    }

    printf( "Main thread ended\n" );
    free( threads );
    return 0;
}
