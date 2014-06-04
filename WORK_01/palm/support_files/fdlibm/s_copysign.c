
/* @(#)s_copysign.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * copysign(double x, double y)
 * copysign(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include "fdlibm.h"

#ifdef __STDC__
	double copysign(double x, double y)
#else
	double copysign(x,y)
	double x,y;
#endif
{
#ifndef PALMOS
	__HI(x) = (__HI(x)&0x7fffffff)|(__HI(y)&0x80000000);
#else
	double4s di, di2;
	di.d = x;di2.d = y;di.i.h = (di.i.h&0x7fffffff)|(di2.i.h&0x80000000);x = di.d;
#endif
        return x;
}
