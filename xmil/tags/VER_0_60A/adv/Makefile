
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/base_rules

ADVLIBC		= ./agb/advlibc

LD			= $(PREFIX)ld

RM		= rm -f

OBJBASE	= ../obj
BINBASE	= ../bin

OBJ		= $(OBJBASE)/agb
BIN		= $(BINBASE)/agb

BASE		= ..
COMMON		= ../common
Z80A		= ../z80aadv
IO			= ../io
VRAM		= ./vram
SOUND		= ../sound
FDD			= ./fdd
TARGET		= .
TARGET2		= ./agb
PATCH		= ./patch

HEADERS		=	$(TARGET2)/compiler.h $(BASE)/common.h $(TARGET2)/advsys.h \
				$(COMMON)/milstr.h $(COMMON)/_memory.h $(TARGET)/trace.h \
				$(COMMON)/rect.h

Z80CORES	=	$(Z80A)/z80core.h $(Z80A)/z80mem.h $(Z80A)/z80dmap.h

PCCORES		=	$(BASE)/pccore.h

IOCORES		=	$(IO)/iocore.h $(IO)/cgrom.h $(IO)/cmt.h $(IO)/crtc.h \
				$(IO)/ctc.h $(IO)/dipsw.h $(IO)/dmac.h $(IO)/fdc.h \
				$(IO)/memio.h $(IO)/pcg.h $(IO)/ppi.h $(IO)/sio.h \
				$(IO)/sndboard.h $(IO)/subcpu.h $(IO)/vramio.h

AOPT	=	
COPT	=	-c -nostdlib -Wall -O3 -fomit-frame-pointer -fsigned-char \
			-I$(ADVLIBC)/include -I$(TARGET) \
			-I$(TARGET2) -I$(BASE) -I$(COMMON) -I$(Z80A) -I$(IO) -I$(PATCH) \
			-I$(VRAM) -I$(SOUND) -I$(FDD) -I$(PATCH)
LOPT	=	-X

ADVOBJ	=	$(OBJ)/parts.o												\
			$(OBJ)/z80a.o		$(OBJ)/z80a_mn.o	$(OBJ)/z80a_cb.o	\
			$(OBJ)/z80a_s.o		$(OBJ)/z80a_io.o	$(OBJ)/z80a_i.o		\
			$(OBJ)/z80mem.o		$(OBJ)/z80dmap2.o						\
			$(OBJ)/iocore.o		$(OBJ)/iocored.o						\
			$(OBJ)/cgrom.o		$(OBJ)/cmt.o							\
			$(OBJ)/crtc.o		$(OBJ)/ctc.o		$(OBJ)/dipsw.o		\
			$(OBJ)/dmac.o		$(OBJ)/fdc.o		$(OBJ)/memio.o		\
			$(OBJ)/pcg.o		$(OBJ)/ppi.o		$(OBJ)/ppi2.o		\
			$(OBJ)/sio.o		$(OBJ)/sndboard.o	$(OBJ)/subcpu.o		\
			$(OBJ)/subcpu2.o	$(OBJ)/vramio.o		$(OBJ)/vramio2.o	\
			$(OBJ)/palettes.o	$(OBJ)/makescrn.o	$(OBJ)/makesub2.o	\
			$(OBJ)/makeatr.o	$(OBJ)/makemix2.o	$(OBJ)/make15.o		\
			$(OBJ)/makec8a.o	$(OBJ)/makec16.o	$(OBJ)/makecsa.o	\
			$(OBJ)/vsyncff.o											\
			$(OBJ)/fddfile.o	$(OBJ)/fdd_2d.o		$(OBJ)/fdd_d88.o	\
			$(OBJ)/xmil.o		$(OBJ)/joymng.o		$(OBJ)/mousemng.o	\
			$(OBJ)/sysmng.o		$(OBJ)/timemng.o	$(OBJ)/advpsg.o		\
			$(OBJ)/xmilopen.o											\
			$(OBJ)/pccore.o		$(OBJ)/nevent.o		$(OBJ)/ievent.o		\
			$(OBJ)/keystat.o	$(OBJ)/calendar.o	$(OBJ)/timing.o		\
			$(OBJ)/ctc2.o		$(OBJ)/make15a.o


