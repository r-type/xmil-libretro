#include	"compiler.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"palettes.h"


#if defined(SUPPORT_8BPP)
#define	SDSYM(sym)				sdraw8##sym
#define	SDSETPIXEL(ptr, pal)	*(ptr) = x1n_pal32[(pal)].p.e
#include	"sdraw.mcr"
#undef	SDSYM
#undef	SDSETPIXEL
#endif

#if defined(SUPPORT_16BPP)
#define	SDSYM(sym)				sdraw16##sym
#define	SDSETPIXEL(ptr, pal)	*(UINT16 *)(ptr) = x1n_pal16[(pal)]
#define	SDSETZPIXEL(ptr, pal)	*(UINT16 *)(ptr) = x1z_pal16[(pal)]
#include	"sdraw.mcr"
#undef	SDSYM
#undef	SDSETPIXEL
#undef	SDSETZPIXEL
#endif

#if defined(SUPPORT_24BPP)
#define	SDSYM(sym)				sdraw24##sym
#define	SDSETPIXEL(ptr, pal)	(ptr)[RGB24_R] = x1n_pal32[(pal)].p.r;	\
								(ptr)[RGB24_G] = x1n_pal32[(pal)].p.g;	\
								(ptr)[RGB24_B] = x1n_pal32[(pal)].p.b
#define	SDSETZPIXEL(ptr, pal)	(ptr)[RGB24_R] = x1z_pal32[(pal)].p.r;	\
								(ptr)[RGB24_G] = x1z_pal32[(pal)].p.g;	\
								(ptr)[RGB24_B] = x1z_pal32[(pal)].p.b
#include	"sdraw.mcr"
#undef	SDSYM
#undef	SDSETPIXEL
#undef	SDSETZPIXEL
#endif

#if defined(SUPPORT_32BPP)
#define	SDSYM(sym)				sdraw32##sym
#define	SDSETPIXEL(ptr, pal)	*(UINT32 *)(ptr) = x1n_pal32[(pal)].d
#define	SDSETZPIXEL(ptr, pal)	*(UINT32 *)(ptr) = x1z_pal32[(pal)].d
#include	"sdraw.mcr"
#undef	SDSYM
#undef	SDSETPIXEL
#undef	SDSETZPIXEL
#endif


// ----

static const SDRAWFN *tbl[4] = {
#if defined(SUPPORT_8BPP)
			sdraw8p,
#else
			NULL,
#endif
#if defined(SUPPORT_16BPP)
			sdraw16p,
#else
			NULL,
#endif
#if defined(SUPPORT_24BPP)
			sdraw24p,
#else
			NULL,
#endif
#if defined(SUPPORT_32BPP)
			sdraw32p,
#else
			NULL,
#endif
};

const SDRAWFN *sdraw_getproctbl(const SCRNSURF *surf) {

	int		proc;

	proc = ((surf->bpp >> 3) - 1) & 3;
	return(tbl[proc]);
}

