/*
 * main.c
 *
 *  Created on: Nov 21, 2019
 *      Author: sdragoi
 */
#include <stdio.h>
#include <string.h>

#include <fcntl.h>            /* Defines O_* constants */
#include <sys/stat.h>         /* Defines mode constants */
#include <mqueue.h>

/* Set buffer size at least the default maxim size of the queue
  * found in/proc/sys/kernel/msgmax */
#define BUF_SIZE 			(1<<13)
#define CLIENT_QUEUE			"/client_queue_"
#define SERVER_QUEUE			"/server_queue"
#define TEST_CLIENTS_NUMBER		5

typedef struct {
	int id;
} thread_info_t;


int count_digits(int x) {
	int count;
	count = 0;

	while (0 != x) {
		count++;
		x /= 10;
	}

	return count;
}

unsigned int prio = 10;

void* client_job(void *arg) {
	int id;
	thread_info_t* thread_info;
	thread_info = (thread_info_t*)arg;

	id = thread_info->id;

	/* Transform client's ID in a string in order to concatenate to queue name */
	int id_length;
	char *string_id;

	id_length = count_digits(id);
	string_id = malloc((id_length + 1) * sizeof(string_id));
	sprintf(string_id, "%d", id);
	string_id[strlen(string_id)] = '\0';

	/* Client's queue name will look like this: "/client_queue_n" */
	char *queue_name;
	queue_name = malloc((strlen(CLIENT_QUEUE) + TEST_CLIENTS_NUMBER / 10 + 1) * sizeof(queue_name));
	strcpy(queue_name, CLIENT_QUEUE);
	strcat(queue_name, string_id);

	mqd_t message_queue;
	message_queue = mq_open();
	// rc = mq_send(m, TEXT, strlen(TEXT), prio);
	// rc = mq_close(m);
	// rc = mq_unlink(NAME);
}
char buf[BUF_SIZE];

int main(int argc, char **argv)
{
	int i;
	mqd_t message_queue;
	if (strncmp(argv[1], "client", strlen("client") == 0)) {
		pthread_t threads[TEST_CLIENTS_NUMBER];
		thread_info_t thread_info[TEST_CLIENTS_NUMBER];

		for (i = 0; i < TEST_CLIENTS_NUMBER; i++) {
			thread_info[i].id = i;
			if (pthread_create(&threads[i], NULL, &client_job, (void *)&thread_info[i])) {
				printf("Error creating thread number %d\n", i);
				return 1;
			}
		}
	} else if (strncmp(argv[1], "server", strlen("server") == 0)) {
		// rc = mq_receive(m, buf, BUF_SIZE, &prio);
		// rc = mq_close(m);
		// rc = mq_unlink(NAME);
	} else {
		printf("Invalid argument!\n");
		return 0;
	}
	return 0;
}