ADVELF	=	$(OBJ)/xmiladv.elf
ADVGBA	=	$(BIN)/xmiladv.hdr


all:		$(OBJ) $(BIN) $(ADVGBA)


$(OBJ):
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)


$(ADVGBA):	$(ADVELF)
	$(ADVLIBC)/lib/makegba.exe $(ADVELF) $(ADVGBA)

$(ADVELF):	$(ADVOBJ) $(ADVLIBC)/lib/advsys.ls
	$(LD) $(LOPT) -o $(ADVELF) $(ADVLIBC)/lib/advcrt0.o $(ADVOBJ) -L$(ADVLIBC)/lib -ladvlibc -T $(ADVLIBC)/lib/advsys.ls

$(OBJ)/parts.o:		$(COMMON)/parts.c $(HEADERS) $(COMMON)/parts.h
	$(CC) $(COPT) -c $< -o $@


$(OBJ)/z80a.o:		$(Z80A)/z80a.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/z80a_mn.o:	$(Z80A)/z80a_mn.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80a_cb.o:	$(Z80A)/z80a_cb.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80a_s.o:	$(Z80A)/z80a_s.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80a_io.o:	$(Z80A)/z80a_io.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80a_i.o:	$(Z80A)/z80a_i.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80mem.o:	$(Z80A)/z80mem.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/z80dmap.o:	$(Z80A)/z80dmap.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/ievent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/z80dmap2.o:	$(Z80A)/z80dmap2.s
	$(AS) $(AOPT) $< -o $@


$(OBJ)/iocore.o:	$(PATCH)/iocore.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/iocored.o:	$(PATCH)/iocored.s
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/cgrom.o:		$(IO)/cgrom.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(PATCH)/font.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/cmt.o:		$(IO)/cmt.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/crtc.o:		$(IO)/crtc.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(VRAM)/vram.h \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/ctc.o:		$(PATCH)/ctc.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/ctc2.o:		$(PATCH)/ctc2.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/dipsw.o:		$(IO)/dipsw.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/dmac.o:		$(IO)/dmac.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/fdc.o:		$(IO)/fdc.c $(HEADERS) $(Z80CORES) $(PCCORES) $(IOCORES) \
					$(BASE)/nevent.h $(FDD)/fddfile.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/memio.o:		$(PATCH)/memio.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(IO)/defipl.res
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/pcg.o:		$(IO)/pcg.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(VRAM)/vram.h \
					$(VRAM)/makescrn.h $(PATCH)/font.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/ppi.o:		$(PATCH)/ppi.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(BASE)/nevent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/ppi2.o:		$(PATCH)/ppi2.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/sio.o:		$(IO)/sio.c $(HEADERS) $(Z80CORES) $(PCCORES) $(IOCORES) \
					$(TARGET)/mousemng.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/sndboard.o:	$(PATCH)/sndboard.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/subcpu.o:	$(PATCH)/subcpu.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(TARGET)/timemng.h $(BASE)/nevent.h \
					$(BASE)/ievent.h $(BASE)/keystat.h $(BASE)/calendar.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/subcpu2.o:	$(PATCH)/subcpu2.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/vramio.o:	$(PATCH)/vramio.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/vramio2.o:	$(PATCH)/vramio2.s
	$(AS) $(AOPT) $< -o $@


