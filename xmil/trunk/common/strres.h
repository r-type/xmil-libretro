
#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR str_null[];
extern const OEMCHAR str_space[];
extern const OEMCHAR str_dot[];

extern const OEMCHAR str_cr[];
extern const OEMCHAR str_crlf[];
#define	str_lf	(str_crlf + 1)

#if defined(OSLINEBREAK_CR)
#define	str_oscr	str_cr
#elif defined(OSLINEBREAK_CRLF)
#define	str_oscr	str_crlf
#else
#define	str_oscr	str_lf
#endif

extern const OEMCHAR str_ini[];
extern const OEMCHAR str_cfg[];
extern const OEMCHAR str_sav[];
extern const OEMCHAR str_bmp[];
extern const OEMCHAR str_e2d[];
extern const OEMCHAR str_d88[];
extern const OEMCHAR str_88d[];
extern const OEMCHAR str_dx1[];

extern const OEMCHAR str_d[];
extern const OEMCHAR str_u[];
extern const OEMCHAR str_x[];
extern const OEMCHAR str_2d[];
extern const OEMCHAR str_2x[];
extern const OEMCHAR str_4x[];
extern const OEMCHAR str_4X[];

extern const OEMCHAR str_false[];
extern const OEMCHAR str_true[];

extern const OEMCHAR str_posx[];
extern const OEMCHAR str_posy[];
extern const OEMCHAR str_width[];
extern const OEMCHAR str_height[];

extern const OEMCHAR str_xmil[];
extern const OEMCHAR str_resume[];

#ifdef __cplusplus
}
#endif

