#pragma once

#include <pthread.h>

typedef struct zem {
	int value;
	pthread_cond_t cond;
	pthread_mutex_t lock;
} zem_t;

void zem_init(zem_t *, int);

void zem_wait(zem_t *);

void zem_post(zem_t *);