$(OBJ)/palettes.o:	$(VRAM)/palettes.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makescrn.o:	$(VRAM)/makescrn.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/palettes.h $(VRAM)/makescrn.h \
					$(VRAM)/makesub.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makesub.o:	$(VRAM)/makesub.c $(HEADERS) $(VRAM)/makesub.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makesub2.o:	$(VRAM)/makesub2.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/makeatr.o:	$(VRAM)/makeatr.c $(HEADERS) $(VRAM)/vram.h \
					$(VRAM)/makesub.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makechr.o:	$(VRAM)/makechr.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h $(VRAM)/makesub.h \
					$(PATCH)/font.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makemix.o:	$(VRAM)/makemix.c $(HEADERS) $(VRAM)/vram.h \
					$(VRAM)/makesub.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makemix2.o:	$(VRAM)/makemix2.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/make15.o:	$(VRAM)/make15.c $(HEADERS) $(PCCORES) $(VRAM)/vram.h \
					$(VRAM)/makescrn.h $(VRAM)/makesub.h $(PATCH)/font.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/make15a.o:	$(VRAM)/make15a.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/makec8.o:	$(VRAM)/makec8.c
	$(CC) $(COPT) -o $*.o $(VRAM)/$(*B).c

$(OBJ)/makec8a.o:	$(VRAM)/makec8a.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/makec16.o:	$(VRAM)/makec16.c
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makecs.o:	$(VRAM)/makecs.c
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/makecsa.o:	$(VRAM)/makecsa.s
	$(AS) $(AOPT) $< -o $@

$(OBJ)/maketxtl.o:	$(VRAM)/maketxtl.c $(HEADERS) $(VRAM)/makescrn.h \
					$(VRAM)/makesub.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/vsyncff.o:	$(VRAM)/vsyncff.s
	$(AS) $(AOPT) $< -o $@


$(OBJ)/fddfile.o:	$(FDD)/fddfile.c $(HEADERS) $(PCCORES) $(IOOCORES) \
					$(FDD)/fddfile.h $(FDD)/fdd_2d.h $(FDD)/fdd_d88.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/fdd_2d.o:	$(FDD)/fdd_2d.c $(HEADERS) $(PCCORES) $(FDD)/fddfile.h \
					$(FDD)/fdd_2d.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/fdd_d88.o:	$(FDD)/fdd_d88.c $(HEADERS) $(PCCORES) $(FDD)/fddfile.h \
					$(FDD)/fdd_d88.h
	$(CC) $(COPT) -c $< -o $@


$(OBJ)/xmil.o:		$(TARGET)/xmil.c $(HEADERS)
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/joymng.o:	$(TARGET)/joymng.c $(HEADERS) $(TARGET)/mousemng.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/mousemng.o:	$(TARGET)/mousemng.c $(HEADERS) $(TARGET)/mousemng.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/sysmng.o:	$(TARGET)/sysmng.c $(HEADERS) $(TARGET)/sysmng.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/timemng.o:	$(TARGET)/timemng.c $(HEADERS) $(TARGET)/timemng.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/advpsg.o:	$(TARGET)/advpsg.c $(HEADERS) $(TARGET)/advpsg.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/xmilopen.o:	$(TARGET)/xmilopen.c $(HEADERS) $(TARGET)/xmilopen.h
	$(CC) $(COPT) -c $< -o $@


$(OBJ)/pccore.o:	$(BASE)/pccore.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(TARGET)/sysmng.h $(TARGET)/timemng.h \
					$(BASE)/xmilver.h $(BASE)/nevent.h $(BASE)/ievent.h \
					$(BASE)/timing.h $(BASE)/calendar.h $(BASE)/keystat.h \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h $(SOUND)/sound.h \
					$(SOUND)/sndctrl.h $(PATCH)/font.h $(FDD)/fddfile.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/nevent.o:	$(BASE)/nevent.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(BASE)/nevent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/ievent.o:	$(BASE)/ievent.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/keystat.o:	$(BASE)/keystat.c $(HEADERS) $(PCCORES) \
					$(BASE)/keystat.h $(BASE)/keystat.tbl
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/calendar.o:	$(BASE)/calendar.c $(HEADERS) $(PCCORES) \
					$(COMMON)/parts.h $(TARGET)/timemng.h $(BASE)/nevent.h \
					$(BASE)/calendar.h
	$(CC) $(COPT) -c $< -o $@

$(OBJ)/timing.o:	$(BASE)/timing.c $(HEADERS) $(PCCORES) \
					$(BASE)/timing.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT) -c $< -o $@



clean:
	$(RM) $(ADVOBJ)
	$(RM) $(ADVELF)
	$(RM) $(ADVGBA)

