/*
 * ass.c
 *
 *  Created on: 2015年1月4日
 *      Author: luke
 */
#include "ass.h"

static ass_pthread_t ass_main_pthread;
static ass_pthread_t ass_client_pthread;
static ass_pthread_t ass_send_pthread;
static pthread_mutex_t ass_pthread_mutex;
static pthread_mutex_t send_flag_mutex;		//保护send_cmd_flag
static pthread_mutex_t recv_flag_mutex;		//保护recv_cmd_flag
static sem_t ass_pthread_sem;

int cli_sockfd;								//connect返回的sockfd
int cli_serial_num;							//msgset的序列号
int send_or_recv = 1;						//0允许recv_pthread,1允许send_pthread
char recv_cmd_flag = 0x00;					//用于判断是否接收到消息
char send_cmd_flag = 0x00;					//发送标志
int end_flag = 0;							//线程结束标志，为1则跳出循环

message_pack_t message_pack;				//存储短信的结构体
image_pack_t image_pack;					//存储图片的结构体
audio_pack_t audio_pack;					//存储音频的结构体
video_pack_t video_pack;					//存储视频的结构体
contact_cmd_t contact_cmd;					//存储联系人增删改联系人的信息
message_t message_cmd;						//存储发送短信的结构体
message_t new_message;						//存储接收短信的结构体

//创建main_pthread并初始化程序
int main_pthread_init()
{
	//判断/tmp/ass是否存在,若不存在则创建
	if(opendir("/tmp/ass") == NULL)
	{
		if(mkdir("/tmp/ass",0775) == -1)
		{
			fprintf(stderr,"mkdir /tmp/ass failed\n");
			return 1;
		}
	}
	//初始化互斥锁与信号量
	pthread_mutex_init(&ass_pthread_mutex,NULL);
	pthread_mutex_init(&send_flag_mutex,NULL);
	pthread_mutex_init(&recv_flag_mutex,NULL);
	sem_init(&ass_pthread_sem,0,0);
	//初始化各标志量
    send_cmd_flag = 0x00;
    recv_cmd_flag = 0x00;
	//设置由send_pthread先加锁
	send_or_recv = 1;
	end_flag = 0;
	sleep(1);

	//创建脱离线程ass_main_pthread（执行main_pthread)
	pthread_attr_init(&ass_main_pthread.attr);
	pthread_attr_setdetachstate(&ass_main_pthread.attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&ass_main_pthread.tid,&ass_main_pthread.attr,main_pthread,NULL);
	pthread_attr_destroy(&ass_main_pthread.attr);
	fprintf(stderr,"main_pthread_init\n");
	return 0;
}

//负责创建client_pthread，监听变量结束程序
void *main_pthread()
{
	client_pthread_init();
	//监听变量结束各线程
	while(end_flag == 0)
	{
		sleep(1);
	}
    if(end_flag >= 1)
    {   
        //关闭socket
    	close_cli_socket(cli_serial_num);
        fprintf(stderr,"close sockfd\n");
		//结束接收线程
        pthread_cancel(ass_client_pthread.tid);
        send_cmd_flag = 0x00;
        recv_cmd_flag = 0x00;
	}   
	if(end_flag >= 2)
	{
		//销毁信号量与互斥锁
        pthread_mutex_destroy(&ass_pthread_mutex);
        pthread_mutex_destroy(&send_flag_mutex);
        pthread_mutex_destroy(&recv_flag_mutex);
        sem_destroy(&ass_pthread_sem);
    }
	//结束自身线程
	pthread_exit(NULL);
}

int client_pthread_init()
{
	//创建脱离线程ass_client_pthread（执行client_pthread)
	pthread_attr_init(&ass_client_pthread.attr);
	pthread_attr_setdetachstate(&ass_client_pthread.attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&ass_client_pthread.tid,&ass_client_pthread.attr,client_pthread,NULL);
	pthread_attr_destroy(&ass_client_pthread.attr);

	fprintf(stderr,"client_thread_init\n");

	return 0;
}


