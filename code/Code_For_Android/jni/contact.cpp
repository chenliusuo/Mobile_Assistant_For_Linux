/*
 * contact.cpp
 *
 *  Created on: 2015年1月7日
 *      Author: luke
 */
#include <string.h>
#include <stdio.h>
#include "package_info.h"
#include "mylog.h"
#include "com_android_ass_AssService.h"

contact_cmd_t contact_cmd;

//增删改联系人获取数据
JNIEXPORT jstring JNICALL Java_com_android_ass_AssService_get_1contact_1cmd
  (JNIEnv * env, jclass, jint item)
{
	char id_string[5];
	switch(item)
	{
		case 0:
			sprintf(id_string,"%d",contact_cmd.id);
			return env->NewStringUTF(id_string);				//返回联系人id
		case 1:
			if(strlen(contact_cmd.name) == 0)
			{
				return env->NewStringUTF(NULL);
			}
			else
			{
				return env->NewStringUTF(contact_cmd.name);		//返回联系人姓名
			}
		case 2:
			if(strlen(contact_cmd.num) == 0)
			{
				return env->NewStringUTF(NULL);
			}
			else
			{
				return env->NewStringUTF(contact_cmd.num);		//返回联系人号码
			}
		case 3:
			if(strlen(contact_cmd.new_name) == 0)
			{
				return env->NewStringUTF(NULL);
			}
			else
			{
				return env->NewStringUTF(contact_cmd.new_name);	//返回联系人修改后姓名
			}
		case 4:
			if(strlen(contact_cmd.new_num) == 0)
			{
				return env->NewStringUTF(NULL);
			}
			else
			{
				return env->NewStringUTF(contact_cmd.new_num);	//返回联系人修改后号码
			}
	}
	return env->NewStringUTF(NULL);
}
