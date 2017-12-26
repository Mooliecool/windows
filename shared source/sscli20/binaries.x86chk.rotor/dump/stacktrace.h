// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
//----------------------------------------------------------------------------- 
//----------------------------------------------------------------------------- 
#ifndef __STACK_TRACE_H__
#define __STACK_TRACE_H__

HINSTANCE LoadImageHlp();
HINSTANCE LoadDbgHelp();

//#include <windows.h>

#include <specstrings.h>

#define strupr _strupr

//
//--- Constants ---------------------------------------------------------------
//

#define cchMaxAssertModuleLen 12
#define cchMaxAssertSymbolLen 257
#define cfrMaxAssertStackLevels 20
#define cchMaxAssertExprLen 257

#define cchMaxAssertStackLevelStringLen \
    ((2 * 8) + cchMaxAssertModuleLen + cchMaxAssertSymbolLen + 12)
    // 2 addresses of at most 8 char, module, symbol, and the extra chars:
    // 0x<address>: <module>! <symbol> + 0x<offset>\n

//
//--- Prototypes --------------------------------------------------------------
//

/****************************************************************************
* MagicDeinit *
*-------------*
*   Description:  
*       Cleans up for the symbol loading code. Should be called before
*       exiting in order to free the dynamically loaded imagehlp.dll
******************************************************************** robch */
void MagicDeinit(void);

/****************************************************************************
* GetStringFromStackLevels *
*--------------------------*
*   Description:  
*       Retrieves a string from the stack frame. If more than one frame, they
*       are separated by newlines. Each fram appears in this format:
*
*           0x<address>: <module>! <symbol> + 0x<offset>
******************************************************************** robch */
void GetStringFromStackLevels(UINT ifrStart, UINT cfrTotal, __out_ecount(cchMaxAssertStackLevelStringLen * cfrTotal) CHAR *pszString, CONTEXT * pContext = NULL);


/****************************************************************************
* GetAddrFromStackLevel *
*-----------------------*
*   Description:  
*       Retrieves the address of the next instruction to be executed on a
*       particular stack frame.
*
*   Return:
*       The address as a DWORD.
******************************************************************** robch */
DWORD_PTR GetAddrFromStackLevel(UINT ifrStart);

/****************************************************************************
* GetStringFromAddr *
*-------------------*
*   Description:  
*       Builds a string from an address in the format:
*
*           0x<address>: <module>! <symbol> + 0x<offset>
******************************************************************** robch */
void GetStringFromAddr(DWORD_PTR dwAddr, __out_ecount(cchMaxAssertStackLevelStringLen) LPSTR szString);

#endif
