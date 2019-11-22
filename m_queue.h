/*
 * m_queue.h
 *
 *  Created on: Nov 22, 2019
 *      Author: sdragoi
 */

#ifndef M_QUEUE_H_
#define M_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>				/* For random() seed */

#include <fcntl.h>				/* Defines O_* constants */
#include <mqueue.h>
#include <pthread.h>

#define BUF_SIZE 				(1<<13) /* Set buffer size at least the default maxim size of the queue
  	  	  	  	  	  	  	  	  	  	 * found in /proc/sys/kernel/msgmax */
#define CLIENT_QUEUE			"/client_queue_"
#define SERVER_QUEUE			"/server_queue"
#define CLIENT_ARG				"client"
#define SERVER_ARG				"server"
#define EXIT_STRING				"exit"
#define TEST_CLIENTS_NUMBER		42

/* Structure used to store each thread id */
typedef struct {
	int id;
} thread_info_t;

int count_digits(int);
void* client_job(void*);
void server_job();

#endif /* M_QUEUE_H_ */
