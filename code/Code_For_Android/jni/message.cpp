/*
 * message.cpp
 *
 *  Created on: 2015年1月3日
 *      Author: luke
 */
#include "com_android_ass_AssService.h"
#include "package_info.h"
#include "mylog.h"
#include <string.h>

message_pack_t message_pack;
message_t message_cmd;
message_t new_message;
int new_message_flag = 0;

//将短信写入结构体
JNIEXPORT jint JNICALL Java_com_android_ass_AssService_message_1pack_1create
(JNIEnv * env, jclass, jstring message_name, jstring message_num, jstring message_body, jstring message_date, jint message_type, jint message_read, jint count)

{
	const char *msg_name= env->GetStringUTFChars(message_name, 0);
	const char *msg_num= env->GetStringUTFChars(message_num, 0);
	const char *msg_body= env->GetStringUTFChars(message_body, 0);
	const char *msg_date= env->GetStringUTFChars(message_date, 0);
	//插入第一条短信前，清空短信存储的结构体
	if(count == 1)
	{
		memset(&message_pack,'\0',sizeof(message_pack_t));
	}
	//若超出范围则不添加
	if(count >= MAX_MESSAGE_ITEMS ||  !msg_name || !msg_num || !msg_date)
	{
		LOGD("message is too much");
		return 1;
	}
	else if((strlen(msg_num)> PHONE_NUM_LEN) || (strlen(msg_body) > MESSAGE_BODY_LEN) || (strlen(msg_date) > MESSAGE_DATE_LEN) || strlen(msg_name)> PERSON_NAME_LEN)
	{
		return 2;
	}

	message_pack.count = count;
	message_pack.message_item[count-1].message_type = message_type;
	message_pack.message_item[count-1].message_read = message_read;
	memcpy(message_pack.message_item[count-1].message_name,msg_name,strlen(msg_name));
	memcpy(message_pack.message_item[count-1].message_num,msg_num,strlen(msg_num));
	memcpy(message_pack.message_item[count-1].message_body,msg_body,strlen(msg_body));
	memcpy(message_pack.message_item[count-1].message_date,msg_date,strlen(msg_date));
	return 0;
//	LOGD("%s %s %s",message_pack.message_item[count-1].message_num,message_pack.message_item[count-1].message_body,message_pack.message_item[count-1].message_date,msg_date);
}

//将新收到短信放入结构体
JNIEXPORT void JNICALL Java_com_android_ass_AssService_new_1message_1create
(JNIEnv * env, jclass, jstring message_name, jstring message_num, jstring message_body, jstring message_date)
{
	const char *msg_name= env->GetStringUTFChars(message_name, 0);
	const char *msg_num= env->GetStringUTFChars(message_num, 0);
	const char *msg_body= env->GetStringUTFChars(message_body, 0);
	const char *msg_date= env->GetStringUTFChars(message_date, 0);
	//插入短信前，清空短信存储的结构体
	memset(&new_message,'\0',sizeof(message_t));

	if(!msg_name || !msg_num || !msg_date)
	{
		return;
	}
	//若超出范围则不添加
	else if((strlen(msg_num)> PHONE_NUM_LEN) || (strlen(msg_body) > MESSAGE_BODY_LEN) || (strlen(msg_date) > MESSAGE_DATE_LEN) || strlen(msg_name)> PERSON_NAME_LEN)
	{
		return ;
	}

	memcpy(new_message.message_name,msg_name,strlen(msg_name));
	memcpy(new_message.message_num,msg_num,strlen(msg_num));
	memcpy(new_message.message_body,msg_body,strlen(msg_body));
	memcpy(new_message.message_date,msg_date,strlen(msg_date));

	new_message_flag = 1;
}

//获取发送短信所需数据
JNIEXPORT jstring JNICALL Java_com_android_ass_AssService_get_1message_1cmd
  (JNIEnv * env, jclass, jint item)
{
	switch(item)
		{
		case 0:
			return env->NewStringUTF(message_cmd.message_num);	//返回收信人姓名
		case 1:
			return env->NewStringUTF(message_cmd.message_body);//返回短信内容
		}
		return env->NewStringUTF(NULL);
}


