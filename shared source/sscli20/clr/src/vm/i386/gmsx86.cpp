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
/**************************************************************/
/*                       gmsx86.cpp                           */
/**************************************************************/

#include "common.h"
#include "gmscpu.h"

/***************************************************************/
/* setMachState figures out what the state of the CPU will be
   when the function that calls 'setMachState' returns.  It stores
   this information in 'frame'

   setMachState works by simulating the execution of the
   instructions starting at the instruction following the
   call to 'setMachState' and continuing until a return instruction
   is simulated.  To avoid having to process arbitrary code, the
   call to 'setMachState' should be called as follows

      if (machState.setMachState != 0) return;

   setMachState is guarnenteed to return 0 (so the return
   statement will never be executed), but the expression above
   insures insures that there is a 'quick' path to epilog
   of the function.  This insures that setMachState will only
   have to parse a limited number of X86 instructions.   */


/***************************************************************/
#ifndef POISONC
#define POISONC ((sizeof(int *) == 4)?0xCCCCCCCCU:UI64(0xCCCCCCCCCCCCCCCC))
#endif

#ifndef DACCESS_COMPILE

void MachState::Init(TADDR* aPEdi, TADDR* aPEsi, TADDR* aPEbx, TADDR* aPEbp, TADDR aEsp, TADDR* aPRetAddr) {
    LEAF_CONTRACT;


#ifdef _DEBUG
    _edi = POISONC;
    _esi = POISONC;
    _ebx = POISONC;
    _ebp = POISONC;
#endif
    _esp = aEsp;
    _pRetAddr = aPRetAddr;
    _pEdi = aPEdi;
    _pEsi = aPEsi;
    _pEbx = aPEbx;
    _pEbp = aPEbp;
}

#endif // #ifndef DACCESS_COMPILE

/***************************************************************/
TADDR LazyMachState::unwindLazyState(LazyMachState* baseState,
                                     MachState* lazyState,
                                     int funCallDepth,
                                     TestFtn testFtn)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    lazyState->_edi = baseState->_edi;
    lazyState->_esi = baseState->_esi;
    lazyState->_ebx = baseState->_ebx;
    lazyState->_ebp = baseState->captureEbp;
#ifndef DACCESS_COMPILE
    lazyState->_pEdi = &baseState->_edi;
    lazyState->_pEsi = &baseState->_esi;
    lazyState->_pEbx = &baseState->_ebx;
    lazyState->_pEbp = &baseState->_ebp;
#endif

    // currently we only do this for depth 1 through 4
    _ASSERTE(1 <= funCallDepth && funCallDepth <= 4);

    // We have captured the state of the registers as they exist in 'captureState'
    // we need to simulate execution from the return address captured in 'captureState
    // until we return from the caller of captureState.

    PTR_BYTE ip = PTR_BYTE(baseState->captureEip);
    PTR_TADDR ESP = PTR_TADDR(baseState->captureEsp);
    ESP++;                                 // pop captureState's return address


    // VC now has small helper calls that it uses in epilogs.  We need to walk into these
    // helpers if we are to decode the stack properly.  After we walk the helper we need
    // to return and continue walking the epiliog.  This varaible remembers were to return to
    PTR_BYTE epilogCallRet = PTR_BYTE((TADDR)0);

    // The very first conditional jump that we are going to encounter is
    // the one testing for the return value of LazyMachStateCaptureState.
    // The non-zero path is the one directly leading to a return statement.
    // This variable keeps track of whether we are still looking for that
    // first conditional jump.
    BOOL bFirstCondJmp = TRUE;

    // The general strategy is that we always try to plough forward:
    // we follow a conditional jump if and only if it is a forward jump.
    // However, in fcall functions that set up a HELPER_METHOD_FRAME in
    // more than one place, gcc will have both of them share the same
    // epilog - and the second one may actually be a backward jump.
    // This can lead us to loop in a destructor code loop.  To protect
    // against this, we remember the ip of the last conditional jump
    // we followed, and if we encounter it again, we take the other branch.
    PTR_BYTE lastCondJmpIp = PTR_BYTE((TADDR)0);

    int datasize; // helper variable for decoding of r/m

#ifdef _DEBUG
    int count = 0;