//默认connect 127.0.0.1 12580
void *client_pthread()
{
	//返回的msg_set序号(用于recv_time_out)
	cli_serial_num = client_create();
	//获取sockfd(用于send)
	cli_sockfd = get_cli_sockfd(cli_serial_num);
	if(cli_serial_num < 0)
	{
		fprintf(stderr,"client_create failed\n");
		return NULL;
	}
	fprintf(stderr,"connect success %d\n",cli_serial_num);
	//设定对于cancel请求的参数
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	send_pthread_init();
	
	//接收手机端信息(有可能阻塞，由main_pthread调用pthread_cancel结束)
	while(end_flag == 0)
	{
		usleep(1000);
		if(send_or_recv == 0)
		{
			pthread_mutex_lock(&ass_pthread_mutex);
			fprintf(stderr,"recv_pthread lock\n");
			recv_package();
			fprintf(stderr,"recv_pthread unlock\n");
			pthread_mutex_unlock(&ass_pthread_mutex);
			send_or_recv = 1;
		}
		usleep(4000);
	}
	fprintf(stderr,"recv_pthread exit\n");
	pthread_exit(NULL);
}

//创建send_pthread执行send_pthread
int send_pthread_init()
{
	pthread_attr_init(&ass_send_pthread.attr);
	pthread_attr_setdetachstate(&ass_send_pthread.attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&ass_send_pthread.tid,&ass_send_pthread.attr,send_pthread,NULL);
	pthread_attr_destroy(&ass_send_pthread.attr);

	fprintf(stderr,"send_pthread_init\n");

	return 0;
}

//向手机发送命令
void *send_pthread()
{
	//设定对于cancel请求的参数
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	//加锁成功后，等待信号量(send_info)
	while(end_flag == 0)
	{
		if(send_or_recv == 1)
		{
			pthread_mutex_lock(&ass_pthread_mutex);
			fprintf(stderr,"send_pthread lock\n");
			sem_wait(&ass_pthread_sem);
			pthread_mutex_lock(&send_flag_mutex);
			send_package();
			pthread_mutex_unlock(&send_flag_mutex);
			fprintf(stderr,"send_pthread unlock\n");
			pthread_mutex_unlock(&ass_pthread_mutex);
			send_or_recv = 0;
		}
		usleep(5000);
	}
	fprintf(stderr,"send_pthread exit\n");
	pthread_exit(NULL);
}

