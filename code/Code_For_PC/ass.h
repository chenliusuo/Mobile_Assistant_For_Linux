/*
 * ass.h
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#ifndef ASS_H_
#define ASS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "package_info.h"
#include "server.h"

const char sig_start = 0xaa;

//线程ID及线程创建使用的pthread_attr_t attr
typedef struct __ass_pthread{
    pthread_t tid;
    pthread_attr_t attr;
}ass_pthread_t;

int main_pthread_init();
void *main_pthread();
int client_pthread_init();
void *client_pthread();
int send_pthread_init();
void *send_pthread();
void recv_package();
void send_package();
void exit_send_pthread();
void recv_file(char *);
void recv_dir(char *);

#endif /* ASS_H_ */
