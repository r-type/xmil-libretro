/*
 *  	File Name:		CarbonAboutBox.h
 *  	Description:		Automatically creates an Apple HI Guidelines-compliant About window.
 *	Format:			Mach-O link-in C procedural libraries.
 *	Author:			ExitToShell() Software <http://www.ExitToShell.com/>
 *      Copyright:		© 2004 by Tim Mityok dba ExitToShell() Software, all rights reserved.
 *	Version:		1.0
 *	Released:		January 17, 2003
 *	Changes:		Initial release.
 *
 */

#ifdef __APPLE_CC__
#include <Carbon/Carbon.h>
#else
#include <Carbon.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *    Library Setup Functions
 */

/*
 *    InitCarbonAboutBox()
 *    
 *    Description:
 *    
 *	Before an About window can be opened you must call this once.
 *	This is the only function call required to make the About window
 *	ready to be used. Any menu item with a command ID of 'abou' is
 *	all that is required to trigger an About window in your app.
 *        
 *    Result:
 *
 *	Any System-defined result codes may be returned if the
 *	internal structures could not be created for whatever reason.   
 */

OSStatus InitCarbonAboutBox();

#ifdef __cplusplus
}
#endif