//执行接受操作
void recv_package()
{
	int i;
	int ret;
	int package_size = 0;
	int message_flag = 0;
	char recv_sig_buf = 0;

	//等待连续3次sig_start
	for(i=0; i<3; i++)
	{
		ret = recv_time_out(cli_serial_num,&recv_sig_buf,sizeof(char));
		if(ret == -1)
		{
			fprintf(stderr,"recv error\n");
			return ;
		}
		if(recv_sig_buf == sig_start)
		{
			recv_sig_buf = 0;
		}
		else
		{
			i = -1;
			recv_sig_buf = 0;
		}
	}

	//接收命令
	ret = recv_time_out(cli_serial_num,&recv_sig_buf,sizeof(char));
	if(ret == -1)
	{
		fprintf(stderr,"recv error\n");
		return ;
	}
	if(recv_sig_buf != CMD_RECV_MESSAGE)
	{
		fprintf(stderr,"recv %x\n",recv_sig_buf);
	}

	switch(recv_sig_buf)
	{
		case CMD_CONTACTS:
			fprintf(stderr,"wait recv contact\n");
			//接收通讯录
			recv_file("/tmp/ass/");
			break;
		case CMD_INSERT_CONTACT:
			break;
		case CMD_DELETE_CONTACT:
			break;
		case CMD_UPDATE_CONTACT:
			break;
		case CMD_MESSAGE:
			//接收短信的结构体至message_pack
			memset(&message_pack,'\0',sizeof(message_pack_t));
			fprintf(stderr,"wait recv message package_size\n");
			//接收package_size若超出结构体大小。则返回；否则接收
			ret = recv_time_out(cli_serial_num,(char *)&package_size,sizeof(int));
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			if(package_size > sizeof(message_pack_t))
			{
				fprintf(stderr,"package_size:%d\n",package_size);
				return ;
			}
			fprintf(stderr,"package_size: %d\n",package_size);
			ret = recv_time_out(cli_serial_num,(char *)&message_pack,package_size);
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			break;
		case CMD_SEND_MESSAGE:
			break;
		case CMD_APPINFO:
			fprintf(stderr,"wait recv appinfo\n");
			//接收应用信息appinfo.db3至指定路径
			recv_file("/tmp/ass/");		
			fprintf(stderr,"wait recv appicon\n");
			//接收应用图标
			recv_dir("/tmp/ass/icon/");
			break;
		case CMD_IMAGE:
			//接收图片信息至image_pack
			memset(&image_pack,'\0',sizeof(image_pack_t));
			fprintf(stderr,"wait recv image package_size\n");
			//接收package_size若超出结构体大小。则返回；否则接收
			ret = recv_time_out(cli_serial_num,(char *)&package_size,sizeof(int));
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			if(package_size > sizeof(image_pack_t))
			{
				fprintf(stderr,"error package_size:%d\n",package_size);
				return ;
			}
			fprintf(stderr,"package_size: %d\n",package_size);
			ret = recv_time_out(cli_serial_num,(char *)&image_pack,package_size);
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			//接收缩略图
			fprintf(stderr,"wait recv imageThumbnail\n");
			recv_dir("/tmp/ass/imageThumbnail/");
			break;
		case CMD_AUDIO:
			//接收音频信息至audio_pack
			memset(&audio_pack,'\0',sizeof(audio_pack_t));
			fprintf(stderr,"wait recv audio package_size\n");
			//接收package_size若超出结构体大小。则返回；否则接收
			ret = recv_time_out(cli_serial_num,(char *)&package_size,sizeof(int));
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			if(package_size > sizeof(audio_pack_t))
			{
				fprintf(stderr,"error package_size:%d\n",package_size);
				return ;
			}
			fprintf(stderr,"package_size: %d\n",package_size);
			ret = recv_time_out(cli_serial_num,(char *)&audio_pack,package_size);
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			break;
		case CMD_VIDEO:
			//接收视频信息至video_pack
			memset(&video_pack,'\0',sizeof(video_pack_t));
			fprintf(stderr,"wait recv video package_size\n");
			//接收package_size若超出结构体大小。则返回；否则接收
			ret = recv_time_out(cli_serial_num,(char *)&package_size,sizeof(int));
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			if(package_size > sizeof(video_pack_t))
			{
				fprintf(stderr,"error package_size:%d\n",package_size);
				return ;
			}
			fprintf(stderr,"package_size: %d\n",package_size);
			ret = recv_time_out(cli_serial_num,(char *)&video_pack,package_size);
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}
			fprintf(stderr,"%d\n",video_pack.count);
			//接收缩略图
			fprintf(stderr,"wait recv videoThumbnail\n");
			recv_dir("/tmp/ass/videoThumbnail/");
			break;
		case CMD_RECV_MESSAGE:
			//接收新短信标志
			ret = recv_time_out(cli_serial_num,(char *)&message_flag,sizeof(int));
			if(ret == -1)
			{
				fprintf(stderr,"recv error\n");
				return ;
			}

			//若未收到短信，则将信号清空
			if(message_flag == 0)
			{
				//若未收到短信，则清空标志
				recv_sig_buf = 0;
			}
			else
			{
				//若收到短信，则接收new_message结构体
				fprintf(stderr,"wait new message\n");
				memset(&new_message,'\0',sizeof(message_t));
				ret = recv_time_out(cli_serial_num,(char *)&new_message,sizeof(message_t));
				if(ret == -1)
				{
					fprintf(stderr,"recv error\n");
					return ;
				}
				fprintf(stderr,"new message:\n%s\n%s\n%s\n",new_message.message_name,new_message.message_date,new_message.message_body);
			}
			break;
		default:
			break;
	}
	//仅允许覆盖0x00与CMD_RECV_MESSAGE
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

