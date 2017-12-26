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
//
// Desription:
//
// This header file contain core defines for each platform: describing 
// calling convention, register size availability and mapping, primitive
// type sizes.
//

#ifndef _FJITCORE_H_
#define _FJITCORE_H_

/***************************************************************************
   Inline trival bools to avoid PREFast Errors. Pretty cheesy way to do this.
   Ideally, PREFast would have a seperate warning when conditional macros
   cause an expression to always be false
***************************************************************************/

#ifndef __TRIVIALINLINEBOOL__
#define __TRIVIALINLINEBOOL__

inline bool returnTrue() { return true; } 
inline bool returnFalse() { return false; } 

#endif // #ifndef __TRIVIALINLINEBOOL__

/***************************************************************************
  The following macros must be defined for each chip.
  These describe platform calling conventions
***************************************************************************/

#if defined(_X86_) 

#define ON_X86_ONLY(exp)  exp
#define ON_PPC_ONLY(exp)
#define ON_SPARC_ONLY(exp)
#define EnregThis         true      // Is 'this' pointer eligble for being passed in a register
#define EnregArgumentsGP returnTrue()      // Should eligble arguments be enregistered in general purpose registers
#define EnregArgumentsFP returnFalse()     // Should eligble arguments be enregistered in floating point registers
#define MAX_GP_ARG_REGISTER 2       // Number of general purpose registers available for passing arguments
#define MAX_FP_ARG_REGISTER 0       // Number of floating point registers available for passing arguments
#define PARAMETER_SPACE   returnFalse()     // Should IL stack space not be recycled for passing arguments  
#define FIXED_ENREG_BUFFER 0        // The parameter space for enregistered args is not fixed size 

#define ARGS_RIGHT_TO_LEFT   returnTrue()
#define STACK_BUFFER         0      // The SP leads the top of the evaluation stack by this amount
#define HASZEROREG           false  // The architecture provides a register that always reads zero
#define M_RETBUFF_CALLSITE   false  // The callsites to functions which use the return buffer are padded with an extra nop
#define ALIGN_LOCALS returnFalse()  // Locals should be aligned
#define ALIGN_ARGS returnFalse()  // Arguments should be aligned
#define HASTOCREG            returnFalse() // Architecture has a TOC register

#define PushEnregArgs        false
#define CALLER_CLEANS_STACK  false

// Specifies which argument register encodings correspond to numbers
static const __int8 argRegistersMap[] = {
             1, // ECX
             2, // EDX
        };

static const __int8 typeSizeMap[] = {
            -1,     //CORINFO_TYPE_UNDEF
            0,      //CORINFO_TYPE_VOID
            4,      //CORINFO_TYPE_BOOL
            4,      //CORINFO_TYPE_CHAR
            4,      //CORINFO_TYPE_BYTE
            4,      //CORINFO_TYPE_UBYTE
            4,      //CORINFO_TYPE_SHORT
            4,      //CORINFO_TYPE_USHORT
            4,      //CORINFO_TYPE_INT
            4,      //CORINFO_TYPE_UINT
            8,      //CORINFO_TYPE_LONG
            8,      //CORINFO_TYPE_ULONG
            4,      //CORINFO_TYPE_NATIVEINT
            4,      //CORINFO_TYPE_NATIVEUINT
            4,      //CORINFO_TYPE_FLOAT
            8,      //CORINFO_TYPE_DOUBLE
            4,      //CORINFO_TYPE_STRING
            4,      //CORINFO_TYPE_PTR
            4,      //CORINFO_TYPE_BYREF
            -1,     //CORINFO_TYPE_VALUECLASS
            4,      //CORINFO_TYPE_CLASS
            8,      //CORINFO_TYPE_REFANY
            // CORINFO_TYPE_VAR only appears in "import-only" mode, but
            // the Fjit generates code even in "import-only" mode -- it
            // just throws the code away afterwards.  So we provide a
            // dummy value for CORINFO_TYPE_VAR.           
            4,      //CORINFO_TYPE_VAR
        };

inline unsigned int typeSizeFromJitType( unsigned type, bool precise )
{
  if ( precise && ( type == 3 /*typeU1*/ || type == 5 /*typeI1*/) )
    return 1;
  else if ( precise && ( type == 4 /*typeU2*/ || type == 6 /*typeI2*/) )
    return 2;
  else  if ( type <= 7 || type == 9 /*typeR4*/ || type == 13 /*typeMethod */)
    return 4;
  else if ( type == 8 /*typeI8*/ || type == 10 /*typeR8*/ || type == 11)
    return 8;
  else
    return  0;
}

