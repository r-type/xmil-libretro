#include	"compiler.h"
#include	"resource.h"
#include	"xmil.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"

void SoundDialogProc(void) {

    FormType*	frm;
	UINT		rate;
	UINT		ms;
	UINT16		slidersata, ret;
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	ControlType*	fmboard;
	SINT16			fm;
#endif
	ControlType*	sample;
	ControlType*	buffer;
	UINT			update = 0;

	frm = ARM_FrmInitForm(IDD_SOUNDOPT);
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
	fmboard = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_FMBOARD));
	ARM_CtlSetValue(fmboard, xmilcfg.SOUND_SW);
#endif
	rate = xmilcfg.samplingrate;
	if (rate == 11025) {
		slidersata = 0;
	} else if (rate == 44100) {
		slidersata = 2;
	}
	else {
		slidersata = 1;
	}
	sample = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_SAMPLERATE));
	ARM_CtlSetValue(sample, slidersata);
		
	ms = xmilcfg.delayms;
	if (ms < 50) {
		ms = 50;
	}
	else if (ms > 1000) {
		ms = 1000;
	}
	buffer = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_SNDBUFFER));
	ARM_CtlSetValue(buffer, ms);
	ret = ARM_FrmDoDialog(frm);
	if (ret == IDC_OK) {
#if defined(SUPPORT_TURBOZ) || defined(SUPPORT_OPM)
		fm = ARM_CtlGetValue(fmboard);
		if (fm != xmilcfg.SOUND_SW) {
			xmilcfg.SOUND_SW = fm;
			update |= SYS_UPDATECFG;
		}
#endif
		slidersata = ARM_CtlGetValue(sample);
		if (slidersata == 0) {
			rate = 11025;
		} else if (slidersata == 2) {
			rate = 44100;
		}
		else {
			rate = 22050;
		}
		if (rate != xmilcfg.samplingrate) {
			xmilcfg.samplingrate = rate;
			update |= SYS_UPDATECFG;
		}
		ms = ARM_CtlGetValue(buffer);
		if (ms != xmilcfg.delayms) {
			xmilcfg.delayms = ms;
			update |= SYS_UPDATECFG;
		}
	}
	ARM_FrmDeleteForm(frm);
	sysmng_update(update);
}

void OptionDialogProc(void) {

    FormType*		frm;
	ControlType*	joyrev;
	ControlType*	joyrapid;
	ControlType*	keym;
	SINT16			rev, rap, skip, ret;
	UINT			update = 0;
	int				i;

	frm = ARM_FrmInitForm(IDD_OPTION);
	joyrev = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_JOYX));
	joyrapid = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_RAPID));
	keym = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_JOYSTICK+xmiloscfg.FIVEWAY));
	ARM_CtlSetValue(keym, 1);
	ARM_CtlSetValue(joyrev, xmilcfg.BTN_MODE);
	ARM_CtlSetValue(joyrapid, xmilcfg.BTN_RAPID);
	ret = ARM_FrmDoDialog(frm);
	if (ret == IDC_OK) {
		rev = ARM_CtlGetValue(joyrev);
		if (rev != xmilcfg.BTN_MODE) {
			xmilcfg.BTN_MODE = rev;
			update |= SYS_UPDATECFG;
		}
		rap = ARM_CtlGetValue(joyrapid);
		if (rap != xmilcfg.BTN_RAPID) {
			xmilcfg.BTN_RAPID = rap;
			update |= SYS_UPDATECFG;
		}
		if (ARM_CtlGetValue(keym) == 0) {
			for (i=2;i>0;i--) {
				skip = ARM_CtlGetValue(ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_JOYSTICK+i)));
				if (skip != 0) {
					break;
				}
			}
			xmiloscfg.FIVEWAY = i;
			update |= SYS_UPDATEOSCFG;
		}
	}
	ARM_FrmDeleteForm(frm);
	sysmng_update(update);
}

