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
// -*- C++ -*-
#ifndef _FJIT_ENCODE_H_
#define _FJIT_ENCODE_H_
#include <specstrings.h>
/*****************************************************************************/

/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XX                                                                           XX
XX                            FJitEncode.h                                   XX
XX                                                                           XX
XX   Encodes and decodes the il to pc map.  In uncompressed form, the map    XX
XX   is a sorted list of il/pc offset pairs where the il and the pc offset   XX
XX   indicate the start of an opcode.  In compressed form, the pairs are     XX
XX   delta encoded from the prior pair                                       XX
XX                                                                           XX
XX   Also has generic boolean array to bit string compress and decompress    XX
XX                                                                           XX
XX                                                                           XX
XX                                                                           XX
XX                                                                           XX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/

class LabelTable;

struct OpType;

class FJit_Encode {

private:

	/*struct Fjit_il2pcMap
	{
		unsigned ilOffset;    
		unsigned pcOffset;	 
	};*/
        typedef unsigned Fjit_il2pcMap;

	Fjit_il2pcMap*	map;
	unsigned		map_len;
	unsigned		map_capacity;
	bool			compressed;

	/* decompress the internals if necessary. Answer the number of entries in the map */
	unsigned decompress();

public:
	
	FJit_Encode();
	virtual ~FJit_Encode();

        // resets the map to empty
        void reset();

	/*adjust the internal mem structs as needed for the size of the method being jitted*/
	void ensureMapSpace(unsigned int len);

	/* decompress the bytes. Answer the number of entries in the map */
	virtual unsigned decompress(__in unsigned char* bytes);

	/* add a new pair to the end of the map.  Note pairs must be added in ascending order */
	void add(unsigned ilOffset, unsigned pcOffset);

        /* fill in the map for offset in the middle of IL opcodes (should be done after jump targets are validated )*/
        void fillIn();
      
	/* map an il offset to a pc offset, returns zero if the il offset does not exist */
	unsigned pcFromIL(unsigned ilOffset);

	/*map a pc offset to an il offset and optionally a pc offset within the opcode, 
	  returns -1 if il offset does not exist */
	virtual signed ilFromPC(unsigned pcOffset, unsigned* pcInILOffset);

	/* return the size of the compressed stream in bytes. */
	unsigned compressedSize();

	/* compress the map into the supplied buffer.  Return true if successful */
	bool compress(__in_ecount(buffer_len)unsigned char* buffer, unsigned buffer_len);

	/* compress the bool* onto itself and answer the number of compressed bytes */
	static unsigned compressBooleans(__in_ecount(buffer_len) unsigned char* buffer, unsigned buffer_len);

	/* answer the number of bytes it takes to encode an unsigned val */
	static unsigned encodedSize(unsigned val);

	/*encode an unsigned, buffer ptr is incremented */
	static unsigned encode(unsigned val, __deref_ecount(sizeof(unsigned)) unsigned char** buffer);

	/*encode an OpType, buffer ptr is incremented  */
   	static unsigned encodeOpType(OpType * pOpType, __deref_ecount(*pOpType) unsigned char ** buffer);

        /*decode an OpType, buffer ptr is incremented */
   	static void decodeOpType(OpType * pOpType, __in unsigned char ** buffer);

	/*decode an unsigned, buffer ptr is incremented, called from FJIT_EETwain.cpp */
	virtual unsigned decode_unsigned(__in unsigned char** buffer);

	/*decode an unsigned, buffer ptr is incremented, called from FJIT_EETwain.cpp */
	static unsigned decode(__in unsigned char** buffer);

	void reportDebuggingData(ICorJitInfo* jitInfo, CORINFO_METHOD_HANDLE ftn,
                            UINT prologEnd, UINT epilogStart, LabelTable * stacks, bool DumpMap );

};
#endif //_FJIT_ENCODE_H_


	

