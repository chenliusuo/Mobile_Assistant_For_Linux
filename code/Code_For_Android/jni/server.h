/*
 * server.h
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "mylog.h"

#define MAX_SOCKET_NUM 1024
#define PACKET_LEN 1024
#define SERV_PORT 12580

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
int server_accept(int);
void close_serv_socket(int);
int recv_time_out(int ,char *,int);

#endif /* SERVER_H_ */