void ScrnOptionDialogProc(void) {

    FormType*		frm;
	ControlType*	dispsync;
	ControlType*	raster;
	ControlType*	frame;
	SINT16			sync, ras, skip, ret;
	UINT			update = 0;
	int				i;

	frm = ARM_FrmInitForm(IDD_SCRNOPT);
	dispsync = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_DISPSYNC));
	raster = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_RASTER));
	ARM_CtlSetValue(dispsync, xmilcfg.DISPSYNC);
	ARM_CtlSetValue(raster, xmilcfg.RASTER);
	frame = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_AUTO+xmiloscfg.DRAW_SKIP));
	ARM_CtlSetValue(frame, 1);
	ret = ARM_FrmDoDialog(frm);
	if (ret == IDC_OK) {
		sync = ARM_CtlGetValue(dispsync);
		if (sync != xmilcfg.DISPSYNC) {
			xmilcfg.DISPSYNC = sync;
			update |= SYS_UPDATECFG;
		}
		ras = ARM_CtlGetValue(raster);
		if (ras != xmilcfg.RASTER) {
			xmilcfg.RASTER = ras;
			update |= SYS_UPDATECFG;
		}
		if (ARM_CtlGetValue(frame) == 0) {
			for (i=4;i>0;i--) {
				skip = ARM_CtlGetValue(ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_AUTO+i)));
				if (skip != 0) {
					break;
				}
			}
			xmiloscfg.DRAW_SKIP = i;
			update |= SYS_UPDATEOSCFG;
		}
	}
	ARM_FrmDeleteForm(frm);
	sysmng_update(update);
}

void DipswDialogProc(void) {

    FormType*		frm;
	ControlType*	boot2hd;
	ControlType*	hireso;
	SINT16			boot, reso, ret;
	UINT8			dipsw;

	frm = ARM_FrmInitForm(IDD_DIPSW);
	boot2hd = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_BOOT2HD));
	hireso = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_HIGHRES));
	ARM_CtlSetValue(boot2hd, (xmilcfg.DIP_SW & DIPSW_BOOTMEDIA)?1:0);
	ARM_CtlSetValue(hireso, (xmilcfg.DIP_SW & DIPSW_RESOLUTE)?0:1);
	ret = ARM_FrmDoDialog(frm);
	if (ret == IDC_OK) {
		boot = ARM_CtlGetValue(boot2hd);
		reso = ARM_CtlGetValue(hireso);
		dipsw = ((boot)?DIPSW_BOOTMEDIA:0) | ((reso)?0:DIPSW_RESOLUTE);
		if (dipsw != xmilcfg.DIP_SW) {
			xmilcfg.DIP_SW = dipsw;
			sysmng_update(SYS_UPDATECFG);
		}
	}
	ARM_FrmDeleteForm(frm);
}

void ConfigDialogProc(void) {

    FormType*		frm;
	ControlType*	resume;
	ControlType*	graf;
	SINT16			res, fiti, ret;
	UINT			update = 0;

	frm = ARM_FrmInitForm(IDD_CONFIG);
	resume = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_RESUME));
	graf = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_GRAFFITI));
	ARM_CtlSetValue(resume, xmiloscfg.resume);
	ARM_CtlSetValue(graf, xmiloscfg.graffiti);
	ret = ARM_FrmDoDialog(frm);
	if (ret == IDC_OK) {
		fiti = ARM_CtlGetValue(graf);
		if (fiti != xmiloscfg.graffiti) {
			xmiloscfg.graffiti = fiti;
			update |= SYS_UPDATEOSCFG;
		}
		res = ARM_CtlGetValue(resume);
		if (res != xmiloscfg.resume) {
			xmiloscfg.resume = res;
			update |= SYS_UPDATEOSCFG;
		}
	}
	ARM_FrmDeleteForm(frm);
	sysmng_update(update);
}

