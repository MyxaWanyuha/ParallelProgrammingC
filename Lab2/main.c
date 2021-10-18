#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

uint32_t min(const uint32_t num1, const uint32_t num2)
{
    return (num1 > num2) ? num2 : num1;
}

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

void check_ptr(void* ptr)
{
    if(ptr == NULL)
    {
        fprintf( stderr, "Bad alloc!\n" );
        abort();
    }
}

double f(size_t n)
{
    //variant 3
    return 1.0 / ((3 * n - 2) * (3 * n - 1));
}

typedef struct
{
    uint32_t start;
    uint32_t end;
    double result;
} block_data;

void* accumulate_block(void* arg)
{
    block_data* data = (block_data*)arg;

    data->result = 0.0;
    for(uint32_t i = data->start; i < data->end; ++i)
    {
        data->result += f(i);
    }
    return NULL;
}

int main()
{
    int n = 0;
    printf("Input n: ");
    if(scanf("%d", &n) != 1 || n < 0)
    {
        fprintf( stderr, "Bad input!\n" );
        exit(-1);
    }

    const uint32_t first = 0;
    const uint32_t last = n;

    const uint32_t length = last - first;
    const uint32_t min_per_thread = 25;
    const uint32_t max_threads = (length + min_per_thread - 1) / min_per_thread;
    const uint32_t hardware_threads = get_threads_count();
    const uint32_t num_threads = min(hardware_threads, max_threads);
    const uint32_t block_size = length / num_threads;

    block_data* results = calloc(num_threads, sizeof(block_data));
    check_ptr(results);
    pthread_t* threads = (pthread_t*)calloc(num_threads - 1, sizeof(pthread_t));
    check_ptr(threads);

    uint32_t block_start = first;
    for(uint32_t i = 0; i < (num_threads - 1); ++i)
    {
        uint32_t block_end = block_start;
        block_end += block_size;

        results[i].start = block_start;
        results[i].end = block_end;
        int thread = pthread_create(&threads[i], NULL, accumulate_block, (void*)&results[i]);
        if(thread != 0)
        {
            fprintf(stderr, "Can't create thread!\n");
            return -4;
        }

        block_start = block_end;
    }

    results[num_threads - 1].start = block_start;
    results[num_threads - 1].end = last;
    accumulate_block((void*)&results[num_threads - 1]);//work for main thread

    for(uint32_t i = 0; i < num_threads - 1; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    double result = 0.0;
    for(uint32_t i = 0; i < num_threads; ++i)
    {
        result += results[i].result;
    }

    free(threads);
    free(results);

    printf("%f\n", result);

    return 0;
}
