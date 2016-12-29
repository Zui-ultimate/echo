#ifndef _THREAD_HANDLE_H_
#define _THREAD_HANDLE_H_

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

void Pthread_detach(pthread_t thread);

#endif
