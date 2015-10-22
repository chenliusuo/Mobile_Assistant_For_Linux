/*
 * ass.cpp
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */
#include "ass.h"


static ass_pthread_t ass_server_pthread;
static ass_pthread_t ass_send_pthread;
static pthread_mutex_t ass_pthread_mutex;	//保证send_pthread与recv_pthread交替
static pthread_mutex_t send_flag_mutex;		//保护send_cmd_flag
static pthread_mutex_t recv_flag_mutex;		//保护recv_cmd_flag
static sem_t ass_pthread_sem;

int serv_sockfd,cli_sockfd;
int send_or_recv = 0;						//0允许recv_pthread,1允许send_pthread
char recv_cmd_flag = 0x00;
char send_cmd_flag = 0x00;
int end_flag = 0;

extern contact_cmd_t contact_cmd;
extern message_t message_cmd;
extern message_pack_t message_pack;
extern message_t new_message;
extern int new_message_flag;
extern image_pack_t image_pack;
extern audio_pack_t audio_pack;
extern video_pack_t video_pack;

//创建脱离线程ass_server_pthread（执行server_pthread)
JNIEXPORT jint JNICALL Java_com_android_ass_AssService_pthread_1init
(JNIEnv * env, jclass thiz)
{
	//初始化信号量与互斥锁
	sem_init(&ass_pthread_sem,0,0);
	pthread_mutex_init(&ass_pthread_mutex,NULL);
	pthread_mutex_init(&send_flag_mutex,NULL);
	pthread_mutex_init(&recv_flag_mutex,NULL);
	//初始化创建server_pthread
	pthread_attr_init(&ass_server_pthread.attr);
	pthread_attr_setdetachstate(&ass_server_pthread.attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&ass_server_pthread.tid,&ass_server_pthread.attr,server_pthread,NULL);
	pthread_attr_destroy(&ass_server_pthread.attr);
	end_flag = 0;
	send_cmd_flag = 0x00;
	recv_cmd_flag = 0x00;
	send_or_recv = 0;
	LOGD("server_thread init");
	return 0;
}

//创建socket并监听，连接客户端创建发送线程
void *server_pthread(void *)
{
	serv_sockfd = server_create();
	if(serv_sockfd < 0 )
	{
		LOGE("server_create error");
	}
	cli_sockfd = server_accept(serv_sockfd);
	if(cli_sockfd < 0)
	{
		LOGE("server_accept error");
	}

	pthread_attr_init(&ass_send_pthread.attr);
	pthread_attr_setdetachstate(&ass_send_pthread.attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&ass_send_pthread.tid,&ass_send_pthread.attr,send_pthread,NULL);
	pthread_attr_destroy(&ass_send_pthread.attr);

	LOGD("send_pthread init");
	//监听cli_sockfd并接收命令
	while(end_flag == 0)
	{
		if(send_or_recv == 0)
		{
			pthread_mutex_lock(&ass_pthread_mutex);
			LOGD("recv_pthread lock");
			recv_package();
			LOGD("recv_pthread unlock");
			pthread_mutex_unlock(&ass_pthread_mutex);
			send_or_recv = 1;
		}
		usleep(5000);
	}
	LOGD("recv_pthread end");
	//结束自身线程
	pthread_exit(NULL);
}

//等待java端调用send_cmd_flag后，执行发送
void *send_pthread(void *)
{
	while(end_flag == 0)
	{
		usleep(1000);
		if(send_or_recv == 1)
		{
			pthread_mutex_lock(&ass_pthread_mutex);
			LOGD("send_pthread lock");
			sem_wait(&ass_pthread_sem);
			pthread_mutex_lock(&send_flag_mutex);
			send_package();
			pthread_mutex_unlock(&send_flag_mutex);
			LOGD("send_pthread unlock");
			pthread_mutex_unlock(&ass_pthread_mutex);
			send_or_recv = 0;
		}
		usleep(4000);
	}
	LOGD("send_pthread end");
	//结束自身线程
	pthread_exit(NULL);
}

