/*
 * agps_demo.c
 * 
 * Copyright 2015 Jarod Lee
 * 
 * This code is a demonstration of how to use CASIC agps service.
 * 
 * It takes four steps to complete an agps routine:
 * 1. Establish a connection to agps server with socket.
 * 2. Send a message to agps server to request agps data.
 * 3. Receive data, which contains agps information, from agps server.
 * 4. Send agps information to gnss module through tty.
 * 
 * However, step 4 is not included in this code, but with a line of 
 * hint at the end. 
 * 
 * Agps routine should be done when a gnss module is powered on.
 * 
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CAS_AGPS_SERVER "www.gnss-aide.com"
#define CAS_AGPS_PORT 2621
#define USER_NAME "freetrial"
#define PASS_WORD "123456"
#define LAT 30.11
#define LON 120.21

int main(int argc, char **argv)
{
	printf("AGPS demo.\n");
	int sockfd, n, counter;
	int portno = CAS_AGPS_PORT;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char message[128];	
	int agps_buffer_length;
	char agps_buffer[8 * 1024];
	char buffer[1024];
	char *p;
	
	printf("Create a socket.\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		printf("ERROR, opening socket.\n");
		return 0;
	}
	
	printf("Get agps server by name.\n");
	server = gethostbyname(CAS_AGPS_SERVER);
	if (server == NULL){
		printf("ERROR, no such host.\n");
		return 0;
	}
	
	printf("Copy agps server information to socket address struct\n");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *) &serv_addr.sin_addr.s_addr,
		   (char *) server->h_addr,
		   server->h_length);
	serv_addr.sin_port = htons(portno);
	
	printf("Connect to agps server.\n");
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("ERROR, cannot connect to server.\n");
		return 0;
	}
	
	printf("Build agps message.\n");
	sprintf(message, "user=%s;pwd=%s;lat=%.1f;lon=%.1f;", USER_NAME, PASS_WORD, LAT, LON);
	printf("Message: %s\n", message);
	
	printf("Send message to agps server.\n");
	n = write(sockfd, message, strlen(message));
	if (n < 0) {
		printf("ERROR, writing to socket.\n");
		return 0;
	}	
	
	printf("Wait for server transferring agps data.\n");
	p = agps_buffer;
	agps_buffer_length = 0;
	for (counter = 0; counter < 100; counter++) {
		n = read(sockfd, buffer, 1024);	
		if (n < 0) {
			printf("ERROR, reading from socket.\n");
			return 0;
		}
		else if (n == 0 ) {
			printf("All agps data are received.\nTotal bytes: %d\n", agps_buffer_length);
			break;
		}
		else {
			//~ printf("\nLength: %d\n\n%s", n, buffer);
			printf("Received %d bytes from agps server.\n", n);	
			memcpy(p, buffer, n);
			p += n;
			agps_buffer_length += n;
		}
	}
	
	close(sockfd);
	
	// The code below is for DEBUG purpose
	if (0) {		
		printf("Demostrate agps_buffer, agps_buffer_length = %d.\n\n", agps_buffer_length);
		int row = 0;
		int col = 0;
		for (counter = 0; counter < agps_buffer_length; counter++) {
			unsigned char c = agps_buffer[counter];

			if (row < 3) {
				printf("%c", c);
			}
			else {
				printf("%02X ", c);
				col += 1;
				if (col >= 20) {
					printf("\n");
					col = 0;
				}
			}		
			if (c == '\n') {
				row += 1;
			}
		}
	}	

	printf("\n\n");
	printf("Now, you can send these data to gnss module through tty.\n");
	printf("Using code like:\nwrite(tty_fd, agps_buffer, agps_buffer_length);\n");
	
	return 0;
}