#endif
    bool bset16bit=false;
    bool b16bit=false;
    for(;;)
    {
        _ASSERTE(count++ < 1000);       // we should never walk more than 1000 instructions!
        b16bit=bset16bit;
        bset16bit=false;

#ifndef DACCESS_COMPILE
    again:
#endif
        switch(*ip)
        {
            case 0x90:              // nop
            case 0x67:
            case 0x64:              // FS: prefix
            incIp1:
                bset16bit=b16bit;
                ip++;
                break;
            case 0x66:
                ip++;
                bset16bit=true;
                break;
            case 0x5B:              // pop EBX
                lazyState->_pEbx = ESP;
                lazyState->_ebx  = *ESP++;
                goto incIp1;
            case 0x5D:              // pop EBP
                lazyState->_pEbp = ESP;
                lazyState->_ebp  = *ESP++;
                goto incIp1;
            case 0x5E:              // pop ESI
                lazyState->_pEsi = ESP;
                lazyState->_esi = *ESP++;
                goto incIp1;
            case 0x5F:              // pop EDI
                lazyState->_pEdi = ESP;
                lazyState->_edi = *ESP++;
                goto incIp1;

            case 0x58:              // pop EAX
            case 0x59:              // pop ECX
            case 0x5A:              // pop EDX
            case 0x9D:              // popfd
                ESP++;
                goto incIp1;

            case 0xEB:              // jmp <disp8>
                ip += (signed __int8) ip[1] + 2;
                break;

            case 0xE8:              // call <disp32>
                ip += 5;
                //
                if (epilogCallRet == 0)
                {
                    PTR_BYTE ip2 = ip;

                    // First look for instrumentation sequence to skip

                    // Any BBT instrumentation:
                    //
                    //   C605xxxxxxxx00    mov     byte ptr [disp32], 0
                    //   68xxxxxxxx        push    imm32
                    //   E8xxxxxxxx        call    rel32
                    //
                    if (ip2[0] == 0xc6 && ip2[1] == 0x05 && ip2[6] == 0x00 &&
                        ip2[7] == 0x68 &&
                        ip2[12] == 0xe8) {
                        ip2 += 17;
                    }

                    // nosweep BBT instrumentation
                    //
                    //   FF3424            push    dword ptr [esp]
                    //   6Axx/68xxxxxxxx   push    imm8/imm32
                    //   E8xxxxxxxx        call    rel32
                    //
                    if (ip2[0] == 0xff && ip2[1] == 0x34 && ip2[2] == 0x24)
                    {
                        if (ip2[3] == 0x6a && ip2[5] == 0xe8) {
                            ip2 += 10;
                        }
                        else if (ip2[3] == 0x68 && ip2[8] == 0xe8) {
                            ip2 += 13;
                        }
                    }

                    // Code coverage instrumentation
                    //
                    //   52                push    edx
                    //   8B15xxxxxxxx      mov     edx,[disp32]
                    //   C682xxxxxxxxxx    mov     byte ptr [edx+disp32], 1
                    //   5A                pop     edx
                    //
                    if (ip2[0] == 0x52 && ip2[1] == 0x8b && ip2[2] == 0x15 &&
                        ip2[7] == 0xc6 && ip2[8] == 0x82 && ip2[14] == 0x5a) {
                        ip2 += 15;
                    }

                    // Skip unconditional branches which BBT likes to insert
                    // Don't follow more than 20 branches
                    //
                    //   EBxx              jmp     rel8
                    //   E9xxxxxxxx        jmp     rel32
                    //
                    for (int branchCount = 0; branchCount < 20; branchCount++)
                    {
                        if (ip2[0] == 0xEB) {             // jmp <disp8>
                            ip2 += (signed __int8) ip2[1] + 2;
                        }
                        else if (ip2[0] == 0xE9) {        // jmp <disp32>
                            ip2 += (__int32)*PTR_DWORD(PTR_TO_TADDR(ip2) + 1) + 5;
                        }
                        else {
                            break;
                        }
                    }

                    if (*ip2 == 0xC2 || *ip2 == 0xC3) { // is next instr a ret or retn?
                        // Yes.  we found a call we need to walk into.
                        epilogCallRet = ip2;            // remember our return address
                        --ESP;                          // simulate pushing the return address
                        ip += (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) - 4);    // goto the call

                        // handle jmp [addr] in case SEH_Epilog is imported
                        if (*ip == 0xFF && *(ip+1) == 0x25)
                        {
                            ip = PTR_BYTE(*PTR_TADDR(*PTR_TADDR(PTR_TO_TADDR(ip) + 2)));
                        }
                    }
                }
                break;

            case 0xE9:              // jmp <disp32>
                ip += (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 1) + 5;
                break;

            case 0x0f:              // follow non-zero jumps:
              if (bFirstCondJmp) {
                  bFirstCondJmp = FALSE;
                  if (ip[1] == 0x85)  // jne <disp32>
                      ip += (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2) + 6;
                  else if (ip[1] == 0x84)  // je <disp32>
                      ip += 6;
                  else
                      goto badOpcode;
              }
              else {
                  if ((ip[1] == 0x84) || (ip[1] == 0x85)) {                     // jz or jnz
                      PTR_BYTE tmpIp = ip + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2) + 6;

                      if ((tmpIp > ip) == (lastCondJmpIp != ip)) {
                          lastCondJmpIp = ip;
                          ip = tmpIp;
                      }
                      else {
                          lastCondJmpIp = ip;
                          ip += 6;
                      }
                  }
                  else
                      goto badOpcode;
              }
              break;

              // This is here because VC seems to not always optimize
              // away a test for a literal constant
            case 0x6A:              // push 0xXX
                ip += 2;
                --ESP;
                break;

            // Added to handle VC7 generated code
            case 0x50:              // push EAX
            case 0x51:              // push ECX
            case 0x52:              // push EDX
            case 0x53:              // push EBX
            case 0x55:              // push EBP
            case 0x56:              // push ESI
            case 0x57:              // push EDI
            case 0x9C:              // pushfd
                --ESP;
            case 0x40:              // inc EAX
            case 0x46:              // inc ESI
                goto incIp1;

            case 0x68:              // push 0xXXXXXXXX
                if ((ip[5] == 0xFF) && (ip[6] == 0x15)) {
                    ip += 11;
                }
                else
                    ip += 5;
                break;

           case 0x74:              // jz <target>
                if (bFirstCondJmp) {
                    bFirstCondJmp = FALSE;
                    ip += 2;            // follow the non-zero path
                    break;
                }
                goto condJumpDisp8;

            case 0x75:              // jnz <target>
                // Except the first jump, we always follow forward jump to avoid possible looping.
                if (bFirstCondJmp) {
                    bFirstCondJmp = FALSE;
                    ip += (signed __int8) ip[1] + 2;   // follow the non-zero path
                    break;
                }
                goto condJumpDisp8;

            case 0x79:              // jns <target>
                goto condJumpDisp8;

           condJumpDisp8:
                {
                    PTR_BYTE tmpIp = ip + (TADDR)(signed __int8) ip[1] + 2;
                    if ((tmpIp > ip) == (lastCondJmpIp != ip)) {
                        lastCondJmpIp = ip;
                        ip = tmpIp;
                    }
                    else {
                        lastCondJmpIp = ip;
                        ip += 2;
                    }
                }
                break;

            case 0x85:
                if ((ip[1] & 0xC0) != 0xC0)  // TEST reg1, reg2
                    goto badOpcode;
                ip += 2;
                break;

            case 0x31:
            case 0x32:
            case 0x33:
                if ((ip[1] & 0xC0) == 0xC0) // mod bits are 11
                {

                    if ((ip[1] & 7) == ((ip[1] >> 3) & 7)) {
                        if (ip[2] == 0x85 && ip[3] == ip[1]) {      // TEST reg, reg
                            if (ip[4] == 0x74) {
                                ip += (signed __int8) ip[5] + 6;   // follow the non-zero path
                                break;
                            }
                            _ASSERTE(ip[4] != 0x0f || ((ip[5] & 0xF0)!=0x80)); // If this goes off, we need the big jumps
                        }
                        else
                        {
                            if(ip[2]==0x74)
                            {
                                ip += (signed __int8) ip[3] + 4;
                                break;
                            }
                            _ASSERTE(ip[2] != 0x0f || ((ip[3] & 0xF0)!=0x80));              // If this goes off, we need the big jumps
                        }
                    }
                    ip += 2;
                }
                else if ((ip[1] & 0xC0) == 0x40) // mod bits are 01
                {
                    // XOR reg1, [reg+offs8]
                    // Used by the /GS flag for call to __security_check_cookie()
                    // Should only be XOR ECX,[EBP+4]
                    _ASSERTE((((ip[1] >> 3) & 0x7) == 0x1) && ((ip[1] & 0x7) == 0x5) && (ip[2] == 4));
                    ip += 3;
                }
                else if ((ip[1] & 0xC0) == 0x80) // mod bits are 10
                {
                    // XOR reg1, [reg+offs32]
                    // Should not happen but may occur with __security_check_cookie()
                    _ASSERTE(!"Unexpected XOR reg1, [reg+offs32]");
                    ip += 6;
                }
                else
                {
                    goto badOpcode;
                }
                break;

            case 0x05:
                // added to handle gcc 3.3 generated code
                // add %reg, constant
                ip += 5;
                break;

            case 0xFF:
                if ( (ip[1] & 0x38) == 0x30)
                {
                    // opcode generated by Vulcan/BBT instrumentation
                    // search for push dword ptr[esp]; push imm32; call disp32 and if found ignore it
                    if ((ip[1] == 0x34) && (ip[2] == 0x24) && // push dword ptr[esp]  (length 3 bytes)
                        (ip[3] == 0x68) &&                    // push imm32           (length 5 bytes)
                        (ip[8] == 0xe8))                      // call disp32          (length 5 bytes)
                    {
                        // found the magic seq emitted by Vulcan instrumentation
                        ip += 13;  // (3+5+5)
                        break;
                    }

                    --ESP;      // push r/m
                    datasize = 0;
                    goto decodeRM;
                }
                else if ( (ip[1] & 0x38) == 0x10) 
                {
                    // added to handle gcc 3.3 generated code
                    // This is a call *(%eax) generated by gcc for destructor calls.
                    // We can safely skip over the call
                    datasize = 0;
                    goto decodeRM;
                }
                else
                {
                    goto badOpcode;
                }
                break;

            case 0x39:                       // comp r/m, reg
            case 0x3B:                       // comp reg, r/m
                datasize = 0;
                goto decodeRM;

            case 0x41:
                ip += 1;
                break;

            case 0xA1:                          // MOV EAX, [XXXX]
                ip += 5;
                break;

            case 0x89:                          // MOV (DWORD)
                if (ip[1] == 0xEC)              // MOV ESP, EBP
                    goto mov_esp_ebp;
                // FALL THROUGH

            case 0x88:                          // MOV (BYTE)
                datasize = 0;

            decodeRM:
                if ((ip[1] & 0xC0) == 0x0) {    // MOV [REG], REG
                    if ((ip[1] & 7) == 5)       // MOV [mem], REG
                        ip += datasize + 6;
                    else
                    if ((ip[1] & 7) == 4)       // SIB byte
                        ip += datasize + 3;
                    else
                    if ((ip[1] & 7) == 6)
                        ip += datasize + 4;
                    else
                        ip += datasize + 2;
                    break;
                }
                if ((ip[1] & 0xC0) == 0x40) {   // MOV [REG+XX], REG
                    if ((ip[1] & 7) == 4)       // SIB byte
                        ip += datasize + 4;
                    else
                        ip += datasize + 3;
                    break;
                }
                if ((ip[1] & 0xC0) == 0x80) {   // MOV [REG+XXXX], REG
                    if ((ip[1] & 7) == 4)       // SIB byte
                        ip += datasize + 7;
                    else
                        ip += datasize + 6;
                    break;
                }
                if ((ip[1] & 0xC0) == 0xC0) {    // MOV EAX, REG
                    ip += datasize + 2;
                    break;
                }
                goto badOpcode;

            case 0x80:                           // OP r/m8, <imm8>
                datasize = 1;
                goto decodeRM;

            case 0x81:                           // OP r/m32, <imm32>
                if (!b16bit && ip[1] == 0xC4) { // ADD ESP, <imm32>
                    ESP = PTR_TADDR(PTR_TO_TADDR(ESP) + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2));
                    ip += 6;
                    break;
                } else if (!b16bit && ip[1] == 0xC5) { // ADD EBP, <imm32>
                    lazyState->_ebp += (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2);
                    ip += 6;
                    break;
                }

                datasize = b16bit?2:4;
                goto decodeRM;

            case 0x83:                           // OP r/m32, <imm8>
                if (ip[1] == 0xC4)  {            // ADD ESP, <imm8>
                    ESP = PTR_TADDR(PTR_TO_TADDR(ESP) + (signed __int8)ip[2]);
                    ip += 3;
                    break;
                }
                if (ip[1] == 0xc5) {            // ADD EBP, <imm8>
                    lazyState->_ebp += (signed __int8)ip[2];
                    ip += 3;
                    break;
                }

                datasize = 1;
                goto decodeRM;

            case 0x8B:                                  // MOV (DWORD)
                if (ip[1] == 0xE5) {                    // MOV ESP, EBP
                mov_esp_ebp:
                    ESP = PTR_TADDR(lazyState->_ebp);
                    ip += 2;
                    break;
                }

                if ((ip[1] & 0xC7) == 0x45) {   // MOV reg, [EBP + imm8]
                    // gcc sometimes restores callee-preserved registers
                    // via 'mov reg, [ebp-xx]' instead of 'pop reg'
                    if ( ip[1] == 0x5D ) {  // MOV EBX, [EBP+XX]
                      lazyState->_pEbx = PTR_TADDR(lazyState->_ebp + (signed __int8)ip[2]);
                      lazyState->_ebx =  *lazyState->_pEbx ;
                    }
                    else if ( ip[1] == 0x75 ) {  // MOV ESI, [EBP+XX]
                      lazyState->_pEsi = PTR_TADDR(lazyState->_ebp + (signed __int8)ip[2]);
                      lazyState->_esi =  *lazyState->_pEsi;
                    }
                    else if ( ip[1] == 0x7D ) {  // MOV EDI, [EBP+XX]
                      lazyState->_pEdi = PTR_TADDR(lazyState->_ebp + (signed __int8)ip[2]);
                      lazyState->_edi =   *lazyState->_pEdi;
                    }
                    else if ( ip[1] == 0x65 /*ESP*/ || ip[1] == 0x6D /*EBP*/)
                      goto badOpcode;

                    /*EAX,ECX,EDX*/
                    ip += 3;
                    break;
                }

                if ((ip[1] & 0xC7) == 0x85) {   // MOV E*X, [EBP+imm32]
                    // gcc sometimes restores callee-preserved registers
                    // via 'mov reg, [ebp-xx]' instead of 'pop reg'
                    if ( ip[1] == 0xDD ) {  // MOV EBX, [EBP+XXXXXXXX]
                      lazyState->_pEbx = PTR_TADDR(lazyState->_ebp + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2));
                      lazyState->_ebx =  *lazyState->_pEbx ;
                    }
                    else if ( ip[1] == 0xF5 ) {  // MOV ESI, [EBP+XXXXXXXX]
                      lazyState->_pEsi = PTR_TADDR(lazyState->_ebp + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2));
                      lazyState->_esi =  *lazyState->_pEsi;
                    }
                    else if ( ip[1] == 0xFD ) {  // MOV EDI, [EBP+XXXXXXXX]
                      lazyState->_pEdi = PTR_TADDR(lazyState->_ebp + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2));
                      lazyState->_edi =   *lazyState->_pEdi;
                    }
                    else if ( ip[1] == 0xE5 /*ESP*/ || ip[1] == 0xED /*EBP*/)
                      goto badOpcode;  // Add more registers

                    /*EAX,ECX,EDX*/
                    ip += 6;
                    break;
                }

                // FALL THROUGH
            case 0x8A:                  // MOV (byte)
                datasize = 0;
                goto decodeRM;

            case 0x8D:                          // LEA
                if ((ip[1] & 0x38) == 0x20) {                       // Don't allow ESP to be updated
                    if (ip[1] == 0xA5)          // LEA ESP, [EBP+XXXX]
                        ESP = PTR_TADDR(lazyState->_ebp + (__int32)*PTR_DWORD(PTR_TO_TADDR(ip) + 2));
                    else if (ip[1] == 0x65)     // LEA ESP, [EBP+XX]
                        ESP = PTR_TADDR(lazyState->_ebp + (signed __int8) ip[2]);
                    else
                        goto badOpcode;
                }

                datasize = 0;
                goto decodeRM;

            case 0xA5:  // MOVS
                ip += 1;
                break;

            case 0xB0:  // MOV AL, imm8
                ip += 2;
                break;
            case 0xB8:  // MOV EAX, imm32
            case 0xB9:  // MOV ECX, imm32
            case 0xBA:  // MOV EDX, imm32
            case 0xBB:  // MOV EBX, imm32
            case 0xBE:  // MOV ESI, imm32
            case 0xBF:  // MOV EDI, imm32
                if(b16bit)
                    ip += 3;
                else
                    ip += 5;
                break;

            case 0xC2:                  // ret N
                {
                unsigned __int16 disp = *PTR_WORD(PTR_TO_TADDR(ip) + 1);
                ip = PTR_BYTE(*ESP);
                lazyState->_pRetAddr = ESP++;
                _ASSERTE(disp < 64);    // sanity check (although strictly speaking not impossible)
                ESP = PTR_TADDR(PTR_TO_TADDR(ESP) + disp);         // pop args
                goto ret;
                }
            case 0xC3:                  // ret
                ip = PTR_BYTE(*ESP);
                lazyState->_pRetAddr = ESP++;

                if (epilogCallRet != 0) {       // we are returning from a special epilog helper
                    ip = epilogCallRet;
                    epilogCallRet = 0;
                    break;                      // this does not count toward funcCallDepth
                }
            ret:
                --funCallDepth;
                if (funCallDepth <= 0 || (testFtn != 0 && (*testFtn)((void*)*lazyState->pRetAddr())))
                    goto done;
                bFirstCondJmp = TRUE;
                break;

            case 0xC6:                  // MOV r/m8, imm8
                datasize = 1;
                goto decodeRM;

            case 0xC7:                  // MOV r/m32, imm32
                datasize = b16bit?2:4;
                goto decodeRM;

            case 0xC9:                  // leave
                ESP = PTR_TADDR(lazyState->_ebp);
                lazyState->_pEbp = ESP;
                lazyState->_ebp = *ESP++;
                ip++;
                break;