//接收PC端命令
void recv_package()
{
	int ret;
	int i;
	char recv_sig_buf = 0;

	//等待连续3次sig_start
	for(i=0; i<3; i++)
	{
		ret = recv_time_out(serv_sockfd,&recv_sig_buf,sizeof(char));
		if(ret != sizeof(char))
		{
			//设置变量代表连接断开
			recv_cmd_flag = CMD_CONNECT_BREAK;
			LOGE("connect break");
			//结束自身线程
			pthread_exit(NULL);
		}
		if(recv_sig_buf == sig_start)
		{
			recv_sig_buf = 0;
		}
		else
		{
			i = -1;
			recv_sig_buf = 0;
			sleep(1);
		}
	}

	LOGD("recv sig_start");

	//接收命令
	ret = recv_time_out(serv_sockfd,&recv_sig_buf,sizeof(char));
	if(ret != sizeof(char))
	{
		//设置变量代表连接断开
		recv_cmd_flag = CMD_CONNECT_BREAK;
		LOGE("connect break");
		//结束自身线程
		pthread_exit(NULL);
	}

	switch(recv_sig_buf)
	{
		case CMD_CONTACTS:
			LOGD("recv cmd_contact");
			break;
		case CMD_INSERT_CONTACT:
			LOGD("recv cmd_insert_contact");
			ret = recv_time_out(serv_sockfd,(char *)&contact_cmd,sizeof(contact_cmd_t));		//获取增加的联系人
			if(ret != sizeof(contact_cmd_t))
			{
				//设置变量代表连接断开
				recv_cmd_flag = CMD_CONNECT_BREAK;
				LOGE("connect break");
				//结束自身线程
				pthread_exit(NULL);
			}
			break;
		case CMD_DELETE_CONTACT:
			LOGD("recv cmd_delete_contact");
			ret = recv_time_out(serv_sockfd,(char *)&contact_cmd,sizeof(contact_cmd_t));		//获取删除的联系人
			if(ret != sizeof(contact_cmd_t))
			{
				//设置变量代表连接断开
				recv_cmd_flag = CMD_CONNECT_BREAK;
				LOGE("connect break");
				//结束自身线程
				pthread_exit(NULL);
			}
			break;
		case CMD_UPDATE_CONTACT:
			LOGD("recv cmd_update_contact");
			ret = recv_time_out(serv_sockfd,(char *)&contact_cmd,sizeof(contact_cmd_t));		//获取修改的联系人
			if(ret != sizeof(contact_cmd_t))
			{
				//设置变量代表连接断开
				recv_cmd_flag = CMD_CONNECT_BREAK;
				LOGE("connect break");
				//结束自身线程
				pthread_exit(NULL);
			}
			break;
		case CMD_MESSAGE:
			LOGD("recv cmd_message");
			break;
		case CMD_SEND_MESSAGE:
			LOGD("recv cmd_send_message");
			ret = recv_time_out(serv_sockfd,(char *)&message_cmd,sizeof(message_t));			//接收待发短信的联系人与内容
			if(ret != sizeof(message_t))
			{
				//设置变量代表连接断开
				recv_cmd_flag = CMD_CONNECT_BREAK;
				LOGE("connect break");
				//结束自身线程
				pthread_exit(NULL);
			}
			break;
		case CMD_APPINFO:
			LOGD("recv cmd_appinfo");
			break;
		case CMD_IMAGE:
			LOGD("recv cmd_image");
			break;
		case CMD_AUDIO:
			LOGD("recv cmd_audio");
			break;
		case CMD_VIDEO:
			LOGD("recv cmd_video");
			break;
		case CMD_RECV_MESSAGE:
			LOGD("recv cmd_recv_message");
			break;
		default:
			break;
	}
	//设置recv_cmd_flag，等待java端get_cmd_flag
	if(recv_cmd_flag == 0x00 || recv_cmd_flag == CMD_RECV_MESSAGE)
	{
        pthread_mutex_lock(&recv_flag_mutex);
        if(recv_sig_buf != 0x00)
        {
            fprintf(stderr,"recv_flag_lock\n");
        }
        recv_cmd_flag = recv_sig_buf;
        pthread_mutex_unlock(&recv_flag_mutex);
        if(recv_sig_buf != 0x00)
        {
            fprintf(stderr,"recv_flag_unlock\n");
        }
	}
	recv_sig_buf = 0;
}

