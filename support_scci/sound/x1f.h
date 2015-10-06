
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(SUPPORT_X1F)		/* ÉRÅ[ÉãÇ∑ÇÁñ ì|ÇæÅI */

#define	x1f_open(f)			(FAILURE)
#define	x1f_close()
#define	x1f_opm(r, d)
#define	x1f_psg(r, d)
#define x1f_sync()

#else

BRESULT x1f_open(const OEMCHAR *filename);
void x1f_close(void);
void x1f_opm(REG8 reg, REG8 dat);
void x1f_psg(REG8 reg, REG8 dat);
void x1f_sync(void);

#endif

#ifdef __cplusplus
}
#endif

