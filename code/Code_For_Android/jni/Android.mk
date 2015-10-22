LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Ass
LOCAL_SRC_FILES := Ass.cpp \
server.cpp \
message.cpp \
contact.cpp	\
multi_media.cpp

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