//返还执行结果
void send_package()
{
	int i;
	int package_size = 0;
	//发送3次sig_start
	for(i=0;i<3;i++)
	{
		send(cli_sockfd,&sig_start,sizeof(char),0);
	}
	//发送命令
	send(cli_sockfd,&send_cmd_flag,sizeof(char),0);
	//根据不同命令发送不同消息
	switch(send_cmd_flag)
	{
		case CMD_CONTACTS:
			send_file("/data/data/com.android.ass/databases/contacts.db3");		//发送contact.db3
			break;
		case CMD_INSERT_CONTACT:
			break;
		case CMD_DELETE_CONTACT:
			break;
		case CMD_UPDATE_CONTACT:
			break;
		case CMD_MESSAGE:
			package_size = sizeof(int)+sizeof(message_t)*(message_pack.count);	//发送短信
			send(cli_sockfd,&package_size,sizeof(int),0);
			send(cli_sockfd,&message_pack,package_size,0);
			break;
		case CMD_SEND_MESSAGE:
			break;
		case CMD_APPINFO:
			send_file("/data/data/com.android.ass/databases/appinfo.db3");		//发送appinfo.db3
			send_dir("/data/data/com.android.ass/files/appicon/");				//发送应用图标
			break;
		case CMD_IMAGE:
			package_size = sizeof(int)+sizeof(image_t)*(image_pack.count);		//发送图片信息
			send(cli_sockfd,&package_size,sizeof(int),0);
			send(cli_sockfd,&image_pack,package_size,0);
			send_dir("/data/data/com.android.ass/files/imageThumbnail/");		//发送图片缩略图
			break;
		case CMD_AUDIO:
			package_size = sizeof(int)+sizeof(audio_t)*(audio_pack.count);		//发送音频信息
			send(cli_sockfd,&package_size,sizeof(int),0);
			send(cli_sockfd,&audio_pack,package_size,0);
			break;
		case CMD_VIDEO:
			package_size = sizeof(int)+sizeof(video_t)*(video_pack.count);		//发送视频信息
			send(cli_sockfd,&package_size,sizeof(int),0);
			send(cli_sockfd,&video_pack,package_size,0);
			send_dir("/data/data/com.android.ass/files/videoThumbnail/");		//发送视频缩略图
			break;
		case CMD_RECV_MESSAGE:
			send(cli_sockfd,&new_message_flag,sizeof(int),0);					//发送新消息标志
			LOGD("send %d ",new_message_flag);
			if(new_message_flag != 0)											//若有消息则发送该结构体
			{
				send(cli_sockfd,&new_message,sizeof(message_t),0);
				new_message_flag = 0;
			}
			break;
		default:
			break;
	}
	send_cmd_flag = 0;
}

//发送dir_path指定的文件夹中内容
void send_dir(const char * dir_path)
{
	DIR * dir = NULL;
	struct dirent * dir_ptr;
	int file_count = 0;

	char **file_set = NULL;
	file_set = (char **)malloc(sizeof(char *)* MAX_FILE_ITEMS);
	memset(file_set,'\0',sizeof(char *)*MAX_FILE_ITEMS);

	LOGD("send dir_path %s",dir_path);

	//获取并发送文件数量,并将文件名转为路径存储到file_set
	if((dir = opendir(dir_path)) != NULL)
	{
		while((dir_ptr = readdir(dir)) != NULL && file_count<MAX_FILE_ITEMS)
		{
			if(strcmp(dir_ptr->d_name,".") == 0 || strcmp(dir_ptr->d_name,"..") == 0)
			{
				continue;
			}
			file_set[file_count] = (char *)malloc(sizeof(char)*FILE_PATH_LEN);
			memset(file_set[file_count],'\0',sizeof(char)*FILE_PATH_LEN);
			strcpy(file_set[file_count],dir_path);
			strcat(file_set[file_count],dir_ptr->d_name);
			file_count++;
		}
	}
	send(cli_sockfd,&file_count,sizeof(int),0);
	LOGD("file_count %d",file_count);

	for(file_count--;file_count >= 0;file_count--)
	{
		send_file(file_set[file_count]);
		free(file_set[file_count]);
	}
	free(file_set);
	closedir(dir);
}

