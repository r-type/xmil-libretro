#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"newdisk.h"
#include	"xmil.h"
#include	"scrndraw.h"

void dialog_changefdd(REG8 drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dlgs_selectfile(fname)) {
			diskdrv_setfddex(drv, fname, 0, 0);
		}
		scrndraw_redraw();
	}
}


// ---- newdisk

static const char str_newdisk[] = "newdisk.d88";

typedef struct {
	BYTE	fdtype;
	char	label[16+1];
} NEWDISK;

void dialog_newdisk( void ) {


    FormType*		frm;
	ControlType*	type;
	FieldType*		label;
	NEWDISK			disk;
	char*			disklabel;
	char			path[MAX_PATH];
	int				media;
	SINT16			ret;

	if (dlgs_selectwritefile(path, sizeof(path), str_newdisk)) {
		frm = ARM_FrmInitForm(IDD_NEWDISK);
		type = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_MAKE2D));
		ARM_CtlSetValue(type, 1);
		ret = ARM_FrmDoDialog(frm);
		if (ret == IDC_OK) {
			media = ARM_CtlGetValue(type);
			if (media == 1) {
				disk.fdtype = (DISKTYPE_2DD << 4);
			}
			else {
				disk.fdtype = (DISKTYPE_2HD << 4);
			}
			label = ARM_FrmGetObjectPtr(frm, ARM_FrmGetObjectIndex(frm, IDC_DISKLABEL));
			disklabel = ARM_FldGetTextPtr(label);
			milstr_ncpy(disk.label, disklabel, sizeof(disk.label));
			ARM_FrmDeleteForm(frm);
			newdisk_fdd(path, disk.fdtype, disk.label);
		}
	}
    return;
}

