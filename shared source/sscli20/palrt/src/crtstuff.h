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
// ===========================================================================
// File: crtstuff.h
// 
// ===========================================================================

/*** 
*
*Purpose:
*  Misc C Runtime style helper functions.
*
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

INTERNAL_(HRESULT) DispAlloc(size_t cb, void FAR* FAR* ppv);

INTERNAL_(void) DispFree(void FAR* pv);

INTERNAL_(OLECHAR FAR*) disp_itoa(int val, OLECHAR FAR* buf);

INTERNAL_(OLECHAR FAR*) disp_ltoa(long val, OLECHAR FAR* buf);

INTERNAL_(OLECHAR FAR*) disp_ultoa(unsigned long val, OLECHAR FAR* buf);

INTERNAL_(OLECHAR FAR*) disp_i64toa(LONG64 val, OLECHAR FAR* buf);

INTERNAL_(OLECHAR FAR*) disp_ui64toa(ULONG64 val, OLECHAR FAR* buf);

INTERNAL_(void) disp_gcvt(double dblIn, int ndigits, OLECHAR FAR* pchOut, int bufSize);

#ifdef __cplusplus
}
#endif

