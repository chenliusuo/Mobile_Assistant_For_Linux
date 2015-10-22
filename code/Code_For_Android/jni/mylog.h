/*
 * mylog.h
 *
 *  Created on: 2015年1月2日
 *      Author: luke
 */

#ifndef MYLOG_H_
#define MYLOG_H_

#include <android/log.h>
#define TAG "ndk"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


#endif /* MYLOG_H_ */
