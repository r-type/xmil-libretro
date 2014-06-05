LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := main

SDL_PATH := ../SDL
XMIL_PATH := $(LOCAL_PATH)/../xmil

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
	$(LOCAL_PATH) \
	$(XMIL_PATH) \
	$(XMIL_PATH)/codecnv \
	$(XMIL_PATH)/common \
	$(XMIL_PATH)/embed \
	$(XMIL_PATH)/embed/menu \
	$(XMIL_PATH)/embed/menubase \
	$(XMIL_PATH)/fdd \
	$(XMIL_PATH)/font \
	$(XMIL_PATH)/io \
	$(XMIL_PATH)/sdl2 \
	$(XMIL_PATH)/sound \
	$(XMIL_PATH)/vram \
	$(XMIL_PATH)/z80c

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	main.c \
	$(subst $(LOCAL_PATH)/,, \
		$(wildcard $(XMIL_PATH)/*.c) \
		$(wildcard $(XMIL_PATH)/codecnv/*.c) \
		$(wildcard $(XMIL_PATH)/common/*.c) \
		$(wildcard $(XMIL_PATH)/embed/*.c) \
		$(wildcard $(XMIL_PATH)/embed/menu/*.c) \
		$(wildcard $(XMIL_PATH)/embed/menubase/*.c) \
		$(wildcard $(XMIL_PATH)/fdd/*.c) \
		$(wildcard $(XMIL_PATH)/font/*.c) \
		$(wildcard $(XMIL_PATH)/io/*.c) \
		$(wildcard $(XMIL_PATH)/sdl2/*.c) \
		$(wildcard $(XMIL_PATH)/sound/*.c) \
		$(wildcard $(XMIL_PATH)/vram/*.c) \
		$(wildcard $(XMIL_PATH)/z80c/*.c) \
	)

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
