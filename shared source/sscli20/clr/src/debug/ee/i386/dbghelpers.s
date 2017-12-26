// ==++==
// 
//  
//   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//  
//   The use and distribution terms for this software are contained in the file
//   named license.txt, which can be found in the root of this distribution.
//   By using this software in any fashion, you are agreeing to be bound by the
//   terms of this license.
//  
//   You must not remove this notice, or any other, from this software.
//  
// 
// ==--== 
//
//  *** NOTE:  If you make changes to this file, propagate the changes to
//             dbghelpers.asm in this directory
//
//  This file uses AT&T i386 syntax. Search web for "AT&T Syntax versus Intel Syntax" 
//  to get document describing differences between the AT&T i386 syntax and the Intel 
//  syntax used by masm.

.text

#ifdef PLATFORM_UNIX
#define STDMANGLE(name,args) name
#else
#define STDMANGLE(name,args) _##name##@##args
#endif

#define ASMFUNC(name)             \
        .globl name             ; \
name: ;

#define ASMFUNCEND()
	
//
// This is the method that we hijack a thread running managed code. It calls
// FuncEvalHijackWorker, which actually performs the func eval, then jumps to 
// the patch address so we can complete the cleanup.
//
// Note: the parameter is passed in eax - see Debugger::FuncEvalSetup for
//       details
//
ASMFUNC(STDMANGLE(FuncEvalHijack,0))
        push %eax       // the ptr to the DebuggerEval
        call STDMANGLE(FuncEvalHijackWorker,4)
        jmp  *%eax      // return is the patch addresss to jmp to
ASMFUNCEND()
