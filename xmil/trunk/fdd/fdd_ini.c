#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"fdd_ini.h"


BRESULT is_d8ufile(const OEMCHAR *fname) {

	(void)fname;
	return(FALSE);
}

BRESULT fdd_ini(const OEMCHAR *filename) {

	(void)filename;
	return(SUCCESS);
}


#if 0
#define		FDDINISIZE	(MAX_PATH+32)

bool is_d8ufile(LPSTR fname) {

	int		leng;

	leng = strlen(fname);
	if ((leng > 4) && (!milstr_cmp(&fname[leng-4], ".D8U"))) {
		return(TRUE);
	}
	return(FALSE);
}


void inistradjust(LPSTR str) {

	int		qout = 0;
	LPSTR	p = str;
	BYTE	c;

	while(1) {
		if ((c = *str++) == '\0') {
			break;
		}
		if (!qout) {
			if (c <= ' ') {
				continue;
			}
			if (c == ';') {
				break;
			}
		}
		if (c == '\"') {
			if ((!qout) || (*str != '\"')) {
				qout ^= 1;
				continue;
			}
			str++;
		}
		*p++ = c;
	}
	*p = '\0';
}


LPSTR checkkey(LPSTR src, LPSTR key) {

	if (milstr_cmp(src, key)) {
		return(NULL);
	}
	src += strlen(key);
	if (*src++ != '=') {
		return(NULL);
	}
	return(src);
}

char	*fddlater[4] = {"FDD0", "FDD1", "FDD2", "FDD3"};


BYTE fdd_ini(LPSTR filename) {

	FILEH	fh;
	int		resetflg = 0;
	BYTE	romtype = xmilcfg.ROM_TYPE;
	BYTE	soundsw = xmilcfg.SOUND_SW;
	BYTE	dipsw = xmilcfg.DIP_SW;
	BYTE	fddstat[4] = {0, 0, 0, 0};
	char	work[FDDINISIZE];
	char	path[MAX_PATH + 32];
	LPSTR	p;
	short	i;

	if ((fh = file_open(filename)) == (FILEH)-1) {
		return(SUCCESS);
	}
	while(file_lineread(fh, work, FDDINISIZE)) {
		inistradjust(work);
		if (!milstr_cmp(work, "FDD")) {
			for (i=0; i<4; i++) {
				if ((!fddstat[i]) && 
					((p = checkkey(work, fddlater[i])) != NULL)) {
					fddstat[i] = 1;
					if (*p) {
						milstr_ncpy(path, filename, MAX_PATH);
						fname_mix(path, p, MAX_PATH);
						x1_set_fd(i, 0, path);
					}
					else {
						x1_eject_fd(i);
					}
				}
			}
		}
		else if ((p = checkkey(work, "RESET")) != NULL) {
			if (solveBOOL(p)) {
				resetflg = 1;
			}
			else if (!milstr_cmp(p, "TURBOZ")) {
				resetflg = 1;
				romtype = 3;
			}
			else if (!milstr_cmp(p, "TURBO")) {
				resetflg = 1;
				romtype = 2;
			}
			else if ((!milstr_cmp(p, "NORMAL")) || (!milstr_cmp(p, "X1"))) {
				resetflg = 1;
				romtype = 1;
			}
		}
		else if ((p = checkkey(work, "IPL")) != NULL) {
			if (!milstr_cmp(p, "TURBOZ")) {
				romtype = 3;
			}
			else if (!milstr_cmp(p, "TURBO")) {
				romtype = 2;
			}
			else if ((!milstr_cmp(p, "NORMAL")) || (!milstr_cmp(p, "X1"))) {
				romtype = 1;
			}
		}
		else if (((p = checkkey(work, "FM")) != NULL) ||
				((p = checkkey(work, "OPM")) != NULL)) {
			soundsw = solveBOOL(p);
		}
		else if (((p = checkkey(work, "BOOT")) != NULL) ||
				((p = checkkey(work, "DISK")) != NULL)) {
			if (!milstr_cmp(p, "2D")) {
				dipsw &= (~DIP_BOOTMEDIA);
			}
			else if (!milstr_cmp(p, "2HD")) {
				dipsw |= DIP_BOOTMEDIA;
			}
		}
		else if (((p = checkkey(work, "SCREEN")) != NULL) ||
				((p = checkkey(work, "RESOLUTE")) != NULL) ||
				((p = checkkey(work, "RESOLUTION")) != NULL)) {
			if (!milstr_cmp(p, "HIGH")) {
				dipsw &= (~DIP_RESOLUTE);
			}
			else if ((!milstr_cmp(p, "NORMAL")) || (!milstr_cmp(p, "LOW"))) {
				dipsw |= DIP_RESOLUTE;
			}
		}
	}
	file_close(fh);
	if (resetflg) {
		if (reset_x1(romtype, soundsw, dipsw)) {
			return(FAILURE);
		}
	}
	return(SUCCESS);
}
#endif

