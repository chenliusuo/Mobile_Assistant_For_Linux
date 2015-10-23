/*
 * server.cpp
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#include "server.h"

#include <stdio.h>

ass_msgset_t msgset[MAX_SOCKET_NUM];

//默认创建SERV_PORT端口号tcp的socket，返回msg_serial_num
int server_create()
{
    int msg_serial_num;
    if( (msg_serial_num = unused_msgfd() ) == 0)
   	{
    	return -1;
    }

    msgset[msg_serial_num].msg = (ass_msg_t *)malloc(sizeof(ass_msg_t));
    if(msgset[msg_serial_num].msg == NULL)
    {
    	msgset[msg_serial_num].msgfd = 0;
    	return -1;
    }
    memset(msgset[msg_serial_num].msg,'\0',sizeof(ass_msg_t));

    msgset[msg_serial_num].msg->serv_sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(msgset[msg_serial_num].msg->serv_sockfd == -1)
    {
    	free(msgset[msg_serial_num].msg);
    	msgset[msg_serial_num].msg = NULL;
    	msgset[msg_serial_num].msgfd = 0;
    	return -1;
    }
    msgset[msg_serial_num].msg->serv_len = sizeof(struct sockaddr);
    msgset[msg_serial_num].msg->cli_len = sizeof(struct sockaddr);

    memset(&msgset[msg_serial_num].msg->serv_addr,'\0',msgset[msg_serial_num].msg->serv_len);
    memset(&msgset[msg_serial_num].msg->cli_addr,'\0',msgset[msg_serial_num].msg->cli_len);

    msgset[msg_serial_num].msg->serv_addr.sin_family = AF_INET;
    msgset[msg_serial_num].msg->serv_addr.sin_port = htons(SERV_PORT);
    msgset[msg_serial_num].msg->serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(msgset[msg_serial_num].msg->serv_sockfd,(struct sockaddr *)&msgset[msg_serial_num].msg->serv_addr,msgset[msg_serial_num].msg->serv_len) == -1)
    {
    	free(msgset[msg_serial_num].msg);
		msgset[msg_serial_num].msg = NULL;
		msgset[msg_serial_num].msgfd = 0;
		return -1;
    }

    if(listen(msgset[msg_serial_num].msg->serv_sockfd,10) == -1)
    {
    	free(msgset[msg_serial_num].msg);
		msgset[msg_serial_num].msg = NULL;
		msgset[msg_serial_num].msgfd = 0;
		return -1;
    }

    return msg_serial_num;
}

//默认连接CLI_PORT端口号tcp的socket，返回msg_serial_num
int client_create()
{
	int msg_serial_num;
	if((msg_serial_num = unused_msgfd()) == 0)
	{
		return -1;
	}

	msgset[msg_serial_num].msg = (ass_msg_t *)malloc(sizeof(ass_msg_t));
	if(msgset[msg_serial_num].msg == NULL)
	{
		msgset[msg_serial_num].msgfd = 0;
		return -1;
	}
	memset(msgset[msg_serial_num].msg,'\0',sizeof(ass_msg_t));

	msgset[msg_serial_num].msg->cli_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(msgset[msg_serial_num].msg->cli_sockfd == -1)
	{
		free(msgset[msg_serial_num].msg);
		msgset[msg_serial_num].msg = NULL;
		msgset[msg_serial_num].msgfd = 0;
		return -1;
	}

	msgset[msg_serial_num].msg->serv_len = sizeof(struct sockaddr);
    msgset[msg_serial_num].msg->cli_len = sizeof(struct sockaddr);

    memset(&msgset[msg_serial_num].msg->serv_addr,'\0',msgset[msg_serial_num].msg->serv_len);
    memset(&msgset[msg_serial_num].msg->cli_addr,'\0',msgset[msg_serial_num].msg->cli_len);

    msgset[msg_serial_num].msg->serv_addr.sin_family = AF_INET;
	//默认端口CLI_PORT/SERV_IP见server.h
    msgset[msg_serial_num].msg->serv_addr.sin_port = htons(CLI_PORT);
    msgset[msg_serial_num].msg->serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	if(connect(msgset[msg_serial_num].msg->cli_sockfd,(struct sockaddr *)&msgset[msg_serial_num].msg->serv_addr,msgset[msg_serial_num].msg->serv_len) == -1)
	{
		fprintf(stderr,"connect failed\n");
		free(msgset[msg_serial_num].msg);
		msgset[msg_serial_num].msg = NULL;
		msgset[msg_serial_num].msgfd = 0;
		return -1;
	}

	return msg_serial_num;
}

//返回client的sockfd
int get_cli_sockfd(int msg_serial_num)
{
	return msgset[msg_serial_num].msg->cli_sockfd;
}

//通过msg_serial_num找到cli_sockfd，接收recv_len长度消息保存至recv_buf,超时等待3次time_out.tv_sec
int recv_time_out(int msg_serial_num,char *recv_buf,int recv_len)
{
	int ret;
	int try_time = 0;
	int len = 0;
	fd_set fd_read;

	FD_ZERO(&fd_read);
	FD_SET(msgset[msg_serial_num].msg->cli_sockfd,&fd_read);

	struct timeval time_out;
	time_out.tv_sec = 3;
	time_out.tv_usec = 0;

	while(len < recv_len)
	{
		ret = select(msgset[msg_serial_num].msg->cli_sockfd + 1,&fd_read,NULL,NULL,&time_out);
		if(ret == -1)
		{
			return -1;
		}
		else if(ret == 0)
		{
			try_time++;
			if(try_time > 3)
			{
				return len;
			}
		}
		else
		{
			if(FD_ISSET(msgset[msg_serial_num].msg->cli_sockfd,&fd_read) == 1)
			{
				ret = recv(msgset[msg_serial_num].msg->cli_sockfd,recv_buf+len,recv_len-len,MSG_NOSIGNAL);
				if(ret == -1 || ret == 0)
				{
					return -1;
				}
				else
				{
					len += ret;
				}
				try_time = 0;
			}
		}
	}

	return len;
}

//监听msg_serial_num指定的socket,成功后返回cli_sockfd
int server_accept(int msg_serial_num)
{
	msgset[msg_serial_num].msg->cli_sockfd = accept(msgset[msg_serial_num].msg->serv_sockfd,(struct sockaddr *)&msgset[msg_serial_num].msg->cli_addr,&msgset[msg_serial_num].msg->cli_len);
	if(msgset[msg_serial_num].msg->cli_sockfd == -1)
	{
		return -1;
	}
	return msgset[msg_serial_num].msg->cli_sockfd;
}

//遍历msg_set,获取最小序号未使用的消息结构体
int unused_msgfd(void)
{
	int i;
	for(i=1; i<MAX_SOCKET_NUM; i++)
	{
		if(msgset[i].msgfd != i)
		{
			break;
		}
	}
	if(i >= MAX_SOCKET_NUM)
	{
		return 0;
	}
	msgset[i].msgfd = i;
	return i;
}

//关闭socket，清理资源
void close_cli_socket(int msg_serial_num)
{
	close(msgset[msg_serial_num].msg->cli_sockfd);
	free(msgset[msg_serial_num].msg);
	msgset[msg_serial_num].msg = NULL;
	msgset[msg_serial_num].msgfd = 0;
}
