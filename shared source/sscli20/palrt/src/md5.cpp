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
// File: md5.cpp
// 
// ===========================================================================
/*++

Abstract:

    MD5 implementation for PAL RT

Revision History:

--*/

/*
       File md5.c.                                          
                   
                     

      This implements the MD5 hash function.  
      For algorithmic background see (for example)

           Bruce Schneier
           Applied Cryptography
           Protocols, Algorithms, and Source Code in C
           Second Edition
           Wiley, 1996
           ISBN 0-471-12845-7
           QA76.9 A25S35

           Alfred J. Menezes et al
           Handbook of Applied Cryptography
           The CRC Press Series on Discrete Mathematics
                   and its Applications
           CRC Press LLC, 1997
           ISBN 0-8495-8523-7
           QA76.9A25M643

       Also see RFC (Request For Comments) 1321 from April, 1992.
       A July 2000 search for "rfc md5" or "rfc 1321"
       at www.yahoo.com found the document at

           http://www.cis.ohio-state.edu/htbin/rfc/rfc1321.html

       and other sites.
*/

#include "rotor_palrt.h"
#include "crypt.h"

typedef const DWORD DWORDC;
#define ROTATE32L(x,n) _rotl(x,n)
#define SHAVE32(x)     (DWORD)(x)

static DWORDC MD5_cons[64] =
// Round 1
 {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
  0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

// Round 2
  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
  0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

// Round 3
  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

// Round 4
  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};


static BOOL MD5_block(MD5_CTX * ctx)
/*
     Update the MD5 hash from a fresh 64 bytes of data.
*/
{
    DWORD a = ctx->partial_hash[0], b = ctx->partial_hash[1]; 
    DWORD c = ctx->partial_hash[2], d = ctx->partial_hash[3];
    DWORD  msg16[32];   /// Two copies of message
    DWORDC *cons = MD5_cons;
    int i;
    BOOL OK = TRUE;

    for (i = 0; i != 16; i++) {   // Copy to local array, zero original
                                  // Make two copies, to simplify indexing
        DWORDC datval = ctx->awaiting_data[i];
        ctx->awaiting_data[i] = 0;
        msg16[i] = msg16[i+16] = datval;
    }


// Round 1
    for (i = -16; i != 0; i += 4) { 
      
            // Rewrite (X & Y) | (~X & Z)  as  Z ^ (X & (Y ^ Z))
            // [Easily validated by checking X = 0 and X = 1 cases.]
            // This avoids ANDNOT (which X86 lacks) and needs only 
            // one temporary register.
            // On register-rich architectures, the Y ^ Z computation
            // can start early, before X is computed.

        a += msg16[i+16] + cons[0] + (d ^ (b & (c ^ d)));
        a = b + ROTATE32L(a, 7);

        d += msg16[i+17] + cons[1] + (c ^ (a & (b ^ c)));
        d = a + ROTATE32L(d, 12);

        c += msg16[i+18] + cons[2] + (b ^ (d & (a ^ b)));
        c = d + ROTATE32L(c, 17);

        b += msg16[i+19] + cons[3] + (a ^ (c & (d ^ a)));
        b = c + ROTATE32L(b, 22);

        cons += 4;
    }

// Round 2
    for (i = -16; i != 0; i += 4) {
            // Rewrite (Z & X) | (~Z & Y)  as  Y ^ (Z & (X ^ Y))
        a += msg16[i+17] + cons[0] + (c ^ (d & (b ^ c)));
        a = b + ROTATE32L(a, 5);

        d += msg16[i+22] + cons[1] + (b ^ (c & (a ^ b)));
        d = a + ROTATE32L(d, 9);

        c += msg16[i+27] + cons[2] + (a ^ (b & (d ^ a))); 
        c = d + ROTATE32L(c, 14);

        b += msg16[i+16] + cons[3] + (d ^ (a & (c ^ d)));
        b = c + ROTATE32L(b, 20);

        cons += 4;
    }

// Round 3
    for (i = 16; i != 0; i -= 4) { 
        a += msg16[i+5]  + cons[0] + ((b ^ c) ^ d);
        a = b + ROTATE32L(a, 4);
        
        d += msg16[i+8]  + cons[1] + (a ^ (b ^ c));
        d = a + ROTATE32L(d, 11);
        
        c += msg16[i+11] + cons[2] + ((d ^ a) ^ b);
        c = d + ROTATE32L(c, 16);

        b += msg16[i+14] + cons[3] + (c ^ (d ^ a));
        b = c + ROTATE32L(b, 23);

        cons += 4;
    }

// Round 4
    for (i = 16; i != 0; i -= 4) {
        a += msg16[i  ]  + cons[0] + (c ^ (~d | b));
        a = b + ROTATE32L(a, 6);

        d += msg16[i+7]  + cons[1] + (b ^ (~c | a));
        d = a + ROTATE32L(d, 10);

        c += msg16[i+14] + cons[2] + (a ^ (~b | d));
        c = d + ROTATE32L(c, 15);

        b += msg16[i+5]  + cons[3] + (d ^ (~a | c));
        b = c + ROTATE32L(b, 21);

        cons += 4;
    }
    assert (cons == MD5_cons + 64);

    ctx->partial_hash[0] += a;
    ctx->partial_hash[1] += b;
    ctx->partial_hash[2] += c;
    ctx->partial_hash[3] += d;
    return OK;
} // end MD5_block