// Specifies which types maybe enregistred in either general purpose or floating point registers
static const bool typeEnregisterMap[] = {
            0,      //CORINFO_TYPE_UNDEF
            0,      //CORINFO_TYPE_VOID
            1,      //CORINFO_TYPE_BOOL
            1,      //CORINFO_TYPE_CHAR
            1,      //CORINFO_TYPE_BYTE
            1,      //CORINFO_TYPE_UBYTE
            1,      //CORINFO_TYPE_SHORT
            1,      //CORINFO_TYPE_USHORT
            1,      //CORINFO_TYPE_INT
            1,      //CORINFO_TYPE_UINT
            0,      //CORINFO_TYPE_LONG
            0,      //CORINFO_TYPE_ULONG
            1,      //CORINFO_TYPE_NATIVEINT
            1,      //CORINFO_TYPE_NATIVEUINT
            0,      //CORINFO_TYPE_FLOAT
            0,      //CORINFO_TYPE_DOUBLE
            1,      //CORINFO_TYPE_STRING
            1,      //CORINFO_TYPE_PTR
            1,      //CORINFO_TYPE_BYREF
            0,      //CORINFO_TYPE_VALUECLASS
            1,      //CORINFO_TYPE_CLASS
            0,      //CORINFO_TYPE_REFANY
            // CORINFO_TYPE_VAR only appears in "import-only" mode, but
            // the Fjit generates code even in "import-only" mode -- it
            // just throws the code away afterwards.  So we provide a
            // dummy value for CORINFO_TYPE_VAR.           
            1,      //CORINFO_TYPE_VAR
        };

// Specifies which types maybe enregistred in floating point registers
static const bool floatEnregisterMap[] = {
            0,      // typeError        
            0,      // typeByRef   
            0,      //  typeRef     
            0,      //  typeU1       
            0,      //  typeU2       
            0,      //  typeI1      
            0,      //  typeI2      
            0,      //  typeI4       
            0,      //  typeI8      
            0,      //  typeR4       
            0,      //  typeR8      
            0,      //  typeRefAny           
            0,      //  typeValClass 
            0,      //  typeMethod  
};
#elif defined(_PPC_)

#define ON_X86_ONLY(exp)  
#define ON_PPC_ONLY(exp)  exp
#define ON_SPARC_ONLY(exp)
#define EnregThis         true      // Is 'this' pointer eligble for being passed in a register
#define EnregArgumentsGP returnTrue()      // Should eligble arguments be enregistered in general purpose registers
#define EnregArgumentsFP returnTrue()      // Should eligble arguments be enregistered in floating point registers
#define MAX_GP_ARG_REGISTER 8       // Number of general purpose registers available for passing arguments
#define MAX_FP_ARG_REGISTER 13      // Number of floating point registers available for passing arguments
#define PARAMETER_SPACE   returnTrue()      // Should IL stack space not be recycled for passing arguments
#define FIXED_ENREG_BUFFER 0        // The parameter space for enregistered args is not fixed size 
 
#define PushEnregArgs        false  // Enregistered arguments are passed in registers
#define CALLER_CLEANS_STACK  true   // The caller is responsible for cleanning up the stack after the call
#define ARGS_RIGHT_TO_LEFT   returnFalse()
#define STACK_BUFFER         0      // The SP leads the top of the evaluation stack by this amount
#define HASZEROREG           false  // The architecture provides a register that always reads zero
#define M_RETBUFF_CALLSITE   true   // The callsites to functions which use the return buffer are padded with an extra nop
#define ALIGN_LOCALS         returnFalse()   // Locals should be aligned
#define ALIGN_ARGS           returnFalse()  // Arguments should be aligned
#define HASTOCREG            returnFalse() // Architecture has a TOC register

static const __int8 argRegistersMap[] = {
             3,  // r3
             4,  // r4
             5,  // r5
             6,  // r6
             7,  // r7
             8,  // r8
             9,  // r9
             10, // r10
        };

static const __int8 argFloatRegistersMap[] = {
             1,  // fp1
             2,  // fp2
             3,  // fp3
             4,  // fp4
             5,  // fp5
             6,  // fp6
             7,  // fp7
             8,  // fp8
             9,  // fp9
             10, // fp10
             11, // fp11
             12, // fp12
             13, // fp13
        };

