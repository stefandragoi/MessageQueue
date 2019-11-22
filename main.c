/*
 * main.c
 *
 *  Created on: Nov 21, 2019
 *      Author: sdragoi
 */
#include "m_queue.h"

int main(int argc, char **argv)
{
	int i;
	pthread_t client_threads[TEST_CLIENTS_NUMBER];
	thread_info_t client_thread_info[TEST_CLIENTS_NUMBER];


	if (0 == strncmp(argv[1], CLIENT_ARG, strlen(CLIENT_ARG))) {
		/* Create threads for every TEST_CLIENTS_NUMBER client */
		for (i = 0; i < TEST_CLIENTS_NUMBER; i++) {
			client_thread_info[i].id = i;
			if (pthread_create(&client_threads[i], NULL, &client_job, (void *)&client_thread_info[i])) {
				printf("Error creating client thread number %d\n", i);
				return 1;
			}
		}

		for (i = 0; i < TEST_CLIENTS_NUMBER; i++) {
			if (pthread_join(client_threads[i], NULL)) {
				printf("Error joining client thread number %d\n", i);
				return 1;
			}
		}
	} else if (0 == strncmp(argv[1], SERVER_ARG, strlen(SERVER_ARG))) {
		server_job();
	} else {
		printf("Invalid argument!\n");
		return 0;
	}
	return 0;
}