//执行发送操作
void send_package()
{
	int i;
	//发送3次sig_start
	for(i=0;i<3;i++)
	{
		send(cli_sockfd,&sig_start,sizeof(char),MSG_NOSIGNAL);
	}
	//发送指令
	send(cli_sockfd,&send_cmd_flag,sizeof(char),MSG_NOSIGNAL);
	//根据发送标识，发送给android端
	switch(send_cmd_flag)
	{
		case CMD_CONTACTS:
			fprintf(stderr,"cmd_contact\n");
			break;
		case CMD_INSERT_CONTACT:
			fprintf(stderr,"cmd_insert_contact\n");
			//发送联系人增删改结构体
			send(cli_sockfd,&contact_cmd,sizeof(contact_cmd_t),MSG_NOSIGNAL);
			break;
		case CMD_DELETE_CONTACT:
			fprintf(stderr,"cmd_delete_contact\n");
			//发送联系人增删改结构体
			send(cli_sockfd,&contact_cmd,sizeof(contact_cmd_t),MSG_NOSIGNAL);
			break;
		case CMD_UPDATE_CONTACT:
			fprintf(stderr,"cmd_update_contact\n");
			//发送联系人增删改结构体
			send(cli_sockfd,&contact_cmd,sizeof(contact_cmd_t),MSG_NOSIGNAL);
			break;
		case CMD_MESSAGE:
			fprintf(stderr,"cmd_message\n");
			break;
		case CMD_SEND_MESSAGE:
			fprintf(stderr,"cmd_send_message\n");
			//发送短信结构体
			send(cli_sockfd,&message_cmd,sizeof(message_t),MSG_NOSIGNAL);
			break;
		case CMD_APPINFO:
			fprintf(stderr,"cmd_appinfo\n");
			break;
		case CMD_IMAGE:
			fprintf(stderr,"cmd_image\n");
			break;
		case CMD_AUDIO:
			fprintf(stderr,"cmd_audio\n");
			break;
		case CMD_VIDEO:
			fprintf(stderr,"cmd_video\n");
			break;
		case CMD_RECV_MESSAGE:
			//fprintf(stderr,"cmd_recv_message\n");
			break;
		default:
			break;
	}
	//允许recv_pthread上锁
	send_cmd_flag = 0;
}

/*******************************文件及目录接收**********************************/

//接收一个目录，没有则创建，有则清空
void recv_dir(char *dir_path)
{
	int file_count;
	int ret;
	DIR * dir;
	struct dirent *dir_info;
	char *file_path = NULL;
	//判断文件夹是否存在
	if((dir = opendir(dir_path)) != NULL)
	{
		//若存在则清空文件
		file_path = (char *)malloc(sizeof(char)*FILE_PATH_LEN);
		while((dir_info = readdir(dir)) != NULL)
		{
			if(strcmp(dir_info->d_name,".")!=0&&strcmp(dir_info->d_name,"..")!=0)
			{
				memset(file_path,'\0',sizeof(char)*FILE_PATH_LEN);
				strcpy(file_path,dir_path);
				strcat(file_path,dir_info->d_name);
				remove(file_path);
			}
		}
		free(file_path);
	}
	else
	{
		//若不存在则创建该文件夹
		if(mkdir(dir_path,0775) == -1)
		{
			fprintf(stderr,"mkdir %s failed\n",dir_path);
			return ;
		}
	}
	//接收文件个数
	ret = recv_time_out(cli_serial_num,(char *)&file_count,sizeof(int));
	if(ret == -1)
	{
		fprintf(stderr,"recv error\n");
		return ;
	}
	//根据文件个数循环调用recv_file
	for(;file_count >0;file_count--)
	{
		recv_file(dir_path);
	}
}

//接受文件并保存到path
void recv_file(char *path)
{
    int ret;
	char *recv_buf = NULL;
	char *file_name = NULL;
	char *file_path = NULL;
	int file_size = 0;
    FILE *fp;

	file_path = (char *)malloc(sizeof(char)*FILE_PATH_LEN);
	memset(file_path,'\0',sizeof(char)*FILE_PATH_LEN);
	strcpy(file_path,path);
	//接收文件名长度
	ret = recv_time_out(cli_serial_num,(char *)&file_size,sizeof(int));
	if(ret == -1)
	{
        fprintf(stderr,"recv error file_name_size\n");
		return ;
	}
	file_name = (char *)malloc(file_size+1);
	memset(file_name,'\0',file_size+1);
	//接收文件名
	ret = recv_time_out(cli_serial_num,file_name,file_size);
	if(ret == -1)
	{
        fprintf(stderr,"recv error file_name\n");
		return ;
	}
	//将文件名与dir_path连接，获得全路径
	strcat(file_path,file_name);
	free(file_name);
	//获取文件尺寸
	ret = recv_time_out(cli_serial_num,(char *)&file_size,sizeof(int));
	if(ret == -1)
	{
        fprintf(stderr,"recv error file_size\n");
		return ;
	}
	//从socket获取file_size字节，并写入file_path指向文件
	fp = fopen(file_path,"w");
    if( fp == NULL)
    {   
		fprintf(stderr,"fopen error\n");
        return ; 
    }   
    recv_buf = (char *)malloc(BUF_SIZE*sizeof(char));
    while(file_size > 0)
    {   
		memset(recv_buf,'\0',BUF_SIZE);
		if(file_size < BUF_SIZE)
		{
			ret = recv_time_out(cli_serial_num,recv_buf,file_size);
		}
		else
		{
			ret = recv_time_out(cli_serial_num,recv_buf,BUF_SIZE);
		}
        if(ret == -1) 
        {
            fprintf(stderr,"recv error file\n");
            return ; 
        }
		fwrite(recv_buf,sizeof(char),(size_t)ret,fp);
		file_size -= ret;
	}
	fclose(fp);
	free(recv_buf);
	free(file_path);
}

