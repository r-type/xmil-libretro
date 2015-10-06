
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

LD			=	$(CXX)
RM			=	rm -f

LIBDIRS		=	$(LIBNDS)

INCLUDE		=	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
				$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
				-I$(CURDIR)/$(BUILD)
LIBPATHS	=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

BASE		=	..
COMMON		=	../common
Z80A		=	../z80ands
Z80C		=	../z80ac
IO			=	../io
VRAM		=	./vram
SOUND		=	./sound
FDD			=	./fdd
TARGET		=	.
TARGET2		=	./nds
PATCH		=	./patch

HEADERS		=	$(TARGET2)/compiler.h $(BASE)/common.h \
				$(COMMON)/milstr.h $(COMMON)/_memory.h $(TARGET2)/trace.h \
				$(COMMON)/rect.h

Z80CORES	=	$(Z80A)/z80core.h $(Z80A)/z80mem.h $(Z80A)/z80dmap.h

PCCORES		=	$(BASE)/pccore.h

IOCORES		=	$(IO)/iocore.h $(IO)/cgrom.h $(IO)/cmt.h $(IO)/crtc.h \
				$(IO)/ctc.h $(IO)/dipsw.h $(IO)/dmac.h $(IO)/fdc.h \
				$(IO)/memio.h $(IO)/pcg.h $(IO)/ppi.h $(IO)/sio.h \
				$(IO)/sndboard.h $(IO)/subcpu.h $(IO)/vramio.h

ARCH	=	-mthumb -mthumb-interwork
#ARCH	=	-mthumb-interwork


AOPT9	+=	-g $(ARCH) --defsym INTERWORK=1
COPT9	+=	-g -Wall -O2 -march=armv5te -mtune=arm946e-s \
			-fomit-frame-pointer -fsigned-char -ffast-math $(ARCH) -DARM9 \
			-I$(TARGET) -I$(TARGET2) -I$(PATCH) -I$(VRAM) -I$(FDD) -I$(BASE) \
			-I$(COMMON) -I$(Z80A) -I$(IO) -I$(SOUND) $(INCLUDE)
LOPT9	=	-specs=./nds/xmilnds9.specs -g $(ARCH) -mno-fpu -Wl,-Map,$(basename $@).map
LIBS9	=	-lnds9

Z80COBJ	=	$(OBJ)/z80c.o		$(OBJ)/z80c_mn.o	$(OBJ)/z80c_sb.o	\
			$(OBJ)/z80c_cb.o	$(OBJ)/z80c_ix.o	$(OBJ)/z80c_iy.o	\
			$(OBJ)/z80mem.o		$(OBJ)/z80dmap.o

Z80AOBJ	=	$(OBJ)/z80a.o		$(OBJ)/z80a_mn.oa	$(OBJ)/z80a_s.oa	\
			$(OBJ)/z80a_cb.oa	$(OBJ)/z80a_i.oa	$(OBJ)/z80a_io.oa	\
			$(OBJ)/z80mem.oa	$(OBJ)/z80dmap2.oa

