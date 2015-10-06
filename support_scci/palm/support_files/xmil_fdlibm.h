/*
 *  xmil_fdlibm.h
 *
 *  Created by tk800 on 05/02/08.
 *
 */

#define PALMOS
#define _IEEE_LIBM

typedef struct {
	union {
		double d;
		struct {
			unsigned int h, l;
		} i;
	};
} double4s;

