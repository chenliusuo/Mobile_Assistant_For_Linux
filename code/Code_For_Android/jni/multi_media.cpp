/*
 * multi_media.cpp
 *
 *  Created on: 2015年1月12日
 *      Author: luke
 */
#include "com_android_ass_AssService.h"
#include "package_info.h"
#include "mylog.h"
#include <string.h>

image_pack_t image_pack;
audio_pack_t audio_pack;
video_pack_t video_pack;

JNIEXPORT jint JNICALL Java_com_android_ass_AssService_image_1pack_1create
  (JNIEnv * env, jclass, jint count, jint img_size, jstring img_name, jstring img_path)
{
	const char *image_name = env->GetStringUTFChars(img_name, 0);
	const char *image_path = env->GetStringUTFChars(img_path, 0);
	//插入第一张图片前，清空图片存储的结构体
	if(count == 1)
	{
		memset(&image_pack,'\0',sizeof(image_pack_t));
	}
	//若超出范围则不添加
	if(count >= MAX_IMAGE_ITEMS || !image_name || !image_path)
	{
		LOGD("image is too much");
		return 1;
	}
	else if((strlen(image_name)> MEDIA_TITLE_LEN) || (strlen(image_path) > MEDIA_PATH_LEN))
	{
		return 2;
	}

	image_pack.count = count;
	image_pack.image_item[count-1].img_size = img_size;
	memcpy(image_pack.image_item[count-1].img_name,image_name,strlen(image_name));
	memcpy(image_pack.image_item[count-1].img_path,image_path,strlen(image_path));
	return 0;
}

JNIEXPORT jint JNICALL Java_com_android_ass_AssService_audio_1pack_1create
  (JNIEnv * env, jclass, jint count, jint size, jint duration, jstring title, jstring artist, jstring path)
{
	const char *audio_title = env->GetStringUTFChars(title, 0);
	const char *audio_artist = env->GetStringUTFChars(artist, 0);
	const char *audio_path = env->GetStringUTFChars(path, 0);
	//插入第一张音频前，清空音频存储的结构体
	if(count == 1)
	{
		memset(&audio_pack,'\0',sizeof(audio_pack_t));
	}
	//若超出范围则不添加
	if(count > MAX_AUDIO_ITEMS || !audio_title || !audio_path || !audio_artist)
	{
		LOGD("audio is too much");
		return 1;
	}
	else if((strlen(audio_title)> MEDIA_TITLE_LEN) || (strlen(audio_path) > MEDIA_PATH_LEN) || (strlen(audio_artist) > MEDIA_ARTIST_LEN) )
	{
		LOGD("audio is too long");
		return 2;
	}
	audio_pack.count = count;
	audio_pack.audio_item[count-1].audio_size = size;
	audio_pack.audio_item[count-1].audio_duration = duration;
	memcpy(audio_pack.audio_item[count-1].audio_title,audio_title,strlen(audio_title));
	memcpy(audio_pack.audio_item[count-1].audio_artist,audio_artist,strlen(audio_artist));
	memcpy(audio_pack.audio_item[count-1].audio_path,audio_path,strlen(audio_path));
//	LOGD("%d: %s %s %s",count,audio_pack.audio_item[count-1].audio_title,audio_pack.audio_item[count-1].audio_artist,audio_pack.audio_item[count-1].audio_path);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_android_ass_AssService_video_1pack_1create
(JNIEnv * env, jclass, jint count, jint size, jint duration, jstring title, jstring path)
{
	const char *video_title = env->GetStringUTFChars(title, 0);
	const char *video_path = env->GetStringUTFChars(path, 0);
	//插入第一张视频前，清空视频存储的结构体
	if(count == 1)
	{
		memset(&video_pack,'\0',sizeof(video_pack_t));
	}
	//若超出范围则不添加
	if(count > MAX_VIDEO_ITEMS || !video_title || !video_path)
	{
		LOGD("video is too much");
		return 1;
	}
	else if((strlen(video_title)> MEDIA_TITLE_LEN) || (strlen(video_path) > MEDIA_PATH_LEN))
	{
		return 2;
	}
	video_pack.count = count;
	video_pack.video_item[count-1].video_size = size;
	video_pack.video_item[count-1].video_duration = duration;
	memcpy(video_pack.video_item[count-1].video_title,video_title,strlen(video_title));
	memcpy(video_pack.video_item[count-1].video_path,video_path,strlen(video_path));
	return 0;
}

