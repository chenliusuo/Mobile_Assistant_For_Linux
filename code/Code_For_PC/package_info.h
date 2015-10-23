/*
 * package_info.h
 *
 *  Created on: 2015年1月3日
 *      Author: luke
 */

#ifndef PACKAGE_INFO_H_
#define PACKAGE_INFO_H_

//命令
#define CMD_CONTACTS			0x01	//获取通讯录
#define CMD_INSERT_CONTACT		0x02	//插入联系人
#define CMD_DELETE_CONTACT		0x03	//删除联系人
#define CMD_UPDATE_CONTACT		0x04	//修改联系人
#define CMD_MESSAGE				0x05	//获取短信
#define CMD_MAKE_CALL			0x06	//拨打电话
#define CMD_SEND_MESSAGE		0x07	//发送短信
#define CMD_APPINFO				0x08	//获取应用程序信息
#define CMD_IMAGE				0x09	//获取图片信息
#define CMD_AUDIO				0x0a	//获取音频信息
#define CMD_VIDEO				0x0b	//获取视频信息
#define CMD_RECV_MESSAGE		0x11	//检测是否短信新接收

//文件及结构体数量上限值
#define MAX_MESSAGE_ITEMS 2000		//短信条数上限
#define MAX_IMAGE_ITEMS 300			//图片数量上限
#define MAX_AUDIO_ITEMS 300			//音频数量上限
#define MAX_VIDEO_ITEMS 100			//视频数量上限
#define MAX_FILE_ITEMS	300			//send_dir中存储文件名的二维指针,应大于图片,视频或应用数量的最大值
//短信结构体上限值
#define PERSON_NAME_LEN 20			//人名长度上限,用于短信,通讯录
#define PHONE_NUM_LEN 20			//号码长度上限,用于短信,通讯录
#define MESSAGE_BODY_LEN 512		//短信内容长度
#define MESSAGE_DATE_LEN 25			//短信时间
//多媒体结构体上限值
#define MEDIA_TITLE_LEN 80			//图片,音频,视频文件名
#define MEDIA_PATH_LEN 120			//图片,音频,视频路径
#define MEDIA_ARTIST_LEN 100		//音频艺术家
//接收文件上限值
#define FILE_PATH_LEN 60			//文件路径长度
#define BUF_SIZE 4096				//接收文件时,定义缓冲区大小

//短信及短信包数据类型定义
typedef struct __message{
	char message_name[PERSON_NAME_LEN];
	char message_num[PHONE_NUM_LEN];
	char message_body[MESSAGE_BODY_LEN];
	char message_date[MESSAGE_DATE_LEN];
	int message_type;	//1接收 2发送
	int message_read;	//0未读 1已读
}message_t;

typedef struct __message_pack{
	int count;
	message_t message_item[MAX_MESSAGE_ITEMS];
}message_pack_t;

//图片信息及图片信息包类型定义
typedef struct __image{
	int img_size;
	char img_name[MEDIA_TITLE_LEN];
	char img_path[MEDIA_PATH_LEN];
}image_t;

typedef struct __image_pack{
	int count;
	image_t image_item[MAX_IMAGE_ITEMS];
}image_pack_t;

//音频信息及音频信息包类型定义
typedef struct __audio{
	int audio_size;
	int audio_duration;
	char audio_title[MEDIA_TITLE_LEN];
	char audio_artist[MEDIA_ARTIST_LEN];
	char audio_path[MEDIA_PATH_LEN];
}audio_t;

typedef struct __audio_pack{
	int count;
	audio_t audio_item[MAX_AUDIO_ITEMS];
}audio_pack_t;

//视频信息及视频信息包类型定义
typedef struct __video{
	int video_size;
	int video_duration;
	char video_title[MEDIA_TITLE_LEN];
	char video_path[MEDIA_PATH_LEN];
}video_t;

typedef struct __video_pack{
	int count;
	video_t video_item[MAX_VIDEO_ITEMS];
}video_pack_t;

//增删改联系人
//增加联系人,设置name,num
//删除联系人,设置id(或name)
//删除指定号码,设置id(或name),num
//修改联系人,全部设置
typedef struct __contact_cmd{
	int id;
	char name[PERSON_NAME_LEN];
	char num[PHONE_NUM_LEN];
	char new_name[PERSON_NAME_LEN];
	char new_num[PHONE_NUM_LEN];
}contact_cmd_t;

int recv_info();
int send_info(char);
void set_end_flag(int);
void set_contact_cmd(contact_cmd_t *);
void set_message_cmd(message_t *);
void get_message(message_t *);
void get_message_pack(message_pack_t *);
void get_image(image_pack_t *);
void get_audio(audio_pack_t *);
void get_video(video_pack_t *);
int main_pthread_init();
#endif /* PACKAGE_INFO_H_ */