NDS9OBJ	=	$(OBJ)/parts.o												\
			$(Z80AOBJ)													\
			$(OBJ)/iocore.o		$(OBJ)/iocored.oa						\
			$(OBJ)/cgrom.o		$(OBJ)/cmt.o		$(OBJ)/crtc.o		\
			$(OBJ)/ctc.o		$(OBJ)/ctc2.oa		$(OBJ)/dipsw.o		\
			$(OBJ)/dmac.o		$(OBJ)/fdc.o		$(OBJ)/fdc2.oa		\
			$(OBJ)/memio.o		\
			$(OBJ)/pcg.o		$(OBJ)/ppi.o		$(OBJ)/ppi2.oa		\
			$(OBJ)/sio.o		$(OBJ)/sndboard.o	$(OBJ)/sndboard.oa	\
			$(OBJ)/subcpu.o		$(OBJ)/subcpu2.oa						\
			$(OBJ)/vramio.o		$(OBJ)/vramio2.oa						\
			$(OBJ)/palettes.o	$(OBJ)/makescrn.o	$(OBJ)/makesub2.oa	\
			$(OBJ)/makemix2.oa	$(OBJ)/make15.o		$(OBJ)/make15a.oa	\
			$(OBJ)/makec8a.oa	$(OBJ)/makec16.o	$(OBJ)/makecsa.oa	\
			$(OBJ)/vsyncff.o											\
			$(OBJ)/nds9psg.o											\
			$(OBJ)/fddfile.o	$(OBJ)/fdd_2d.o		$(OBJ)/fdd_d88.o	\
			$(OBJ)/xmil9.o		$(OBJ)/joymng.o		$(OBJ)/mousemng.o	\
			$(OBJ)/sysmng.o		$(OBJ)/timemng.o	$(OBJ)/bss.oa		\
			$(OBJ)/libnds9.o	$(OBJ)/ndscore.o	$(OBJ)/trace.o		\
			$(OBJ)/extrom.o		$(OBJ)/resource.o	$(OBJ)/softkbd9.o	\
			$(OBJ)/pccore.o		$(OBJ)/nevent.o		$(OBJ)/ievent.o		\
			$(OBJ)/keystat.o	$(OBJ)/calendar.o	$(OBJ)/timing.o
NDS9ELF	=	$(OBJ)/$(PROJECT)9.elf
NDS9ARM	=	$(OBJ)/$(PROJECT)9.arm


AOPT7	+=	-g $(ARCH) --defsym INTERWORK=1
COPT7	+=	-g -Wall -O2 -mcpu=arm7tdmi -mtune=arm7tdmi \
			-fomit-frame-pointer -fsigned-char -ffast-math $(ARCH) -DARM7 \
			-I$(TARGET) -I$(TARGET2) -I$(PATCH) -I$(VRAM) -I$(FDD) -I$(BASE) \
			-I$(COMMON) -I$(Z80A) -I$(IO) -I$(SOUND) $(INCLUDE)
LOPT7	=	--specs=ds_arm7.specs -g $(ARCH) -mno-fpu -Wl,-Map,$(basename $@).map
LIBS7	=	-lnds7

NDS7OBJ	=	$(OBJ)/xmil7.o												\
			$(OBJ)/nds7psg.o	$(OBJ)/softkbd7.o						\
			$(OBJ)/libnds7.o
NDS7ELF	=	$(OBJ)/$(PROJECT)7.elf
NDS7ARM	=	$(OBJ)/$(PROJECT)7.arm

NDSBIN	=	$(OBJ)/$(PROJECT).bin
#NDSROM	=	$(BIN)/$(PROJECT).nds


all:		$(OBJBASE) $(BINBASE) $(OBJ) $(BIN) $(NDSROM)



$(OBJBASE):
	@mkdir $(OBJBASE)

$(BINBASE):
	@mkdir $(BINBASE)



$(OBJ):
	@mkdir $(OBJ)

$(BIN):
	@mkdir $(BIN)


$(BIN)/$(PROJECT).nds:	$(NDSBIN)
	@$(BIN)/StoreRom $< $@ -D ./romimage


$(NDSBIN):	$(NDS9ARM) $(NDS7ARM)
	@ndstool -c $@ -9 $(NDS9ARM) -7 $(NDS7ARM)
	@echo built ... $(notdir $@)


$(NDS9ARM):	$(NDS9ELF)
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)

$(NDS9ELF):	$(NDS9OBJ)
	$(LD) $(LOPT9) $(NDS9OBJ) $(LIBPATHS) $(LIBS9) -o $@


$(NDS7ARM):	$(NDS7ELF)
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)

$(NDS7ELF):	$(NDS7OBJ)
	$(LD) $(LOPT7) $(NDS7OBJ) $(LIBPATHS) $(LIBS7) -o $@



$(OBJ)/parts.o:		$(COMMON)/parts.c $(HEADERS) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@



