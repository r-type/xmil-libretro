#include	"compiler.h"

#include	"romeo.h"
#include	"juliet.h"


enum {
	ROMEO_AVAIL			= 0x01,
	ROMEO_YMF288		= 0x02,
	ROMEO_YM2151		= 0x04
};

typedef struct {
	UINT8	op[8];
	UINT8	ttl[8*4];
} YM2151FLAG;

typedef struct {
	UINT8	algo[8];
	UINT8	ttl[8*4];
	UINT8	psgmix;
} YMF288FLAG;

typedef struct {
	HMODULE			mod;

	PCIFINDDEV		finddev;
	PCICFGREAD32	read32;
	PCIMEMWR8		out8;
	PCIMEMWR16		out16;
	PCIMEMWR32		out32;
	PCIMEMRD8		in8;
	PCIMEMRD16		in16;
	PCIMEMRD32		in32;

	ULONG			addr;
	ULONG			irq;
	ULONG			avail;
	ULONG			snoopcount;

	YM2151FLAG		ym2151;
	YMF288FLAG		ymf288;
} _ROMEO;


static const UINT8 opmask[] = {0x08,0x08,0x08,0x08,0x0c,0x0e,0x0e,0x0f};

typedef struct {
const char	*symbol;
	UINT	addr;
} DLLPROCESS;

static const char fnstr_finddev[] = FN_PCIFINDDEV;
static const char fnstr_read32[] = FN_PCICFGREAD32;
static const char fnstr_out8[] = FN_PCIMEMWR8;
static const char fnstr_out16[] = FN_PCIMEMWR16;
static const char fnstr_out32[] = FN_PCIMEMWR32;
static const char fnstr_inp8[] = FN_PCIMEMRD8;
static const char fnstr_inp16[] = FN_PCIMEMRD16;
static const char fnstr_inp32[] = FN_PCIMEMRD32;

static const DLLPROCESS	dllproc[] = {
				{fnstr_finddev,		offsetof(_ROMEO, finddev)},
				{fnstr_read32,		offsetof(_ROMEO, read32)},
				{fnstr_out8,		offsetof(_ROMEO, out8)},
				{fnstr_out16,		offsetof(_ROMEO, out16)},
				{fnstr_out32,		offsetof(_ROMEO, out32)},
				{fnstr_inp8,		offsetof(_ROMEO, in8)},
				{fnstr_inp16,		offsetof(_ROMEO, in16)},
				{fnstr_inp32,		offsetof(_ROMEO, in32)}};


static	_ROMEO	romeo;
static	SINT8	YM2151vol = -12;


BOOL juliet_load(void) {

	int				i;
const DLLPROCESS	*dp;
	BOOL			r = SUCCESS;

	juliet_unload();

	romeo.mod = LoadLibrary(OEMTEXT(PCIDEBUG_DLL));
	if (romeo.mod == NULL) {
		return(FAILURE);
	}
	for (i=0, dp=dllproc; i<sizeof(dllproc)/sizeof(DLLPROCESS); i++, dp++) {
		FARPROC proc;
		proc = GetProcAddress(romeo.mod, dp->symbol);
		if (proc == NULL) {
			r = FAILURE;
			break;
		}
		*(DWORD *)(((UINT8 *)&romeo) + dp->addr) = (DWORD)proc;
	}
	if (r) {
		juliet_unload();
	}
	return(r);
}

void juliet_unload(void) {

	if (romeo.mod) {
		FreeLibrary(romeo.mod);
	}
	ZeroMemory(&romeo, sizeof(romeo));
	FillMemory(romeo.ym2151.ttl, 8*4, 0x7f);
	FillMemory(romeo.ymf288.ttl, 8*4, 0x7f);
	romeo.ymf288.psgmix = 0x3f;
}


// ----

// pciFindPciDevice使うと、OS起動後一発目に見つけられないことが多いので
// 自前で検索する（矢野さん方式）

#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))
#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

static ULONG searchRomeo(void) {

	UINT	bus;
	UINT	dev;
	UINT	func;
	ULONG	addr;
	ULONG	dev_vend;

	for (bus=0; bus<0x100; bus++) {
		for (dev=0; dev<0x20; dev++) {
			for (func=0; func<0x08; func++) {
				addr = PCIBUSDEVFUNC(bus, dev, func);
				dev_vend = romeo.read32(addr, 0x0000);
				if ((dev_vend == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID)) ||
					(dev_vend == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID2))) {
					return(addr);
				}
			}
		}
	}
	return((ULONG)-1);
}

