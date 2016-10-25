# makefile for arm-linux
#   gcc version 4

CC		=	gcc
STRIP	=	strip
RM		=	rm -f

XMIL_PATH	= ../..

CFLAGS	=	-Wall -O3 -Wstrict-overflow=0 -I. -I.. \
			-I$(XMIL_PATH) \
			-I$(XMIL_PATH)/common \
			-I$(XMIL_PATH)/embed \
			-I$(XMIL_PATH)/embed/menu \
			-I$(XMIL_PATH)/embed/menubase \
			-I$(XMIL_PATH)/fdd \
			-I$(XMIL_PATH)/font \
			-I$(XMIL_PATH)/io \
			-I$(XMIL_PATH)/sound \
			-I$(XMIL_PATH)/vram \
			-I$(XMIL_PATH)/z80c

CFLAGS	+=	$(shell sdl2-config --cflags)
LFLAGS	=

TARGET	=	xmil

CPPSRCS	=	./main.c \
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
			$(wildcard $(XMIL_PATH)/sdl2/*.cpp) \
			$(wildcard $(XMIL_PATH)/sound/*.c) \
			$(wildcard $(XMIL_PATH)/vram/*.c) \
			$(wildcard $(XMIL_PATH)/z80c/*.c)

OBJS = $(addsuffix .o,$(basename $(CPPSRCS)))
LIBS = -lm $(shell sdl2-config --libs)

.SUFFIXES: .c.o
.SUFFIXES: .cpp.o

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(LFLAGS) -g -o $@ $(OBJS) $(LIBS)
	$(STRIP) $@

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