/****************************设置或读取指令*******************************/

//设定结束标志量,交由main_pthread结束程序(option：1断开连接，2关闭程序）
void set_end_flag(int option)
{
	end_flag = option;
	return;
}

//设置发送标志，并post信号量
int send_info(char flag)
{
	//CMD_RECV_MESSAGE只在0x00或CMD_RECV_MESSAGE可赋值给send_cmd_flag
	if(flag == CMD_RECV_MESSAGE && (send_cmd_flag != 0x00 && send_cmd_flag != CMD_RECV_MESSAGE))
	{
		return 2;
	}
	//若send_cmd_flag正被修改，则返回
	if(pthread_mutex_trylock(&send_flag_mutex) != 0)
	{
		return 1;
	}
	send_cmd_flag = flag;
	if(flag != CMD_RECV_MESSAGE)
	{
		fprintf(stderr,"%x %x\n",flag,send_cmd_flag);
	}
	//不允许重复post
	if((int)ass_pthread_sem.__align == 0 && send_cmd_flag != 0x00)
	{
		sem_post(&ass_pthread_sem);
	}
	else
	{
		fprintf(stderr,"sem_post failed %d\n",(int)ass_pthread_sem.__align);
	}
	pthread_mutex_unlock(&send_flag_mutex);

	return 0;
}

//检查是否接收到消息
int recv_info()
{
	char flag;
	if(pthread_mutex_trylock(&recv_flag_mutex) != 0)
	{
		return 0x00;
	}
	flag = recv_cmd_flag;
	recv_cmd_flag = 0;
	pthread_mutex_unlock(&recv_flag_mutex);
	return flag;
}

/****************************结构体拷贝*******************************/

//设置增删改联系人结构体
void set_contact_cmd(contact_cmd_t *tmp_contact)
{
	memcpy(&contact_cmd,tmp_contact,sizeof(contact_cmd_t));
}

//设置发送短信结构体
void set_message_cmd(message_t *tmp_message)
{
	memcpy(&message_cmd,tmp_message,sizeof(message_t));
}

//将存储短信息的结构体拷贝给所给的结构体指针
void get_message_pack(message_pack_t *ui_message_pack)
{
	memcpy(ui_message_pack,&message_pack,sizeof(int)+sizeof(message_t)*(message_pack.count));
}

//将存储短信息的结构体拷贝给所给的结构体指针
void get_message(message_t *ui_message)
{
	memcpy(ui_message,&new_message,sizeof(message_t));
}

//将存储图片的结构体拷贝给所给的结构体指针
void get_image(image_pack_t *ui_image_pack)
{
	memcpy(ui_image_pack,&image_pack,sizeof(int)+sizeof(image_t)*(image_pack.count));
}

//将存储音频的结构体拷贝给所给的结构体指针
void get_audio(audio_pack_t *ui_audio_pack)
{
	memcpy(ui_audio_pack,&audio_pack,sizeof(int)+sizeof(audio_t)*(audio_pack.count));
}

//将存储视频的结构体拷贝给所给的结构体指针
void get_video(video_pack_t *ui_video_pack)
{
	memcpy(ui_video_pack,&video_pack,sizeof(int)+sizeof(video_t)*(video_pack.count));
}
