#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define Sleep(x) usleep((x)*1000)
void check_ptr(void* ptr)
{
    if(ptr == NULL)
    {
        fprintf( stderr, "Bad alloc!\n" );
        abort();
    }
}

int* tickets = NULL;
int* choosing = NULL;
int threads_count;

#define size 10
char g_string[size + 1];

typedef struct
{
    char c;
    int id;
} thread_data;

void lock(int thread)
{
	choosing[thread] = 1;
	int max_ticket = 0;

	for (int i = 0; i < threads_count; ++i)
	{
		int ticket = tickets[i];
		max_ticket = ticket > max_ticket ? ticket : max_ticket;
	}

	tickets[thread] = max_ticket + 1;
	choosing[thread] = 0;

	for (int other = 0; other < threads_count; ++other)
	{
		while (choosing[other]) {}

		while (tickets[other] != 0 &&
				(tickets[other] < tickets[thread] ||
				(tickets[other] == tickets[thread] && other < thread))) {}
	}
}

void unlock(int thread)
{
	tickets[thread] = 0;
}

void* work_for_thread(void* arg)
{
	thread_data* data = (thread_data*)arg;
	lock(data->id);
	
    for(int i = 0; i < size; ++i)
    {
        g_string[i] = data->c;
        Sleep(50);
    }
    printf("%s\n", g_string);

	unlock(data->id);
	return NULL;
}

int main(int argc, char** argv)
{
	g_string[size] = '\0';

	printf("Enter threads count: ");
	if(scanf("%i", &threads_count) != 1 || threads_count < 1)
	{
		printf("Bad input\n");
		return -1;
	}

	tickets = (int*)calloc(threads_count, sizeof(int));
	check_ptr(tickets);
	memset((void*)tickets, 0, sizeof(tickets[0]) * threads_count);

	choosing = (int*)calloc(threads_count, sizeof(int));
	check_ptr(choosing);
	memset((void*)choosing, 0, sizeof(choosing[0]) * threads_count);

    pthread_t* threads = (pthread_t*)calloc(threads_count, sizeof(pthread_t));
	check_ptr(threads);

	thread_data* datas = (thread_data*)calloc(threads_count, sizeof(thread_data));
	check_ptr(threads);

	for (int i = 0; i < threads_count; ++i)
	{
		datas[i].id = i;
		datas[i].c = 'a' + i;
		pthread_create(&threads[i], NULL, &work_for_thread, (void*)(&datas[i]));
	}

	for (int i = 0; i < threads_count; ++i)
	{
		pthread_join(threads[i], NULL);
	}


	free(tickets);
	free(choosing);
	free(threads);
	free(datas);
	return 0;
}