BRESULT juliet_prepare(void) {

	ULONG	pciaddr;

	if (romeo.mod == NULL) {
		return(FAILURE);
	}

	pciaddr = searchRomeo();
	if (pciaddr != (ULONG)-1) {
		romeo.addr = romeo.read32(pciaddr, ROMEO_BASEADDRESS1);
		romeo.irq = romeo.read32(pciaddr, ROMEO_PCIINTERRUPT) & 0xff;
		if (romeo.addr) {
			romeo.avail = ROMEO_AVAIL | ROMEO_YMF288;
			juliet_YM2151Reset();
			juliet_YMF288Reset();
		}
		return(SUCCESS);
	}
	return(FAILURE);
}


// ---- YM2151部

static void YM2151W(UINT8 addr, UINT8 data) {

	// 書き込み直後だと、ROMEOチップでの遅延のため、まだ書き込みが起こっていない（＝Busyが
	// 立っていない）可能性がある。ので、Snoopカウンタで書き込み発生を見張る
	while ( romeo.snoopcount==romeo.in32(romeo.addr + ROMEO_SNOOPCTRL) ) Sleep(0);
	romeo.snoopcount = romeo.in32(romeo.addr + ROMEO_SNOOPCTRL);

	// カウンタ増えた時点ではまだBusyの可能性があるので、OPMのBusyも見張る
	while ( romeo.in8(romeo.addr + ROMEO_YM2151DATA)&0x80 ) Sleep(0);

	romeo.out8(romeo.addr + ROMEO_YM2151ADDR, addr);
	romeo.in8(romeo.addr + ROMEO_YM2151DATA);
	romeo.out8(romeo.addr + ROMEO_YM2151DATA, data);
}

static void YM2151setvolume(UINT8 ch, int vol) {

	UINT8	mask;
	int		ttl;

	ch &= 7;
	mask = romeo.ym2151.op[ch];
	do {
		if (mask & 1) {
			ttl = romeo.ym2151.ttl[ch] & 0x7f;
			ttl -= vol;
			if (ttl < 0) {
				ttl = 0;
			}
			else if (ttl > 0x7f) {
				ttl = 0x7f;
			}
			YM2151W(ch + 0x60, (UINT8)ttl);
		}
		ch += 0x08;
		mask >>= 1;
	} while(mask);
}


// リセットと同時に、OPMチップの有無も確認
void juliet_YM2151Reset(void) {

	BYTE	flag;

	if (romeo.avail & ROMEO_AVAIL) {
		juliet_YM2151Enable(FALSE);
		romeo.out32(romeo.addr + ROMEO_YM2151CTRL, 0x00);
		Sleep(10);					// 44.1kHz x 192 clk = 4.35ms 以上ないと、DACのリセットかからない
		flag = romeo.in8(romeo.addr + ROMEO_YM2151DATA) + 1;
		romeo.out32(romeo.addr + ROMEO_YM2151CTRL, 0x80);
		Sleep(10);					// リセット解除後、一応安定するまでちょっと待つ
		flag |= romeo.in8(romeo.addr + ROMEO_YM2151DATA);
		if ( !flag ) {					// flag!=0 だと OPM チップがない
			romeo.avail |= ROMEO_YM2151;
			// Busy検出用にSnoopカウンタを使う
			romeo.out32(romeo.addr + ROMEO_SNOOPCTRL, (unsigned int)0x80000000);
			romeo.snoopcount = 0xffffffff;
		}
	}
}

BOOL juliet_YM2151IsEnable(void) {

	return((romeo.avail & ROMEO_YM2151) != 0);
}

BOOL juliet_YM2151IsBusy(void) {

	BOOL	ret;

	ret = FALSE;
	if (romeo.avail & ROMEO_YM2151) {
		if ((romeo.snoopcount == romeo.in32(romeo.addr + ROMEO_SNOOPCTRL)) ||
			(romeo.in8(romeo.addr + ROMEO_YM2151DATA) & 0x80)) {
			ret = TRUE;
		}
	}
	return(ret);
}

