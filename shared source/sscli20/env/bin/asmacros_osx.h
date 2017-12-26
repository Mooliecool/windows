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

#define LOCAL_LABEL(name) L##name
#define GLOBAL_LABEL(name) _##name

#define ASMFUNC(name) \
    .align 2   \n\
    .globl GLOBAL_LABEL(name) \n \
GLOBAL_LABEL(name): \n

#define TEXT_SECTION .text
#define DATA_SECTION .data

#define TOC_DATA_SECTION DATA_SECTION

#define ARGUMENT_REGISTERS_GP(cmd,reg1,reg2)\
    cmd r3,  (0*4+reg1)(reg2) \n \
    cmd r4,  (1*4+reg1)(reg2) \n \
    cmd r5,  (2*4+reg1)(reg2) \n \
    cmd r6,  (3*4+reg1)(reg2) \n \
    cmd r7,  (4*4+reg1)(reg2) \n \
    cmd r8,  (5*4+reg1)(reg2) \n \
    cmd r9,  (6*4+reg1)(reg2) \n \
    cmd r10, (7*4+reg1)(reg2)

#define ARGUMENT_REGISTERS_FP(cmd,reg1,reg2)\
    cmd f1,  (0*8+reg1)(reg2) \n \
    cmd f2,  (1*8+reg1)(reg2) \n \
    cmd f3,  (2*8+reg1)(reg2) \n \
    cmd f4,  (3*8+reg1)(reg2) \n \
    cmd f5,  (4*8+reg1)(reg2) \n \
    cmd f6,  (5*8+reg1)(reg2) \n \
    cmd f7,  (6*8+reg1)(reg2) \n \
    cmd f8,  (7*8+reg1)(reg2) \n \
    cmd f9,  (8*8+reg1)(reg2) \n \
    cmd f10, (9*8+reg1)(reg2) \n \
    cmd f11, (10*8+reg1)(reg2) \n \
    cmd f12, (11*8+reg1)(reg2) \n \
    cmd f13, (12*8+reg1)(reg2)

#define CALLEESAVED_REGISTERS_GP(cmd,reg1,reg2)\
    cmd r13, (0*4+reg1)(reg2) \n \
    cmd r14, (1*4+reg1)(reg2) \n \
    cmd r15, (2*4+reg1)(reg2) \n \
    cmd r16, (3*4+reg1)(reg2) \n \
    cmd r17, (4*4+reg1)(reg2) \n \
    cmd r18, (5*4+reg1)(reg2) \n \
    cmd r19, (6*4+reg1)(reg2) \n \
    cmd r20, (7*4+reg1)(reg2) \n \
    cmd r21, (8*4+reg1)(reg2) \n \
    cmd r22, (9*4+reg1)(reg2) \n \
    cmd r23, (10*4+reg1)(reg2) \n \
    cmd r24, (11*4+reg1)(reg2) \n \
    cmd r25, (12*4+reg1)(reg2) \n \
    cmd r26, (13*4+reg1)(reg2) \n \
    cmd r27, (14*4+reg1)(reg2) \n \
    cmd r28, (15*4+reg1)(reg2) \n \
    cmd r29, (16*4+reg1)(reg2) \n \
    cmd r30, (17*4+reg1)(reg2) \n \
    cmd r31, (18*4+reg1)(reg2)

#define CALLEESAVED_REGISTERS_FP(cmd,reg1,reg2)\
    cmd f14, (0*8+reg1)(reg2) \n \
    cmd f15, (1*8+reg1)(reg2) \n \
    cmd f16, (2*8+reg1)(reg2) \n \
    cmd f17, (3*8+reg1)(reg2) \n \
    cmd f18, (4*8+reg1)(reg2) \n \
    cmd f19, (5*8+reg1)(reg2) \n \
    cmd f20, (6*8+reg1)(reg2) \n \
    cmd f21, (7*8+reg1)(reg2) \n \
    cmd f22, (8*8+reg1)(reg2) \n \
    cmd f23, (9*8+reg1)(reg2) \n \
    cmd f24, (10*8+reg1)(reg2) \n \
    cmd f25, (11*8+reg1)(reg2) \n \
    cmd f26, (12*8+reg1)(reg2) \n \
    cmd f27, (13*8+reg1)(reg2) \n \
    cmd f28, (14*8+reg1)(reg2) \n \
    cmd f29, (15*8+reg1)(reg2) \n \
    cmd f30, (16*8+reg1)(reg2) \n \
    cmd f31, (17*8+reg1)(reg2)


// Do the equivalent of "extern int X;", where
// the first argument is the variable name.
// This must be invoked within a .data section.
#define EXTERN_GLOBAL(name)                             \
    .non_lazy_symbol_pointer                         \n \
    L_##name##_non_lazy_ptr:                         \n \
        .indirect_symbol _##name                     \n \
        .long 0

#define EXTERN_FUNC(name)                               \
    .picsymbol_stub                                  \n \
    L_##name##$stub:                                 \n \
        .indirect_symbol _##name                     \n \
        mflr r0                                      \n \
        bcl 20,31,L0$_##name                         \n \
    L0$_##name:                                      \n \
        mflr r11                                     \n \
        addis r11,r11,ha16(L_##name##lazy_ptr-L0$_##name) \n \
        mtlr r0                                           \n \
        lwz r12,lo16(L_##name##lazy_ptr-L0$_##name)(r11)  \n \
        mtctr r12                                         \n \
        addi r11,r11,lo16(L_##name##lazy_ptr-L0$_##name)  \n \
        bctr                                              \n \
    .lazy_symbol_pointer                                  \n \
    L_##name##lazy_ptr:                                   \n \
        .indirect_symbol _##name                          \n \
        .long dyld_stub_binding_helper

#define LOAD_EXTERNAL_VAR(name, reg) \
       bl 0f                                              \n \
    0:                                                    \n \
       mflr reg                                           \n \
       addis reg , reg , ha16(L_##name##_non_lazy_ptr-0b) \n \
       lwz reg , lo16(L_##name##_non_lazy_ptr-0b)(reg)

#define EXTERN_LABEL(name) L_##name##$stub

#define CALL_FNCPTR_R12 \
    mtctr r12 \n \
    bctrl

#define roundup(n, roundto)  ((n)+roundto-((n) % roundto))
