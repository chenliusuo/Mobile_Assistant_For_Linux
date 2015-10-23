/*
 * server.h
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_SOCKET_NUM 1024
#define PACKET_LEN 1024
#define SERV_IP "127.0.0.1"
#define SERV_PORT 12580
#define CLI_PORT 12580

//tcp通信用结构体
typedef struct __ass_msg{
    int serv_sockfd;
    int cli_sockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t serv_len;
    socklen_t cli_len;
    int sendmsglen;
    int recvmsglen;
    long sendmsg[PACKET_LEN];
    long recvmsg[PACKET_LEN];
}ass_msg_t;

typedef struct __ass_msgset{
    int msgfd;
    ass_msg_t *msg;
}ass_msgset_t;

int unused_msgfd(void);
int server_create();
int client_create();
int server_accept(int);
int get_cli_sockfd(int);
int recv_time_out(int ,char *,int);
void close_cli_socket(int);

#endif /* SERVER_H_ */
