#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"timemng.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"nevent.h"
#include	"ievent.h"
#include	"calendar.h"
#include	"statsave.h"
#include	"vram.h"
#include	"palettes.h"
#include	"makescrn.h"
#include	"sound.h"
#include	"fddfile.h"

#if defined(MACOS)
#define	CRCONST		str_cr
#elif defined(WIN32) || defined(X11) || defined(SLZAURUS)
#define	CRCONST		str_lf
#else
#define	CRCONST		str_crlf
#endif


typedef struct {
	char	name[16];
	char	vername[28];
	UINT32	ver;
} NP2FHDR;

typedef struct {
	char	index[10];
	UINT16	ver;
	UINT32	size;
} NP2FENT;

enum {
	STATFLAG_BIN			= 0,
	STATFLAG_TERM,
	STATFLAG_DISK,
	STATFLAG_EVT
};

typedef struct {
	UINT32	id;
	INTPTR	proc;
} PROCTBL;

typedef struct {
	UINT32	id;
	UINT	evt;
} ENUMTBL;

#define	PROCID(a, b, c, d)	(((d) << 24) + ((c) << 16) + ((b) << 8) + (a))

#include "statsave.tbl"


/* ---- */

/* 関数ポインタを intに変更。 */
static UINT32 proc2id(INTPTR func, const PROCTBL *tbl, UINT count) {

	while(count) {
		count--;
		if (func == tbl->proc) {
			return(tbl->id);
		}
		tbl++;
	}
	return(0);
}

static INTPTR id2proc(UINT32 id, const PROCTBL *tbl, UINT count) {

	while(count) {
		count--;
		if (id == tbl->id) {
			return(tbl->proc);
		}
		tbl++;
	}
	return((INTPTR)NULL);
}


/* ---- */

enum {
	SFFILEH_WRITE	= 0x0001,
	SFFILEH_BLOCK	= 0x0002,
	SFFILEH_ERROR	= 0x0004
};

typedef struct {
	_STFLAGH	sfh;
	UINT		stat;
	FILEH		fh;
	UINT		secpos;
	NP2FHDR		f;
} _SFFILEH, *SFFILEH;

static SFFILEH statflag_open(const OEMCHAR *filename,
												OEMCHAR *err, UINT errlen) {

	FILEH	fh;
	SFFILEH	ret;

	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto sfo_err1;
	}
	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfo_err2;
	}
	if ((file_read(fh, &ret->f, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) &&
		(!memcmp(&ret->f, &np2flagdef, sizeof(np2flagdef)))) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		if ((err) && (errlen > 0)) {
			err[0] = '\0';
			ret->sfh.err = err;
			ret->sfh.errlen = errlen;
		}
		return(ret);
	}
	_MFREE(ret);

sfo_err2:
	file_close(fh);

sfo_err1:
	return(NULL);
}