#ifndef DACCESS_COMPILE
            case 0xCC:
                if (IsDebuggerPresent())
                {
                    OutputDebugStringA("CLR: Invalid breakpoint in a helpermethod frame epilog\n");
                    DebugBreak();
                    goto again;
                }
                *((int*) 0) = 1;
                goto done;
#endif

            case 0xD0:  //  shl REG16, 1
            case 0xD1:  //  shl REG32, 1
                    if (0xE4 == ip[1] || 0xE5 == ip[1]) // shl, ESP, 1 or shl EBP, 1
                    goto badOpcode;       // Doesn't look like valid code
                ip += 2;
                break;

            case 0xD9:  // single prefix
                if (0xEE == ip[1])
                {
                    ip += 2;            // FLDZ
                    break;
                }
                //
                // INTENTIONAL FALL THRU
                //
            case 0xDD:  // double prefix
                if ((ip[1] & 0xC0) != 0xC0)
                {
                    datasize = 0;       // floatop r/m
                    goto decodeRM;
                }
                else
                {
                    goto badOpcode;
                }
                break;

            case 0xF3: // rep prefix
                ip += 1;
                break;

            case 0xF6:
                if ( (ip[1] & 0x38) == 0x00) // TEST r/m8, imm8
                {
                    datasize = 1;
                    goto decodeRM;
                }
                else
                {
                    goto badOpcode;
                }
                break;

            case 0xF7:
                if ( (ip[1] & 0x38) == 0x00) // TEST r/m32, imm32
                {
                    datasize = b16bit?2:4;
                    goto decodeRM;
                }
                else
                {
                    goto badOpcode;
                }
                break;

            default:
            badOpcode:
                _ASSERTE(!"Bad opcode");
                // FIX what to do here?
#ifndef DACCESS_COMPILE
                *((unsigned __int8**) 0) = ip;  // cause an access violation (Free Build assert)
#else
                DacNotImpl();
#endif
                goto done;
        }
    }
done:
    _ASSERTE(epilogCallRet == 0);

    // At this point the fields in 'frame' coorespond exactly to the register
    // state when the the helper returns to its caller.
    lazyState->_esp = PTR_TO_TADDR(ESP);
    // Return a pointer to the return address.
    return PTR_TO_TADDR(lazyState->_pRetAddr);
}