void juliet_YM2151W(UINT8 addr, UINT8 data) {

	UINT	ch;
	int		ttl;
	UINT8	mask;

	if (romeo.avail & ROMEO_YM2151) {
		if ((addr & 0xe0) == 0x60) {				// ttl
			romeo.ym2151.ttl[addr & 0x1f] = data;
			if (romeo.ym2151.op[addr & 7] & (1 << ((addr >> 3) & 3))) {
				ttl = (data & 0x7f) - YM2151vol;
				if (ttl < 0) {
					ttl = 0;
				}
				else if (ttl > 0x7f) {
					ttl = 0x7f;
				}
				data = (UINT8)ttl;
			}
		}
		else if ((addr & 0xf8) == 0x20) {				// algorithm
			ch = addr & 7;
			mask = opmask[ch];
			if (romeo.ym2151.op[ch] != mask) {
				romeo.ym2151.op[ch] = mask;
				YM2151setvolume((UINT8)ch, YM2151vol);
			}
		}
		YM2151W(addr, data);
	}
}

void juliet_YM2151Enable(BOOL enable) {

	UINT8	ch;
	int		vol;

	if (romeo.avail & ROMEO_YM2151) {
		vol = (enable)?YM2151vol:-127;
		for (ch=0; ch<8; ch++) {
			YM2151setvolume(ch, vol);
		}
	}
}


// ---- YMF288部

static void YMF288W(UINT8 a1, UINT8 addr, UINT8 data) {

	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + (a1?ROMEO_YMF288ADDR2:ROMEO_YMF288ADDR1), addr);
	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + (a1?ROMEO_YMF288DATA2:ROMEO_YMF288DATA1), data);
}

static void YMF288setvolume(UINT ch, int vol) {

	UINT8	a1;
	UINT8	mask;
const UINT8	*pttl;
	int		ttl;

	a1 = ch & 4;
	mask = opmask[romeo.ymf288.algo[ch & 7] & 7];
	pttl = romeo.ymf288.ttl + ((ch & 4) << 2);
	ch = 0x40 + (ch & 3);
	do {
		if (mask & 1) {
			ttl = pttl[ch & 0x0f] & 0x7f;
			ttl -= vol;
			if (ttl < 0) {
				ttl = 0;
			}
			else if (ttl > 0x7f) {
				ttl = 0x7f;
			}
			YMF288W(a1, ch, (UINT8)ttl);
		}
		ch += 0x04;
		mask >>= 1;
	} while(mask);
}


void juliet_YMF288Reset(void) {

	if (romeo.avail & ROMEO_YMF288) {
		juliet_YMF288Enable(FALSE);
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x00);
		Sleep(150);
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x80);
		Sleep(150);
	}
}

BOOL juliet_YMF288IsEnable(void) {

	return((romeo.avail & ROMEO_YMF288) != 0);
}

BOOL juliet_YMF288IsBusy(void) {

	return((!(romeo.avail & ROMEO_YMF288)) ||
			((romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) != 0));
}

void juliet_YMF288A(UINT8 addr, UINT8 data) {

	if (romeo.avail & ROMEO_YMF288) {
		if (addr == 0x07) {							// psg mix
			romeo.ymf288.psgmix = data;
		}
		else if ((addr & 0xf0) == 0x40) {			// ttl
			romeo.ymf288.ttl[addr & 0x0f] = data;
		}
		else if ((addr & 0xfc) == 0xb0) {			// algorithm
			romeo.ymf288.algo[addr & 3] = data;
		}
		YMF288W(0, addr, data);
	}
}

void juliet_YMF288B(UINT8 addr, UINT8 data) {

	if (romeo.avail & ROMEO_YMF288) {
		if ((addr & 0xf0) == 0x40) {				// ttl
			romeo.ymf288.ttl[0x10 + (addr & 0x0f)] = data;
		}
		else if ((addr & 0xfc) == 0xb0) {			// algorithm
			romeo.ymf288.algo[4 + (addr & 3)] = data;
		}
		YMF288W(1, addr, data);
	}
}

void juliet_YMF288Enable(BOOL enable) {

	UINT8	ch;
	int		vol;

	if (romeo.avail & ROMEO_YMF288) {
		YMF288W(0, 0x07, (enable)?romeo.ymf288.psgmix:0x3f);
		vol = (enable)?0:-127;
		for (ch=0; ch<3; ch++) {
			YMF288setvolume(ch + 0, vol);
			YMF288setvolume(ch + 4, vol);
		}
	}
}


#if 0
// ---- delay...

#define	FM_BUFBIT		10
#define	FM_BUFFERS		(1 << FM_BUFBIT)

typedef struct {
	DWORD	clock;
	DWORD	data;
} FMRINGDATA;

typedef struct {
	DWORD		base;
	DWORD		pos;
	DWORD		avail;
	DWORD		maxclock;
	FMRINGDATA	datas[FM_BUFFERS];
} FMRING;