static int statflag_closesection(SFFILEH sffh) {

	UINT	leng;
	UINT8	zero[16];

	if (sffh == NULL) {
		goto sfcs_err1;
	}
	if (sffh->stat == (SFFILEH_BLOCK | SFFILEH_WRITE)) {
		leng = (0 - sffh->sfh.hdr.size) & 15;
		if (leng) {
			ZeroMemory(zero, sizeof(zero));
			if (file_write(sffh->fh, zero, leng) != leng) {
				goto sfcs_err2;
			}
		}
		if ((file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) ||
			(file_write(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												!= sizeof(sffh->sfh.hdr))) {
			goto sfcs_err2;
		}
	}
	if (sffh->stat & SFFILEH_BLOCK) {
		sffh->stat &= ~SFFILEH_BLOCK;
		sffh->secpos += sizeof(sffh->sfh.hdr) +
									((sffh->sfh.hdr.size + 15) & (~15));
		if (file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) {
			goto sfcs_err2;
		}
	}
	return(STATFLAG_SUCCESS);

sfcs_err2:
	sffh->stat = SFFILEH_ERROR;

sfcs_err1:
	return(STATFLAG_FAILURE);
}

static int statflag_readsection(SFFILEH sffh) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if ((sffh->stat == 0) &&
		(file_read(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												== sizeof(sffh->sfh.hdr))) {
		sffh->stat = SFFILEH_BLOCK;
		sffh->sfh.pos = 0;
		return(STATFLAG_SUCCESS);
	}
	sffh->stat = SFFILEH_ERROR;
	return(STATFLAG_FAILURE);
}

int statflag_read(STFLAGH sfh, void *buf, UINT size) {

	if ((sfh == NULL) || (buf == NULL) ||
		((sfh->pos + size) > sfh->hdr.size)) {
		goto sfr_err;
	}
	if (size) {
		if (file_read(((SFFILEH)sfh)->fh, buf, size) != size) {
			goto sfr_err;
		}
		sfh->pos += size;
	}
	return(STATFLAG_SUCCESS);

sfr_err:
	return(STATFLAG_FAILURE);
}

static SFFILEH statflag_create(const OEMCHAR *filename) {

	SFFILEH	ret;
	FILEH	fh;

	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfc_err1;
	}
	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto sfc_err2;
	}
	if (file_write(fh, &np2flagdef, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->stat = SFFILEH_WRITE;
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		return(ret);
	}
	file_close(fh);
	file_delete(filename);

sfc_err2:
	_MFREE(ret);

sfc_err1:
	return(NULL);
}

static int statflag_createsection(SFFILEH sffh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if (sffh->stat != SFFILEH_WRITE) {
		sffh->stat = SFFILEH_ERROR;
		return(STATFLAG_FAILURE);
	}
	CopyMemory(sffh->sfh.hdr.index, tbl->index, sizeof(sffh->sfh.hdr.index));
	sffh->sfh.hdr.ver = tbl->ver;
	sffh->sfh.hdr.size = 0;
	return(STATFLAG_SUCCESS);
}

int statflag_write(STFLAGH sfh, const void *buf, UINT size) {

	SFFILEH	sffh;

	if (sfh == NULL) {
		goto sfw_err1;
	}
	sffh = (SFFILEH)sfh;
	if (!(sffh->stat & SFFILEH_WRITE)) {
		goto sfw_err2;
	}
	if (!(sffh->stat & SFFILEH_BLOCK)) {
		sffh->stat |= SFFILEH_BLOCK;
		sfh->pos = 0;
		if (file_write(sffh->fh, &sfh->hdr, sizeof(sfh->hdr))
														!= sizeof(sfh->hdr)) {
			goto sfw_err2;
		}
	}
	if (size) {
		if ((buf == NULL) || (file_write(sffh->fh, buf, size) != size)) {
			goto sfw_err2;
		}
		sfh->pos += size;
		if (sfh->hdr.size < sfh->pos) {
			sfh->hdr.size = sfh->pos;
		}
	}
	return(STATFLAG_SUCCESS);

sfw_err2:
	sffh->stat = SFFILEH_ERROR;

sfw_err1:
	return(STATFLAG_FAILURE);
}

static void statflag_close(SFFILEH sffh) {

	if (sffh) {
		statflag_closesection(sffh);
		file_close(sffh->fh);
		_MFREE(sffh);
	}
}

void statflag_seterr(STFLAGH sfh, const OEMCHAR *str) {

	if ((sfh) && (sfh->errlen)) {
		milstr_ncat(sfh->err, str, sfh->errlen);
		milstr_ncat(sfh->err, CRCONST, sfh->errlen);
	}
}


/* common */

static int flagsave_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_write(sfh, tbl->arg1, tbl->arg2));
}

static int flagload_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_read(sfh, tbl->arg1, tbl->arg2));
}


/* event */

typedef struct {
	UINT32	next;
	SINT32	clock;
	SINT32	baseclock;
	UINT32	proc;
} NEVTITEM;

typedef struct {
	NEVTITEM	item[NEVENT_MAXEVENTS];
	UINT32		first;
} NEVTSAVE;