$(OBJ)/z80c.o:		$(Z80C)/z80c.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80c_mn.o:	$(Z80C)/z80c_mn.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80c_sb.o:	$(Z80C)/z80c_sb.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80c_cb.o:	$(Z80C)/z80c_cb.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80c_ix.o:	$(Z80C)/z80c_ix.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80c_iy.o:	$(Z80C)/z80c_iy.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80mem.o:	$(Z80C)/z80mem.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80dmap.o:	$(Z80C)/z80dmap.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@



$(OBJ)/z80a.o:		$(Z80A)/z80a.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(COMMON)/parts.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/z80a_mn.oa:	$(Z80A)/z80a_mn.s $(TARGET2)/ramptr.inc \
					$(Z80A)/z80a.inc $(Z80A)/z80a_mn.inc $(Z80A)/z80dma.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80a_s.oa:	$(Z80A)/z80a_s.s $(Z80A)/z80a.inc $(Z80A)/z80a_mn.inc \
					$(Z80A)/z80a_s.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80a_cb.oa:	$(Z80A)/z80a_cb.s $(Z80A)/z80a.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80a_i.oa:	$(Z80A)/z80a_i.s $(Z80A)/z80a.inc $(Z80A)/z80a_mn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80a_io.oa:	$(Z80A)/z80a_io.s $(TARGET2)/ramptr.inc \
					$(Z80A)/z80a.inc $(PATCH)/iocore.inc $(VRAM)/makescrn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80mem.oa:	$(Z80A)/z80mem.s $(Z80A)/z80a.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/z80dmap2.oa:	$(Z80A)/z80dmap2.s $(TARGET2)/ramptr.inc \
					$(Z80A)/z80a.inc $(Z80A)/z80dma.inc
	$(AS) $(AOPT9) $< -o $@



$(OBJ)/iocore.o:	$(PATCH)/iocore.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/iocored.oa:	$(PATCH)/iocored.s $(TARGET2)/ramptr.inc \
					$(PATCH)/iocore.inc $(VRAM)/makescrn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/cgrom.o:		$(IO)/cgrom.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(PATCH)/font.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/cmt.o:		$(IO)/cmt.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/crtc.o:		$(IO)/crtc.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(VRAM)/vram.h \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/ctc.o:		$(PATCH)/ctc.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/ctc2.oa:		$(PATCH)/ctc2.s $(Z80A)/z80a.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/dipsw.o:		$(IO)/dipsw.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/dmac.o:		$(IO)/dmac.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT9) -c $< -o $@ -Wa,
#	$(CC) $(COPT9) -Wa,-alh -c $< -o $@

$(OBJ)/fdc.o:		$(PATCH)/fdc.c $(HEADERS) $(Z80CORES) $(PCCORES) $(IOCORES) \
					$(BASE)/nevent.h $(FDD)/fddfile.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/fdc2.oa:		$(PATCH)/fdc2.s
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/memio.o:		$(PATCH)/memio.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(IO)/defipl.res
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/pcg.o:		$(IO)/pcg.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(VRAM)/vram.h \
					$(VRAM)/makescrn.h $(PATCH)/font.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/ppi.o:		$(PATCH)/ppi.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(BASE)/nevent.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/ppi2.oa:		$(PATCH)/ppi2.s $(TARGET2)/ramptr.inc $(PATCH)/iocore.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/sio.o:		$(IO)/sio.c $(HEADERS) $(Z80CORES) $(PCCORES) $(IOCORES) \
					$(TARGET)/mousemng.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/sndboard.o:	$(PATCH)/sndboard.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(TARGET)/ipcxfer.h $(SOUND)/nds9psg.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/sndboard.oa	:$(PATCH)/sndboard.s $(TARGET2)/ramptr.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/subcpu.o:	$(PATCH)/subcpu.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(TARGET)/timemng.h $(BASE)/nevent.h \
					$(BASE)/ievent.h $(BASE)/keystat.h $(BASE)/calendar.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/subcpu2.oa:	$(PATCH)/subcpu2.s $(TARGET2)/ramptr.inc \
					$(PATCH)/iocore.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/vramio.o:	$(PATCH)/vramio.c $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/vramio2.oa:	$(PATCH)/vramio2.s $(TARGET2)/ramptr.inc \
					$(Z80A)/z80a.inc $(PATCH)/iocore.inc $(VRAM)/vram.inc
	$(AS) $(AOPT9) $< -o $@