static const __int8 typeSizeMap[] = {
            -1,     //CORINFO_TYPE_UNDEF
            0,      //CORINFO_TYPE_VOID
            4,      //CORINFO_TYPE_BOOL
            4,      //CORINFO_TYPE_CHAR
            4,      //CORINFO_TYPE_BYTE
            4,      //CORINFO_TYPE_UBYTE
            4,      //CORINFO_TYPE_SHORT
            4,      //CORINFO_TYPE_USHORT
            4,      //CORINFO_TYPE_INT
            4,      //CORINFO_TYPE_UINT
            8,      //CORINFO_TYPE_LONG
            8,      //CORINFO_TYPE_ULONG
            4,      //CORINFO_TYPE_NATIVEINT
            4,      //CORINFO_TYPE_NATIVEUINT
            4,      //CORINFO_TYPE_FLOAT
            8,      //CORINFO_TYPE_DOUBLE
            4,      //CORINFO_TYPE_STRING
            4,      //CORINFO_TYPE_PTR
            4,      //CORINFO_TYPE_BYREF
            -1,     //CORINFO_TYPE_VALUECLASS
            4,      //CORINFO_TYPE_CLASS
            8,      //CORINFO_TYPE_REFANY
            // CORINFO_TYPE_VAR only appears in "import-only" mode, but
            // the Fjit generates code even in "import-only" mode -- it
            // just throws the code away afterwards.  So we provide a
            // dummy value for CORINFO_TYPE_VAR.           
            4,      //CORINFO_TYPE_VAR
        };

inline unsigned int typeSizeFromJitType( unsigned type, bool precise )
{
  if ( precise && ( type == 3 /*typeU1*/ || type == 5 /*typeI1*/) )
    return 1;
  else if ( precise && ( type == 4 /*typeU2*/ || type == 6 /*typeI2*/) )
    return 2;
  else if ( type <= 7 || type == 9 /*typeR4*/ || type == 13 /*typeMethod */)
    return 4;
  else if ( type == 8 /*typeI8*/ || type == 10 /*typeR8*/ || type == 11)
    return 8;
  else
    return  0;
}

// Specifies which types maybe enregistred in either general purpose or floating point registers
static const bool typeEnregisterMap[] = {
            0,      //CORINFO_TYPE_UNDEF
            0,      //CORINFO_TYPE_VOID
            1,      //CORINFO_TYPE_BOOL
            1,      //CORINFO_TYPE_CHAR
            1,      //CORINFO_TYPE_BYTE
            1,      //CORINFO_TYPE_UBYTE
            1,      //CORINFO_TYPE_SHORT
            1,      //CORINFO_TYPE_USHORT
            1,      //CORINFO_TYPE_INT
            1,      //CORINFO_TYPE_UINT
            1,      //CORINFO_TYPE_LONG
            1,      //CORINFO_TYPE_ULONG
            1,      //CORINFO_TYPE_NATIVEINT
            1,      //CORINFO_TYPE_NATIVEUINT
            1,      //CORINFO_TYPE_FLOAT
            1,      //CORINFO_TYPE_DOUBLE
            1,      //CORINFO_TYPE_STRING
            1,      //CORINFO_TYPE_PTR
            1,      //CORINFO_TYPE_BYREF
            1,      //CORINFO_TYPE_VALUECLASS
            1,      //CORINFO_TYPE_CLASS
            1,      //CORINFO_TYPE_REFANY
            // CORINFO_TYPE_VAR only appears in "import-only" mode, but
            // the Fjit generates code even in "import-only" mode -- it
            // just throws the code away afterwards.  So we provide a
            // dummy value for CORINFO_TYPE_VAR.           
            1,      //CORINFO_TYPE_VAR
        };

// Specifies which types maybe enregistred in floating point registers
static const bool floatEnregisterMap[] = {
            0,      // typeError        
            0,      // typeByRef   
            0,      //  typeRef     
            0,      //  typeU1       
            0,      //  typeU2       
            0,      //  typeI1      
            0,      //  typeI2      
            0,      //  typeI4       
            0,      //  typeI8      
            1,      //  typeR4       
            1,      //  typeR8      
            0,      //  typeRefAny           
            0,      //  typeValClass 
            0,      //  typeMethod  
};
#else
#error "Platform not supported"
#endif 

// Support for reading multibyte primitives correctly
#if BIGENDIAN
#define BIGENDIAN_MACHINE true
#else
#define BIGENDIAN_MACHINE false
#endif

// Support for generating sizes aligned on a slot size
#define WORD_ALIGNED(n) ((n)+((n)%(SIZE_STACK_SLOT/(sizeof(void *)))))
#define BYTE_ALIGNED(n) (((n)+(SIZE_STACK_SLOT-1)) & ~(SIZE_STACK_SLOT-1) )


#define FjitCompile _compile

#if defined(_X86_) 
#include "i386/cx86def.h"
#elif defined(_PPC_)
#include "ppc/cppcdef.h"
#else
#error "Platform not supported"
#endif

#include "fjit.h"
#include "fjitdef.h"
#include "fjitverifier.h"

#endif // _FJITCORE_H_