static UINT32 evt2id(NEVENTITEM next) {

	if (next == NEVENTITEM_NONE) {
		return((UINT32)-2);
	}
	else if (next == NEVENTITEM_TERM) {
		return((UINT32)-1);
	}
	else {
		return(next - nevent.item);
	}
}

static NEVENTITEM id2evt(UINT32 n) {

	if (n == (UINT32)-2) {
		return(NEVENTITEM_NONE);
	}
	else if (n == (UINT32)-1) {
		return(NEVENTITEM_TERM);
	}
	else if (n < NEVENT_MAXEVENTS) {
		return(nevent.item + n);
	}
	else {
		/* error! */
		return(NEVENTITEM_TERM);
	}
}

static int flagsave_evt(STFLAGH sfh, const SFENTRY *tbl) {

	NEVTSAVE	nevt;
	UINT		i;

	for (i=0; i<NEVENT_MAXEVENTS; i++) {
		nevt.item[i].next = evt2id(nevent.item[i].next);
		nevt.item[i].clock = nevent.item[i].clock;
		nevt.item[i].baseclock = nevent.item[i].baseclock;
		nevt.item[i].proc = proc2id((INTPTR)nevent.item[i].proc,
												evtproc, NELEMENTS(evtproc));
	}
	nevt.first = evt2id(nevent.first);
	(void)tbl;
	return(statflag_write(sfh, &nevt, sizeof(nevt)));
}

static int flagload_evt(STFLAGH sfh, const SFENTRY *tbl) {

	NEVTSAVE	nevt;
	int			ret;
	UINT		i;

	ret = statflag_read(sfh, &nevt, sizeof(nevt));

	for (i=0; i<NEVENT_MAXEVENTS; i++) {
		nevent.item[i].next = id2evt(nevt.item[i].next);
		nevent.item[i].clock = nevt.item[i].clock;
		nevent.item[i].baseclock = nevt.item[i].baseclock;
		nevent.item[i].proc = (NEVENTCB)id2proc(nevt.item[i].proc,
												evtproc, NELEMENTS(evtproc));
	}
	nevent.first = id2evt(nevt.first);
	(void)tbl;
	return(ret);
}


/* disk */

typedef struct {
	OEMCHAR	fname[MAX_PATH];
	UINT32	ftype;
	UINT	readonly;
	DOSDATE	date;
	DOSTIME	time;
} STATDISK;

static const OEMCHAR str_fddx[] = OEMTEXT("FDD%u");
static const OEMCHAR str_updated[] = OEMTEXT("%s: updated");
static const OEMCHAR str_notfound[] = OEMTEXT("%s: not found");

static int disksave(STFLAGH sfh, const _FDDFILE *fdd) {

	STATDISK	st;
	FILEH		fh;

	ZeroMemory(&st, sizeof(st));
	if (fdd->type != DISKTYPE_NOTREADY) {
		file_cpyname(st.fname, fdd->fname, NELEMENTS(st.fname));
		st.ftype = fdd->ftype;
		st.readonly = fdd->protect;
		fh = file_open_rb(st.fname);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &st.date, &st.time);
			file_close(fh);
		}
	}
	return(statflag_write(sfh, &st, sizeof(st)));
}

static int flagsave_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	REG8	i;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<MAX_FDDFILE; i++) {
		ret |= disksave(sfh, fddfile + i);
	}
	(void)tbl;
	return(ret);
}

static int diskcheck(STFLAGH sfh, const OEMCHAR *name) {

	int			ret;
	FILEH		fh;
	STATDISK	st;
	OEMCHAR		buf[256];
	DOSDATE		date;
	DOSTIME		time;

	ret = statflag_read(sfh, &st, sizeof(st));
	if (st.fname[0]) {
		fh = file_open_rb(st.fname);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &date, &time);
			file_close(fh);
			if ((memcmp(&st.date, &date, sizeof(date))) ||
				(memcmp(&st.time, &time, sizeof(time)))) {
				ret |= STATFLAG_DISKCHG;
				OEMSPRINTF(buf, str_updated, name);
				statflag_seterr(sfh, buf);
			}
		}
		else {
			ret |= STATFLAG_DISKCHG;
			OEMSPRINTF(buf, str_notfound, name);
			statflag_seterr(sfh, buf);
		}
	}
	return(ret);
}

