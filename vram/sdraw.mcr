
static void SCRNCALL SDSYM(x1)(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			for (x=0; x<sdraw->width; x++) {
				SDSETPIXEL(q, p[x]);
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH;
		q += sdraw->yalign;
	} while(++y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

static void SCRNCALL SDSYM(x2)(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		xmax;
	int		x;
	UINT	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			xmax = sdraw->width / 2;
			for (x=0; x<xmax; x++) {
				c = p[x];
				SDSETPIXEL(q, c);
				q += sdraw->xalign;
				SDSETPIXEL(q, c);
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH;
		q += sdraw->yalign;
	} while(++y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

#if defined(SUPPORT_TURBOZ) && defined(SDZMODE)
static void SCRNCALL SDSYM(z)(SDRAW sdraw, int maxy) {

const UINT8	*p;
	UINT8	*q;
	int		y;
	int		xmax;
	int		x;
	UINT	c;

	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (sdraw->dirty[y]) {
			xmax = sdraw->width / 2;
			for (x=0; x<xmax; x++) {
				c = (p[x + 320] << 8) + p[x] + XMILPAL_4096G;
				SDSETPIXEL(q, c);
				q += sdraw->xalign;
				SDSETPIXEL(q, c);
				q += sdraw->xalign;
			}
			q -= sdraw->xbytes;
		}
		p += SURFACE_WIDTH;
		q += sdraw->yalign;
	} while(++y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}
#endif


// ----

static const SDRAWFN SDSYM(p)[] = {
		SDSYM(x1),		SDSYM(x2),
#if defined(SUPPORT_TURBOZ)
#if defined(SDZMODE)
		SDSYM(z)
#else
		NULL
#endif
#endif
	};