//发送file_path指定文件的大小及数据
void send_file(const char * file_path)
{
	char *send_buf = NULL;
	struct stat file_stat_buf;
	int file_size;
	char *file_name = NULL;
	FILE *fp = NULL;
	int ret;

	//将file_name指向文件名并发送
	file_name = strrchr(file_path,'/') + 1;
	if(file_name == NULL)
	{
		return;
	}
	file_size = (int)strlen(file_name);
	send(cli_sockfd,&file_size,sizeof(int),0);
	send(cli_sockfd,file_name,strlen(file_name),0);

	//获取文件大小并发送
	ret = stat(file_path,&file_stat_buf);
	if(ret == -1)
	{
		file_size = 0;
	}
	else
	{
		file_size = file_stat_buf.st_size;
	}
	send(cli_sockfd,&file_size,sizeof(int),0);

	//发送对应文件
	fp = fopen(file_path,"r");
	if(fp == NULL)
	{
		LOGE("fopen error");
	}
	send_buf = (char *)malloc(BUF_SIZE*sizeof(char));
	while(file_size > 0)
	{
		memset(send_buf,'\0',BUF_SIZE);
		ret = fread(send_buf,sizeof(char),BUF_SIZE,fp);
		if(ret != 0)
		{
			send(cli_sockfd,send_buf,ret,0);
			file_size -= ret;
		}
	}
	fclose(fp);
	free(send_buf);
}

//返回recv_cmd_flag，并重置为0
JNIEXPORT jint JNICALL Java_com_android_ass_AssService_get_1cmd_1flag
  (JNIEnv * env, jclass thiz)
{
	int tmp_cmd_flag;
	if(pthread_mutex_trylock(&recv_flag_mutex) != 0)
	{
		return 0x00;
	}
	tmp_cmd_flag = recv_cmd_flag;
	recv_cmd_flag = 0;
	pthread_mutex_unlock(&recv_flag_mutex);
	return tmp_cmd_flag;
}

//设置发送命令，并post信号量
JNIEXPORT void JNICALL Java_com_android_ass_AssService_send_1cmd_1flag
  (JNIEnv *, jclass, jint send_cmd)
{
	if(send_cmd_flag != 0x00)
	{
		LOGE("send_cmd_flag != 0");
		return;
	}
	if(pthread_mutex_trylock(&send_flag_mutex) != 0)
	{
		LOGD("send_flag_mutex lock failed");
		return;
	}
	send_cmd_flag = send_cmd;
	sem_post(&ass_pthread_sem);
	pthread_mutex_unlock(&send_flag_mutex);
	return;
}


//设定结束标志量,结束C线程及清理(0仅结束线程并关闭socket;1销毁信号量互斥锁)
JNIEXPORT void JNICALL Java_com_android_ass_AssService_set_1end_1flag
  (JNIEnv *, jclass,jint option)
{
	if(option >= 0)
	{
		end_flag = 1;
		LOGD("set end_flag");

		//关闭socket
		close_serv_socket(serv_sockfd);
		LOGD("socket close");
	}
	if(option >= 1)
	{
		//销毁信号量与互斥锁
		pthread_mutex_destroy(&ass_pthread_mutex);
		pthread_mutex_destroy(&send_flag_mutex);
		pthread_mutex_destroy(&recv_flag_mutex);
		sem_destroy(&ass_pthread_sem);
		LOGD("mutex/sem destroy");
	}
}
