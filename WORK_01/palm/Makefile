WARNINGS = -W -Wall -Wpointer-arith
OPTIMIZATION = -O2
APP = xmil
PEAL = ./support_files/peal-2004_12_29

ARMSRCS = ../calendar.c ../common/_memory.c ../common/bmpdata.c ../common/milstr.c ../common/parts.c ../common/profile.c ../common/rect.c ../common/strres.c ../common/textfile.c ../common/wavefile.c ../fdd/diskdrv.c ../fdd/fdd_2d.c ../fdd/fdd_d88.c ../fdd/fdd_mtr.c ../fdd/fddfile.c ../fdd/newdisk.c ../font/font.c ../font/fontdata.c ../font/fontmake.c ../font/fontx1.c ../ievent.c ../io/cgrom.c ../io/cmt.c ../io/crtc.c ../io/ctc.c ../io/dipsw.c ../io/dmac.c ../io/fdc.c ../io/iocore.c ../io/memio.c ../io/pcg.c ../io/ppi.c ../io/sio.c ../io/sndboard.c ../io/subcpu.c ../io/vramio.c ../keystat.c ../pccore.c ../sound/opmgenc.c ../sound/psggenc.c ../sound/opmgeng.c ../nevent.c ../sound/psggeng.c ../sound/sndcsec.c ../sound/sndctrl.c ../sound/sound.c ../sound/x1f.c ../timing.c ../vram/make15.c ../vram/make24.c ../vram/makeatr.c ../vram/makechr.c ../vram/makemix.c ../vram/makescrn.c ../vram/makesub.c ../vram/maketxth.c ../vram/maketxtl.c ../vram/palettes.c ../vram/scrndraw.c ../vram/scrnsave.c ../vram/sdrawq16.c ../vram/vram.c ../z80c/z80c_cb.c ../z80c/z80c_ix.c ../z80c/z80c_iy.c ../z80c/z80c_mn.c ../z80c/z80c_sb.c ../z80c/z80c.c ../z80c/z80dmap.c ../z80c/z80mem.c dosio.c fontmng.c joymng.c mousemng.c timemng.c palmossub.c scrnmng.c soundmng.c sysmng.c xmil.c dialog/d_bmp.c dialog/d_config.c dialog/d_disk.c dialog/dialogs.c palmkbd.c ini.c ../statsave.c trace.c support_files/fdlibm/e_log.c support_files/fdlibm/e_pow.c support_files/fdlibm/e_rem_pio2.c support_files/fdlibm/k_cos.c support_files/fdlibm/k_rem_pio2.c support_files/fdlibm/k_sin.c support_files/fdlibm/s_copysign.c support_files/fdlibm/s_scalbn.c support_files/fdlibm/s_sin.c support_files/fdlibm/w_log.c support_files/fdlibm/w_pow.c support_files/fdlibm/e_sqrt.c support_files/fdlibm/w_sqrt.c support_files/fdlibm/s_floor.c support_files/fdlibm/s_fabs.c

INCLUDE = -I./ -I./dialog -I./support_files -I./support_files/fdlibm -I../ -I../common -I../fdd -I../font -I../IO -I../sound -I../vram -I../z80c -I-

all: $(APP).prc

$(APP).prc: $(APP).def $(APP).o $(APP).ro armc.ro
	build-prc -o $(APP).prc $(APP).def $(APP).o $(APP).ro armc.ro

armc.ro: $(ARMSRCS) $(PEAL)/arm/pealstub.c $(PEAL)/arm/pealstub.h
	arm-palmos-gcc -Lnfm $(OPTIMIZATION) -fPIC -march=armv4t -c $(PEAL)/arm/pealstub.c -o pealstub.o

	arm-palmos-gcc -Lnfm $(OPTIMIZATION) -Wl,--split-by-file=64000 -fPIC -march=armv4t -msingle-pic-base -Wl,--emit-relocs -nostartfiles $(WARNINGS) -I$(PEAL)/arm $(INCLUDE) $(ARMSRCS) pealstub.o -o armc03e8.bin
	$(PEAL)/postlink/peal-postlink -v -s 1000 -o armc.ro armc03e8.bin
#	../../postlink/peal-postlink -v -o armc.unsplit armc03e8.bin


$(APP).ro: $(APP).rcp
	pilrc -q -ro $(APP).rcp $(APP).ro

#$(APP).o: m68k.o peal.o $(OBJS)
#	m68k-palmos-gcc m68k.o peal.o $(OBJS) -o $(APP).o -lPalmOSGlue
$(APP).o: m68k.o peal.o
	m68k-palmos-gcc m68k.o peal.o -o $(APP).o -lPalmOSGlue

m68k.o: m68k.c $(PEAL)/m68k/peal.h
	m68k-palmos-gcc $(OPTIMIZATION) $(WARNINGS) -I$(PEAL)/m68k $(INCLUDE) -c m68k.c -o m68k.o
peal.o: $(PEAL)/m68k/peal.c $(PEAL)/m68k/peal.h $(PEAL)/m68k/elf.h $(PEAL)/m68k/elf_common.h $(PEAL)/m68k/elf32.h
	m68k-palmos-gcc $(OPTIMIZATION) $(WARNINGS) -I$(PEAL)/m68k -c $(PEAL)/m68k/peal.c -o peal.o


clean:
	rm -f *~ *.o *.ro $(APP).prc *.bin *.unsplit
