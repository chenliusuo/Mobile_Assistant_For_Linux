/*
 * ass.h
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#ifndef ASS_H_
#define ASS_H_

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include "mylog.h"
#include "com_android_ass_AssService.h"
#include "package_info.h"
#include "server.h"

const char sig_start = 0xaa;        //传输开始信号

//线程ID及线程创建使用的pthread_attr_t attr
typedef struct __ass_pthread{
    pthread_t tid;
    pthread_attr_t attr;
}ass_pthread_t;

void *server_pthread(void *);
void *send_pthread(void *);
void send_contacts();
void recv_package();
void send_package();
void send_file(const char *);
void send_dir(const char *);

#endif /* ASS_H_ */
