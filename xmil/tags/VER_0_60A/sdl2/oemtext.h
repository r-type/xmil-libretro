/**
 * @file	oemtext.h
 * @breif	defines converter
 */

#pragma once

#include "codecnv/codecnv.h"

#define	oemtext_oem2sjis(a, b, c, d)	codecnv_utf8tosjis(a, b, c, d)
#define	oemtext_sjis2oem(a, b, c, d)	codecnv_sjistoutf8(a, b, c, d)

#ifdef __cplusplus
#include <string>
namespace std
{
	typedef string oemstring;
}
#endif	/* __cplusplus */
