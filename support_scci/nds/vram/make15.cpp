#include "compiler.h"
#include "libnds.h"
#include "pccore.h"
#include "vram.h"
#include "makescrn.h"
#include "makesub.h"
#include "font.h"


// ---- 25lines

// pos_cy
// fedcba9876543210fedcba9876543210
// ppppppppppp00???????????yyyyyyyy

#if !defined(USE_ARMROUTINE)
void width40x25_200l(UINT _pos) {							// 40x25 200line

	UINT	pos_cy;
	UINT8	*dst;
	UINT	y;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_ROW * 3];

	pos_cy = _pos << 21;
	pos_cy += min(makescrn.fontcy, MAKETEXT_ROW);
	dst = reinterpret_cast<UINT8 *>(BG_GFX);
	y = makescrn.surfcy;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos_cy >> 21);
			dirty = (udtmp & makescrn.dispflag);
			if (dirty) {
				TRAMUPDATE(pos_cy >> 21) = (UINT8)(udtmp ^ dirty);
				if (dirty & UPDATE_TRAM) {
					makechr8(work, &makescrn, pos_cy, udtmp);
					makemix_mixtext40(dst, work, pos_cy & 0xff);
				}
				if (dirty & UPDATE_VRAM) {
					makemix_mixgrph40(dst,
									makescrn.disp1 + TRAM2GRAM(pos_cy >> 21),
									pos_cy & 0xff);
				}
			}
			pos_cy += (1 << 21);
			dst += 8;
		} while(--x);

		if (udtmp & 4) {								// ècî{äpï`âÊÇæÇ¡ÇΩÇÁ
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		pos_cy += makescrn.surfrx << 21;
	} while(--y);
}

void width80x25_200l(UINT _pos) {							// 80x25 200line

	UINT32	pos_cy;
	UINT8	*dst;
	UINT	y;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_ROW * 3];

	pos_cy = _pos << 21;
	pos_cy += min(makescrn.fontcy, MAKETEXT_ROW);
	dst = reinterpret_cast<UINT8 *>(BG_GFX);
	y = makescrn.surfcy;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos_cy >> 21);
			dirty = (udtmp & makescrn.dispflag);
			if (dirty) {
				TRAMUPDATE(pos_cy >> 21) = (UINT8)(udtmp ^ dirty);
				if (dirty & UPDATE_TRAM) {
					makechr8(work, &makescrn, pos_cy, udtmp);
					makemix_mixtext80(dst, work, pos_cy & 0xff);
				}
				if (dirty & UPDATE_VRAM) {
					makemix_mixgrph80(dst,
									makescrn.disp1 + TRAM2GRAM(pos_cy >> 21),
									pos_cy & 0xff);
				}
			}
			pos_cy += (1 << 21);
			dst += 4;
		} while(--x);

		if (udtmp & 4) {								// ècî{äpï`âÊÇæÇ¡ÇΩÇÁ
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		pos_cy += makescrn.surfrx << 21;
	} while(--y);
}
#endif	// !defined(USE_ARMROUTINE)


// ---- 12lines

void width40x12_200l(UINT pos) {							// 40x12 200line

	UINT	fontcy;
	UINT8	*dst;
	UINT	y;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	dst = reinterpret_cast<UINT8 *>(BG_GFX);
	y = makescrn.surfcy;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			dirty = (udtmp & makescrn.dispflag);
			if (dirty) {
				TRAMUPDATE(pos) = (UINT8)(udtmp ^ dirty);
				if (dirty & UPDATE_TRAM) {
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext40(dst, work, fontcy);
					makemix_mixtext40(dst + MAKESCRN_VIDEOYALIGN,
												work + MAKETEXT_STEP, fontcy);
				}
				if (dirty & UPDATE_VRAM) {
					src = makescrn.disp1 + TRAM2GRAM(LOW10(pos));
					makemix_mixgrph40(dst, src, fontcy);
					makemix_mixgrph40(dst + MAKESCRN_VIDEOYALIGN,
												src + GRAM_HALFSTEP, fontcy);
				}
			}
			pos = LOW11(pos + 1);
			dst += 8;
		} while(--x);

		if (udtmp & 4) {							// ècî{äpï`âÊÇæÇ¡ÇΩÇÁ
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		pos = LOW11(pos + makescrn.surfrx);
	} while(--y);
}

void width80x12_200l(UINT pos) {							// 80x12 200line

	UINT	fontcy;
	UINT8	*dst;
	UINT	y;
	UINT	x;
	REG8	udtmp;
	REG8	dirty;
	UINT8	work[MAKETEXT_STEP * 2];
const UINT8	*src;

	fontcy = min(makescrn.fontcy, MAKETEXT_ROW);
	dst = reinterpret_cast<UINT8 *>(BG_GFX);
	y = makescrn.surfcy;
	do {
		x = makescrn.surfcx;
		do {
			udtmp = TRAMUPDATE(pos);
			dirty = (udtmp & makescrn.dispflag);
			if (dirty) {
				TRAMUPDATE(pos) = (UINT8)(udtmp ^ dirty);
				if (dirty & UPDATE_TRAM) {
					makechr16(work, pos, fontcy, udtmp);
					makemix_mixtext80(dst, work, fontcy);
					makemix_mixtext80(dst + MAKESCRN_VIDEOYALIGN,
												work + MAKETEXT_STEP, fontcy);
				}
				if (dirty & UPDATE_VRAM) {
					src = makescrn.disp1 + TRAM2GRAM(LOW10(pos));
					makemix_mixgrph80(dst, src, fontcy);
					makemix_mixgrph80(dst + MAKESCRN_VIDEOYALIGN,
												src + GRAM_HALFSTEP, fontcy);
				}
			}
			pos = LOW11(pos + 1);
			dst += 4;
		} while(--x);

		if (udtmp & 4) {							// ècî{äpï`âÊÇæÇ¡ÇΩÇÁ
			makescrn.fontycnt += makescrn.charcy;
		}
		else {
			makescrn.fontycnt = makescrn.charcy * 2;
		}
		makescrn.fontycnt &= 15;
		dst += makescrn.surfstep;
		pos = LOW11(pos + makescrn.surfrx);
	} while(--y);
}