static int flagcheck_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	OEMCHAR	buf[8];

	ret = 0;
	for (i=0; i<MAX_FDDFILE; i++) {
		OEMSPRINTF(buf, str_fddx, i+1);
		ret |= diskcheck(sfh, buf);
	}
	(void)tbl;
	return(ret);
}

static int flagload_disk(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	UINT8		i;
	STATDISK	st;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<MAX_FDDFILE; i++) {
		ret |= statflag_read(sfh, &st, sizeof(st));
		if (st.fname[0]) {
			fddfile_set(i, st.fname, st.ftype, st.readonly);
		}
	}
	(void)tbl;
	return(ret);
}


/* checker */

static int flagcheck_versize(STFLAGH sfh, const SFENTRY *tbl) {

	if ((sfh->hdr.ver == tbl->ver) && (sfh->hdr.size == tbl->arg2)) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagcheck_veronly(STFLAGH sfh, const SFENTRY *tbl) {

	if (sfh->hdr.ver == tbl->ver) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}


/* interface */

int statsave_save(const OEMCHAR *filename) {

	SFFILEH		sffh;
	int			ret;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_create(filename);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	ret = STATFLAG_SUCCESS;
	tbl = xmiltbl;
	tblterm = tbl + (sizeof(xmiltbl)/sizeof(SFENTRY));
	while(tbl < tblterm) {
		ret |= statflag_createsection(sffh, tbl);
		switch(tbl->type) {
			case STATFLAG_BIN:
			case STATFLAG_TERM:
				ret |= flagsave_common(&sffh->sfh, tbl);
				break;

			case STATFLAG_DISK:
				ret |= flagsave_disk(&sffh->sfh, tbl);
				break;

			case STATFLAG_EVT:
				ret |= flagsave_evt(&sffh->sfh, tbl);
				break;
		}
		tbl++;
	}
	statflag_close(sffh);
	return(ret);
}

int statsave_check(const OEMCHAR *filename, OEMCHAR *buf, UINT size) {

	SFFILEH		sffh;
	int			ret;
	BRESULT		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, buf, size);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	done = FALSE;
	ret = STATFLAG_SUCCESS;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = xmiltbl;
		tblterm = tbl + (sizeof(xmiltbl)/sizeof(SFENTRY));
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, 10)) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
					ret |= flagcheck_versize(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

				case STATFLAG_EVT:
					ret |= flagcheck_veronly(&sffh->sfh, tbl);
					break;

				case STATFLAG_DISK:
					ret |= flagcheck_disk(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);
	return(ret);
}

int statsave_load(const OEMCHAR *filename) {

	SFFILEH		sffh;
	int			ret;
	BRESULT		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, NULL, 0);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	/* PCCORE read! */
	ret = statflag_readsection(sffh);
	if ((ret != STATFLAG_SUCCESS) ||
		(memcmp(sffh->sfh.hdr.index, xmiltbl[0].index, 10))) {
		statflag_close(sffh);
		return(STATFLAG_FAILURE);
	}

	soundmng_stop();
	ret |= flagload_common(&sffh->sfh, xmiltbl);

	sound_changeclock();
	sound_reset();

	Z80_RESET();
	nevent_allreset();
	ievent_reset();
	calendar_reset();
	iocore_reset();

	done = FALSE;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = xmiltbl + 1;
		tblterm = xmiltbl + (sizeof(xmiltbl)/sizeof(SFENTRY));
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, 10)) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
					ret |= flagload_common(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

				case STATFLAG_DISK:
					ret |= flagload_disk(&sffh->sfh, tbl);
					break;

				case STATFLAG_EVT:
					ret |= flagload_evt(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);

	memio_update();
	crtc_update();
	sndboard_update();

	pal_reset();
	makescrn_reset();

	soundmng_play();

	return(ret);
}

