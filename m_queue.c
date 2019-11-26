/*
 * m_queue.c
 *
 *  Created on: Nov 22, 2019
 *      Author: sdragoi
 */

#include "m_queue.h"

/* Helper function used in creating queue name */
int count_digits(int x) {
	int count = 0;

	if (0 == x) {
		return 1;
	} else {
		while (0 != x) {
			count++;
			x /= 10;
		}
	}

	return count;
}

void* client_job(void *arg) {
	char *string_id;
	char *queue_name;
	char message_buffer[BUF_SIZE];

	int id, id_length, rc, token;

	mqd_t message_queue_client;
	mqd_t message_queue_server;

	thread_info_t* thread_info;
	thread_info = (thread_info_t*)arg;

	id = thread_info->id;

	/* Create a string from client's ID in order to concatenate to queue name
	 * Client's queue name will look like this: "/client_queue_n"
	 */
	id_length = count_digits(id);
	string_id = malloc((id_length + 1) * sizeof(string_id));
	if (NULL == string_id) {
		printf("Malloc error, string_id\n");
		return NULL;
	}
	snprintf(string_id, id_length + 1, "%d", id);
	string_id[id_length] = '\0';
	queue_name = malloc((strlen(CLIENT_QUEUE) + strlen(string_id) + 1) * sizeof(queue_name));
	if (NULL == queue_name) {
		printf("Malloc error, queue name\n");
		return NULL;
	}
	strncpy(queue_name, CLIENT_QUEUE, strlen(CLIENT_QUEUE));
	queue_name[strlen(CLIENT_QUEUE)] = '\0';
	strncat(queue_name, string_id, strlen(string_id));

	printf("Client %d started\n", id);

	message_queue_client = mq_open(queue_name, O_CREAT | O_RDWR, 0600, NULL);
	if (message_queue_client < 0) {
		printf("Unable to open queue for client %d\n", id);
		free(string_id);
		free(queue_name);
		return NULL;
	}

	message_queue_server = mq_open(SERVER_QUEUE, O_CREAT | O_RDWR, 0600, NULL);
	if (message_queue_server < 0) {
		printf("Client %d unable to open server queue\n", id);
		free(string_id);
		free(queue_name);
		return NULL;
	}

	rc = mq_send(message_queue_server, queue_name, strlen(queue_name), 0);
	if (rc < 0) {
		printf("Client %d unable to send\n", id);
		free(string_id);
		free(queue_name);
		return NULL;
	}

	rc = mq_receive(message_queue_client, message_buffer, BUF_SIZE, 0);
	if (rc < 0) {
		printf("Receiving error on client %d\n", id);
		free(string_id);
		free(queue_name);
		return NULL;
	}

	token = 0;
	memcpy(&token, message_buffer, sizeof(int));

	printf("Client %d got the following token %d\n", id, token);

	rc = mq_close(message_queue_client);
	if (rc < 0) {
		printf("Error closing client %d queue\n", id);
	}
	rc = mq_close(message_queue_server);
	if (rc < 0) {
		printf("Error closing server queue\n");
	}

	rc = mq_unlink(queue_name);
	if (rc < 0) {
		printf("Error unlinking client %d queue\n", id);
	}

	free(string_id);
	free(queue_name);

	return NULL;
}

void server_job() {
	int rc, token;
	char message_buffer[BUF_SIZE], client_queue_name[BUF_SIZE];
	mqd_t message_queue_client;
	mqd_t message_queue_server;

	 /* Seed the PRNG */
	struct timespec ts;
	if (timespec_get(&ts, TIME_UTC) == 0) {
		printf("Unable to generate seed\n");
	}
	srandom(ts.tv_nsec ^ ts.tv_sec);

	message_queue_server = mq_open(SERVER_QUEUE, O_CREAT | O_RDWR, 0600, NULL);
	if (message_queue_server < 0) {
		printf("Server unable to open queue\n");
		return;
	}

	printf("Server started\n");

	while(1) {
		memset(message_buffer, 0, BUF_SIZE);
		rc = mq_receive(message_queue_server, message_buffer, BUF_SIZE, 0);
		if (rc < 0) {
			printf("Receiving error on server\n");
			continue;
		}

		/* Condition to stop the program */
		if (0 == strncmp(message_buffer, EXIT_STRING, strlen(EXIT_STRING))) {
			rc = mq_close(message_queue_server);
			if (rc < 0) {
				printf("Error closing server queue\n");
			}

			rc = mq_unlink(SERVER_QUEUE);
			if (rc < 0) {
				printf("Error unlinking server queue\n");
			}

			return;
		}

		memset(client_queue_name, 0, sizeof(client_queue_name));
		memcpy(client_queue_name, message_buffer, strlen(message_buffer));
		memset(message_buffer, 0, sizeof(message_buffer));

		token = random();
		memcpy(message_buffer, &token, sizeof(int));

		message_queue_client = mq_open(client_queue_name, O_CREAT | O_RDWR, 0600, NULL);
		if (message_queue_client < 0) {
			printf("Server unable to open %s\n", client_queue_name);
			continue;
		}

		rc = mq_send(message_queue_client, message_buffer, strlen(message_buffer), 0);
		if (rc < 0) {
			printf("Server unable to send to %s\n", client_queue_name);
		}

		rc = mq_close(message_queue_client);
		if (rc < 0) {
			printf("Error closing %s\n", client_queue_name);
		}
	}
}