static	FMRING	fmr;
#ifdef __cplusplus
extern "C" {
#endif
		DWORD	basedclk = 0;
#ifdef __cplusplus
}
#endif

static void (*sendfmfn[3])(BYTE addr, BYTE data) =
							{juliet_YM2151W, juliet_YMF288A, juliet_YMF288B};

LABEL static void fmr_release(void) {

	__asm {
				dec		fmr.avail
				mov		edx, fmr.pos
				mov		eax, fmr.datas.data[edx * 8]
				inc		edx
				and		edx, (FM_BUFFERS - 1);
				mov		fmr.pos, edx
				push	eax
				shr		eax, 8
				push	eax
				shr		eax, 8
				call	sendfmfn[eax*4]
				add		esp, 8
				ret
	}
}

LABEL static void fmr_send(DWORD clock, DWORD data) {

	__asm {
				mov		ecx, fmr.avail
				cmp		ecx, FM_BUFFERS
				jne		short fmdatasend
				call	fmr_release
				dec		ecx
fmdatasend:		add		ecx, fmr.pos
				and		ecx, (FM_BUFFERS - 1);
				mov		eax, fmr.base
				add		eax, [esp + 4]
				mov		fmr.datas.clock[ecx * 8], eax
				mov		eax, [esp + 8]
				mov		fmr.datas.data[ecx * 8], eax
				inc		fmr.avail
				ret
	}
}

#define	_CPUID	__asm _emit 00fh __asm _emit 0a2h
#define	_RDTSC	__asm _emit 00fh __asm _emit 031h
#define	RDTSC_SFT	6

LABEL void juliet2_reset(void) {

	__asm {
				push	ebx
				push	esi
				push	edi
				mov		ecx, (type fmr) / 4
				xor		eax, eax
				mov		edi, offset fmr
				rep stosd

				pushfd
				pop		edx
				mov		edi, edx
				xor		edx, 00200000h
				push	edx
				popfd
				pushfd
				pop		edx
				cmp		edi, edx
				je		short jul2reset_exit
				inc		eax
				_CPUID
				xor		eax, eax
				test	edx, 10h
				je		short jul2reset_exit

				push	100
				mov		edi, dword ptr Sleep
				_CPUID
				_RDTSC
				mov		esi, edx
				xchg	edi, eax
				call	eax
				_CPUID
				_RDTSC
				sub		eax, edi
				sbb		edx, esi
				shrd	eax, edx, RDTSC_SFT
				mov		fmr.maxclock, eax

jul2reset_exit:	xor		eax, eax
				mov		basedclk, eax
				pop		edi
				pop		esi
				pop		ebx
				ret
	}
}

LABEL void juliet2_sync(DWORD delaytick) {

	__asm {
				_RDTSC
				shrd	eax, edx, RDTSC_SFT
				mov		fmr.base, eax

				mov		eax, [esp+4]
				mul		basedclk
				mov		ecx, 100
				div		ecx
				add		fmr.base, eax
				ret
	}
}

LABEL void juliet2_exec(void) {

	__asm {
				cmp		fmr.avail, 0
				je		jul2exe_exit
jul2exe:		_RDTSC
				shrd	eax, edx, RDTSC_SFT
				neg		eax
				mov		edx, fmr.pos
				add		eax, fmr.datas[edx * 8].clock
				cmp		eax, fmr.maxclock
				jb		jul2exe_exit
				call	fmr_release
				cmp		fmr.avail, 0
				jne		short jul2exe
jul2exe_exit:	ret
	}
}

void juliet2_YM2151W(BYTE addr, BYTE data, DWORD clock) {

	if (romeo.avail & ROMEO_YM2151) {
		if (basedclk) {
			fmr_send(clock, (addr << 8) | data);
		}
		else {
			juliet_YM2151W(addr, data);
		}
	}
}

void juliet2_YMF288A(BYTE addr, BYTE data, DWORD clock) {

	if (romeo.avail & ROMEO_YMF288) {
		if (basedclk) {
			fmr_send(clock, 0x10000 | (addr << 8) | data);
		}
		else {
			juliet_YMF288A(addr, data);
		}
	}
}

void juliet2_YMF288B(BYTE addr, BYTE data, DWORD clock) {

	if (romeo.avail & ROMEO_YMF288) {
		if (basedclk) {
			fmr_send(clock, 0x20000 | (addr << 8) | data);
		}
		else {
			juliet_YMF288B(addr, data);
		}
	}
}
#endif