EXTERN_C BOOL PALAPI MD5Init(MD5_CTX * ctx)
{
    BOOL OK = TRUE;
    DWORD i;
    ctx->nbit_total[0] = ctx->nbit_total[1] = 0;
    for (i = 0; i != 16; i++) {
        ctx->awaiting_data[i] = 0;
    }
   
     /* 
         Initialize hash variables.
         
         N.B.  The initial values in RFC 1321 appear
         in byte-reversed order.  Bruce Schneier's 
         2nd edition neglects to rearrange them.
     */

    ctx->partial_hash[0] = 0x67452301;
    ctx->partial_hash[1] = 0xefcdab89;
    ctx->partial_hash[2] = ~ctx->partial_hash[0];
    ctx->partial_hash[3] = ~ctx->partial_hash[1];
   
    return OK;
} // end MD5Init


EXTERN_C BOOL PALAPI MD5Update(
        MD5_CTX *   ctx,    // IN/OUT
        const BYTE *    msg,    // IN
        DWORD           nbyte)  // IN
/*
    Append data to a partially hashed MD5 message.
*/
{
    const BYTE *fresh_data = msg;
    DWORD nbyte_left = nbyte;
    DWORD nbit_occupied = ctx->nbit_total[0] & 511;
    DWORD *awaiting_data;
    BOOL OK = TRUE;
    DWORDC nbitnew_low = SHAVE32(8*nbyte);
        

    assert((nbit_occupied & 7) == 0);   // Partial bytes not implemented
    ctx->nbit_total[0] += nbitnew_low;
    ctx->nbit_total[1] += (nbyte >> 29) 
           + (SHAVE32(ctx->nbit_total[0]) < nbitnew_low);

        /* Advance to word boundary in waiting_data */
    
    if ((nbit_occupied & 31) != 0) {
        awaiting_data = ctx->awaiting_data + nbit_occupied/32;

        while ((nbit_occupied & 31) != 0 && nbyte_left != 0) {
            *awaiting_data |= (DWORD)*fresh_data++ 
                     << (nbit_occupied & 31);
            nbyte_left--;
            nbit_occupied += 8;
        }
    } // if nbit_occupied

             /* Transfer 4 bytes at a time */

    do {
        DWORDC nword_occupied = nbit_occupied/32;
        DWORD nwcopy = min(nbyte_left/4, 16 - nword_occupied);
        assert (nbit_occupied <= 512);
        assert ((nbit_occupied & 31) == 0 || nbyte_left == 0);
        awaiting_data = ctx->awaiting_data + nword_occupied;
        nbyte_left -= 4*nwcopy;
        nbit_occupied += 32*nwcopy;

        while (nwcopy != 0) {
            DWORDC byte0 = (DWORD)fresh_data[0];
            DWORDC byte1 = (DWORD)fresh_data[1];
            DWORDC byte2 = (DWORD)fresh_data[2];
            DWORDC byte3 = (DWORD)fresh_data[3];
            *awaiting_data++ = byte0 | (byte1 << 8)
                        | (byte2 << 16) | (byte3 << 24);
                             /* Little endian */
            fresh_data += 4;
            nwcopy--;
        } 

        if (nbit_occupied == 512) {
            OK = OK && MD5_block(ctx);
            nbit_occupied = 0;
            awaiting_data -= 16;
            assert(awaiting_data == ctx->awaiting_data);
        }
    } while (nbyte_left >= 4); 

    assert (ctx->awaiting_data + nbit_occupied/32
                       == awaiting_data);

    while (nbyte_left != 0) {
        DWORDC new_byte = (DWORD)*fresh_data++;

        assert((nbit_occupied & 31) <= 16);
        *awaiting_data |= new_byte << (nbit_occupied & 31);
        nbit_occupied += 8;
        nbyte_left--;
    }
    assert (nbit_occupied == (ctx->nbit_total[0] & 511));
    return OK;
} // end MD5Update



EXTERN_C BOOL PALAPI MD5Final(
        MD5_CTX *   ctx,        // IN/OUT
        BYTE *          digest)     // OUT
/*
        Finish an MD5 hash.
*/
{
    BOOL OK = TRUE;
    DWORDC nbit0 = ctx->nbit_total[0];
    DWORDC nbit1 = ctx->nbit_total[1];
    DWORD  nbit_occupied = nbit0 & 511;
    DWORD i;

    assert((nbit_occupied & 7) == 0);

    ctx->awaiting_data[nbit_occupied/32] 
         |= (DWORD)0x80 << (nbit_occupied & 31);
                          // Append a 1 bit
    nbit_occupied += 8;

// TBD -- Above seems wierd -- why number bytes from the
//        least significant end but number bits the other way?

    // Append zero bits until length (in bits) is 448 mod 512.
    // Then append the length, in bits.
    // Here we assume the buffer was zeroed earlier.

    if (nbit_occupied > 448) {   // If fewer than 64 bits left
        OK = OK && MD5_block(ctx);
        nbit_occupied = 0;
    }
    ctx->awaiting_data[14] = nbit0;
    ctx->awaiting_data[15] = nbit1;
    OK = OK && MD5_block(ctx);

         /* Copy final digest to byte array */

    for (i = 0; i != 4; i++) {
        DWORDC dwi = ctx->partial_hash[i];
        digest[4*i    ] = (BYTE)(dwi         & 255);  // Little-endian
        digest[4*i + 1] = (BYTE)((dwi >>  8) & 255);
        digest[4*i + 2] = (BYTE)((dwi >> 16) & 255);
        digest[4*i + 3] = (BYTE)((dwi >> 24) & 255);
    }
    return OK;
} // end MD5Final
