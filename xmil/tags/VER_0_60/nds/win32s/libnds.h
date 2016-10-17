
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;

#include "ndsreg.h"
#include "ndsmem.h"
#include "ndssys.h"
#include "ndsirq.h"
#include "ndsswi.h"
#include "ndsdma.h"
#include "ndsvideo.h"
#include "ndssound.h"
#include "ndsinput.h"
#include "ndspower.h"
#include "ndsconsole.h"
#include "ndsipc.h"
#include "ndstouch.h"

#if defined(__cplusplus)
extern "C"
{
#endif	/* defined(__cplusplus) */

int nds9main();
int nds7main();

#if defined(__cplusplus)
}
#endif	/* defined(__cplusplus) */

