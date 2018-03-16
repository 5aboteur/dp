#include "tcp_echo.h"

static int sockfd;

int
main(int argc, char *argv[])
{
	int bytes, rc, serv_len, connected;
	struct sockaddr_in serv_addr;

	serv_len = sizeof(serv_addr);

	if (argc < 2) {
		port = _DEF_PORT;
		printf(_RED_CLR"[System]"_DEF_CLR" using default port: %d\n", port);
	} else
		port = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		printf(_RED_CLR"[System] "_DEF_CLR);
		perror("socket");
		exit(EXIT_FAILURE);
	}

	memset((char *) &serv_addr, 0, serv_len);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	connected = 0;

	do {
		rc = connect(sockfd, (struct sockaddr *) &serv_addr, serv_len);
		if (rc >= 0)
			connected = 1;
		else {
			puts(_YELLOW_CLR"Client:"_DEF_CLR" connection refused...\n");
			sleep(1);
		}
	} while (!connected);

	printf(_YELLOW_CLR"Client:"_DEF_CLR" connected!\n");

	signal(SIGCHLD, (__sighandler_t) killproc);

	while (0x1) {
		char chkmsg[MSGSIZ];

		bytes = recv(sockfd, chkmsg, MSGSIZ, 0);
		if (bytes < 0) {
			printf(_RED_CLR"[System] "_DEF_CLR);
			perror("send");
			exit(EXIT_FAILURE);
		}

		printf(_YELLOW_CLR"Client:"_DEF_CLR" check message: %s\n", chkmsg);

		bytes = send(sockfd, chkmsg, MSGSIZ, 0);
		if (bytes < 0) {
			printf(_RED_CLR"[System] "_DEF_CLR);
			perror("recv");
			exit(EXIT_FAILURE);
		}

		sleep(1);
	}

	return 0;
}

void
killproc(void)
{
	int status = 0;
	wait(&status);
	close(sockfd);
}