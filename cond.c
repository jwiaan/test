#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int g = 1;
pthread_barrier_t barrier;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *thread(void *p)
{
	while (1) {
		pthread_mutex_lock(&mutex);
		if (g == 0) {
			printf("%lu wait\n", pthread_self());
			pthread_cond_wait(&cond, &mutex);
			printf("%lu wake\n", pthread_self());
		}

		printf("%lu get %d\n", pthread_self(), g);
		assert(g);
		--g;
		pthread_mutex_unlock(&mutex);
		pthread_barrier_wait(&barrier);
	}
}

int main(void)
{
	pthread_barrier_init(&barrier, NULL, 2);
	for (int i = 0; i < 2; ++i) {
		pthread_t t;
		pthread_create(&t, NULL, thread, NULL);
	}

	sleep(1);
	pthread_mutex_lock(&mutex);
	++g;
	printf("main put %d\n", g);
	pthread_barrier_wait(&barrier);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	pause();
}