$(OBJ)/palettes.o:	$(VRAM)/palettes.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makescrn.o:	$(VRAM)/makescrn.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/palettes.h $(VRAM)/makescrn.h \
					$(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makesub.o:	$(VRAM)/makesub.cpp $(HEADERS) $(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makesub2.oa:	$(VRAM)/makesub2.s
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/makeatr.o:	$(VRAM)/makeatr.cpp $(HEADERS) $(VRAM)/vram.h \
					$(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makechr.o:	$(VRAM)/makechr.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h $(VRAM)/makesub.h \
					$(PATCH)/font.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makemix.o:	$(VRAM)/makemix.cpp $(HEADERS) $(VRAM)/vram.h \
					$(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makemix2.oa:	$(VRAM)/makemix2.s $(VRAM)/makescrn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/make15.o:	$(VRAM)/make15.cpp $(HEADERS) $(PCCORES) $(VRAM)/vram.h \
					$(VRAM)/makescrn.h $(VRAM)/makesub.h $(PATCH)/font.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/make15a.oa:	$(VRAM)/make15a.s $(TARGET2)/ndsreg.inc \
					$(VRAM)/vram.inc $(VRAM)/makescrn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/makec8.o:	$(VRAM)/makec8.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h $(VRAM)/makesub.h \
					$(PATCH)/font.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makec8a.oa:	$(VRAM)/makec8a.s \
					$(TARGET2)/ramptr.inc $(TARGET2)/romimg.inc \
					$(VRAM)/makescrn.inc $(VRAM)/maketxtl.inc \
					$(VRAM)/makeatr.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/makec16.o:	$(VRAM)/makec16.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
					$(VRAM)/vram.h $(VRAM)/makescrn.h $(VRAM)/makesub.h \
					$(PATCH)/font.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makecs.o:	$(VRAM)/makecs.cpp $(HEADERS) $(VRAM)/vram.h \
					$(VRAM)/makescrn.h $(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/makecsa.oa:	$(VRAM)/makecsa.s $(VRAM)/makescrn.inc
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/maketxtl.o:	$(VRAM)/maketxtl.cpp $(HEADERS) $(VRAM)/makescrn.h \
					$(VRAM)/makesub.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/vsyncff.o:	$(VRAM)/vsyncff.cpp
	$(CXX) $(COPT9) -c $< -o $@



$(OBJ)/nds9psg.o:	$(SOUND)/nds9psg.cpp $(HEADERS) $(TARGET)/ipcxfer.h \
					$(SOUND)/nds9psg.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/nds7psg.o:	$(SOUND)/nds7psg.cpp $(HEADERS) $(TARGET)/ipcxfer.h \
					$(SOUND)/nds7psg.h
	$(CXX) $(COPT7) -c $< -o $@


$(OBJ)/opmgeng.oa:	$(SOUND)/opmgeng.s
	$(AS) $(AOPT9) $< -o $@



$(OBJ)/fddfile.o:	$(FDD)/fddfile.c $(HEADERS) $(PCCORES) $(IOOCORES) \
					$(FDD)/fddfile.h $(FDD)/fdd_2d.h $(FDD)/fdd_d88.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/fdd_2d.o:	$(FDD)/fdd_2d.c $(HEADERS) $(PCCORES) $(FDD)/fddfile.h \
					$(FDD)/fdd_2d.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/fdd_d88.o:	$(FDD)/fdd_d88.c $(HEADERS) $(PCCORES) $(FDD)/fddfile.h \
					$(FDD)/fdd_d88.h
	$(CC) $(COPT9) -c $< -o $@



$(OBJ)/xmil9.o:		$(TARGET)/xmil9.cpp $(HEADERS) $(TARGET)/xmil9.h \
					$(TARGET)/sysmng.h $(Z80CORES) $(PCCORES) $(IOCORES) \
					$(TARGET)/joymng.h $(BASE)/timing.h $(VRAM)/makescrn.h \
					$(FDD)/fddfile.h $(PATCH)/font.h $(TARGET)/ipcxfer.h \
					$(SOUND)/nds9psg.h $(VRAM)/vram.h $(VRAM)/vsyncff.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/xmil7.o:		$(TARGET)/xmil7.cpp $(HEADERS) $(TARGET)/xmil7.h \
					$(TARGET)/ipcxfer.h $(SOUND)/nds7psg.h
	$(CXX) $(COPT7) -c $< -o $@

$(OBJ)/joymng.o:	$(TARGET)/joymng.cpp $(HEADERS) $(TARGET)/mousemng.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/mousemng.o:	$(TARGET)/mousemng.cpp $(HEADERS) $(TARGET)/mousemng.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/sysmng.o:	$(TARGET)/sysmng.cpp $(HEADERS) $(TARGET)/sysmng.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/timemng.o:	$(TARGET)/timemng.cpp $(HEADERS) $(TARGET)/timemng.h
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/bss.oa:		$(TARGET)/bss.s
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/resource.o:	$(TARGET)/resource.cpp
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/softkbd9.o:	$(TARGET)/softkbd9.cpp
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/softkbd7.o:	$(TARGET)/softkbd7.cpp
	$(CXX) $(COPT7) -c $< -o $@



$(OBJ)/libnds9.o:	$(TARGET2)/libnds9.cpp $(HEADERS)
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/libnds7.o:	$(TARGET2)/libnds7.cpp $(HEADERS)
	$(CXX) $(COPT7) -c $< -o $@

$(OBJ)/ndscore.o:	$(TARGET2)/ndscore.s
	$(AS) $(AOPT9) $< -o $@

$(OBJ)/trace.o:		$(TARGET2)/trace.cpp $(HEADERS)
	$(CXX) $(COPT9) -c $< -o $@

$(OBJ)/extrom.o:	$(TARGET2)/extrom.cpp $(HEADERS)
	$(CXX) $(COPT9) -c $< -o $@



$(OBJ)/pccore.o:	$(BASE)/pccore.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(TARGET)/sysmng.h $(TARGET)/timemng.h \
					$(BASE)/xmilver.h $(BASE)/nevent.h $(BASE)/ievent.h \
					$(BASE)/timing.h $(BASE)/calendar.h $(BASE)/keystat.h \
					$(VRAM)/palettes.h $(VRAM)/makescrn.h $(SOUND)/sound.h \
					$(SOUND)/sndctrl.h $(PATCH)/font.h $(FDD)/fddfile.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/nevent.o:	$(BASE)/nevent.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(BASE)/nevent.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/ievent.o:	$(BASE)/ievent.c $(HEADERS) $(Z80CORES) $(PCCORES) \
					$(IOCORES) $(BASE)/nevent.h $(BASE)/ievent.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/keystat.o:	$(BASE)/keystat.c $(HEADERS) $(PCCORES) \
					$(BASE)/keystat.h $(BASE)/keystat.tbl
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/calendar.o:	$(BASE)/calendar.c $(HEADERS) $(PCCORES) \
					$(COMMON)/parts.h $(TARGET)/timemng.h $(BASE)/nevent.h \
					$(BASE)/calendar.h
	$(CC) $(COPT9) -c $< -o $@

$(OBJ)/timing.o:	$(BASE)/timing.c $(HEADERS) $(PCCORES) \
					$(BASE)/timing.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT9) -c $< -o $@



clean:
	$(RM) $(NDS9OBJ)
	$(RM) $(NDS9ELF)
	$(RM) $(NDS9ARM)
	$(RM) $(NDSBIN)
	$(RM) $(NDSROM)

