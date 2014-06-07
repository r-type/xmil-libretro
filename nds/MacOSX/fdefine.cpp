#include	"compiler.h"
#include	"dosio.h"
#include	"strres.h"

void Setfiletype(int ftype, OSType *creator, OSType *fileType) {
	switch(ftype) {
		case FTYPE_D88:				// D88
			*fileType = '.D88';
			break;

		case FTYPE_BETA:			// ベタイメージ
			*fileType = '.2D ';
			break;

	}
}

static int Getfiletype(FInfo *fndrinfo) {

	switch(fndrinfo->fdType) {
		case '.D88':
			return(FTYPE_D88);

		case 'BETA':
		case '.2D ':
			return(FTYPE_BETA);
	}
	return(FTYPE_NONE);
}

static int GetFileExt(char* filename) {

    char*	p;
    char*	n;
    int		ftype;
    
    p = file_getext((char *)filename);
    n = file_getname((char *)filename);
    if (!milstr_cmp(p, str_d88)) {
			ftype = FTYPE_D88;
		}
    else if (!milstr_cmp(p, str_e2d)) {
        ftype = FTYPE_BETA;
    }
    else if (!milstr_cmp(p, str_ini)) {
			ftype = FTYPE_SMIL;
		}
    else if (!milstr_cmp(p, str_bmp)) {
			ftype = FTYPE_BMP;
		}
    else {
        ftype = FTYPE_NONE;
    }
    return(ftype);
}

int file_getftype(char* filename) {

	FSSpec	fss;
	Str255	fname;
	FInfo	fndrInfo;
    int		ftype;

    ftype = GetFileExt(filename);
	if (ftype == FTYPE_NONE) {
        mkstr255(fname, filename);
        FSMakeFSSpec(0, 0, fname, &fss);
        if (FSpGetFInfo(&fss, &fndrInfo) != noErr) {
            return(FTYPE_NONE);
        }
        ftype = Getfiletype(&fndrInfo);
    }
	return(ftype);
}
