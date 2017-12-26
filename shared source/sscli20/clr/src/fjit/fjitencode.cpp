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
#include "jitpch.h"

/*****************************************************************************/

/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XX                                                                           XX
XX                            FJitEncode.cpp                                   XX
XX                                                                           XX
XX   Encodes and decodes the il to pc map.  In uncompressed form, the map    XX
XX   is a sorted list of il/pc offset pairs where the il and the pc offset   XX
XX   indicate the start of an opcode.  In compressed form, the pairs are     XX
XX   delta encoded from the prior pair                                       XX
XX                                                                           XX
XX                                                                           XX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/


//Note; The compression is done inplace 

#include "fjitencode.h"
#include "fjit.h"   // Needed for LabelTable definition and New macro

FJit_Encode::FJit_Encode() {
        map = NULL;
	map_len = 0;
        map_capacity = 0;
	compressed = false;
}

FJit_Encode::~FJit_Encode() {
	if (map) delete [] map;
	map = NULL;
	map_capacity = 0;
}

void FJit_Encode::reset() {
	map_len = 0;
        _ASSERTE(!compressed);
}
/*adjust the internal mem structs as needed for the size of the method being jitted*/
void FJit_Encode::ensureMapSpace(unsigned int len) {
	//note, we set the map capcity to be at least one greater #opcodes to allow for an 
	//end of method entry
	unsigned needed;
	needed = len + 1;  //since we are using reference il codes
	if (needed >= map_capacity) {
		if (map) delete [] map;
        New(map,Fjit_il2pcMap[needed]);
		map_capacity = needed;
	}
	// Reset the values of the field targets
	memset(map, 0, needed * sizeof(Fjit_il2pcMap));
	map_len = 0;
	compressed = false;
}

/* add a new pair to the end of the map.  Note pairs must be added in ascending order */
void FJit_Encode::add(unsigned ilOffset, unsigned pcOffset) {
        _ASSERTE(!compressed);
	_ASSERTE(ilOffset < map_capacity);

        map[ilOffset] = pcOffset;
	map_len = map_len > ilOffset ? map_len : ilOffset;
}

void FJit_Encode::fillIn()
{
  _ASSERTE(!compressed);
   for (unsigned i = 1; i <= map_len; i++)
     if ( map[i] == 0 )
       map[i] = map[i-1];
}

/* map an il offset to a pc offset, 
   if il offset is in middle of op code, 
   return pc offset of start of op code
*/
unsigned FJit_Encode::pcFromIL(unsigned ilOffset) {
    map_len = decompress();
    // The ilOffset should never be outside the array, but we don't 
    // assert it here because invalid code can generate bad requests
    // Value of '0' is treated as error code by the jit
    if ( ilOffset <= map_len )
      return map[ilOffset];
    else
      return 0;
}

/*map a pc offset to an il offset and a pc offset within the opcode, 
  returns -1 if il offset does not exist */
signed FJit_Encode::ilFromPC(unsigned pcOffset, unsigned* pcInILOffset) {
	map_len = decompress();
        unsigned low_i = 0, low = 0;
	signed high = -1;
	
        for (unsigned i = 0; i <= map_len; i ++ )
	{
          if ( map[i] == pcOffset )
            {
              if (pcInILOffset) *pcInILOffset = 0;
		 return i; 
	    }
          if ( map[i] < pcOffset && map[i] > low )
            {
              low = map[i]; low_i = i;
	    }
          if ( map[i] > pcOffset && map[i] < (unsigned)high )
              high = map[i]; 
	} 

        if (high == -1) {
	    //not in table
	    if (pcInILOffset) {
			*pcInILOffset = pcOffset;
	    }
	    return -1;
        }

        if (pcInILOffset) {
		*pcInILOffset = pcOffset - map[low_i];
	}
	return low_i; 
}

/* return the size of the compressed stream in bytes. */
unsigned FJit_Encode::compressedSize() {
	unsigned pcDelta;
	unsigned pcOffset = 0;
	unsigned current = 0;
	unsigned char* bytes = (unsigned char*) map;

	if (compressed) {
		return map_len;
	};

	// lift out the first entry so we don't overwrite it with the length
	pcDelta = map[current] - pcOffset;

	if (map_len) {
		encode(map_len, &bytes);
	}

	//since we are compressing in place, we need to be careful to not overwrite ourselves
	while (current < map_len ) {
		current++;
                unsigned buffer = map[current];
		encode(pcDelta, &bytes);
		_ASSERTE((size_t) bytes <= (size_t) &map[current+1]);
		pcOffset += pcDelta;
		pcDelta = buffer - pcOffset;
	}
        encode(pcDelta,&bytes);
	_ASSERTE((size_t) bytes <= (size_t) &map[current]);
	compressed = true;
	map_len = (unsigned)(bytes - (unsigned char*) map);
	return map_len;
}

/* compress the map into the supplied buffer.  Return true if successful */
bool FJit_Encode::compress(__in_ecount(buffer_len) unsigned char* buffer, unsigned buffer_len) {
	if (!compressed) {
		map_len = compressedSize();
	}
	if (map_len > buffer_len) {
		return false;
	}
	memcpy(buffer, map, map_len);
	return true;
}

/* decompress the internals if necessary. Answer the number of entries in the map */
unsigned FJit_Encode::decompress(){
	if (!compressed ) return map_len;

	//since we compressed in place, allocate a new map and then decompress.
	//Note, we are assuming that a map is rarely compressed and then decompressed
	//In fact, there is no known instance of this happening

	Fjit_il2pcMap* temp_map = map;
	map = NULL;
	map_len = map_capacity = 0;
	decompress((unsigned char*) temp_map);
	if(temp_map) delete [] temp_map;
	return map_len;
}


/* compress the bool* onto itself and answer the number of compressed bytes */
unsigned FJit_Encode::compressBooleans(__in_ecount(buffer_len) unsigned char* buffer, unsigned buffer_len) {
	unsigned len = 0;
	unsigned char* compressed = (unsigned char*) buffer;
	unsigned char bits;
	
	/*convert booleans to bits and pack into bytes */
	while (buffer_len >= 8) {
		bits = 0;
		for (unsigned i=0;i<8;i++) {
			bits = (bits>>1) + (*buffer++ ? 128 : 0);
		}
		*compressed++ = bits;
		len++;
		buffer_len -= 8;		
	}
	if (buffer_len) {
		bits = 0;
		unsigned zeroBits = 8;
		while (buffer_len--) {
			bits = (bits>>1) + (*buffer++ ? 128 : 0);
			zeroBits--;
		}
		*compressed++ = (bits >> zeroBits);
		len++;
	}
	return len;
}

/* answer the number of bytes it takes to encode an unsigned val */
unsigned FJit_Encode::encodedSize(unsigned val) {
	unsigned len = 0;
	do {
		len++;
	} while ((val = (val>>7)) > 0);
	return len;
}

/* decompress the bytes. Answer the number of entries in the map */
unsigned FJit_Encode::decompress(__in unsigned char* bytes) {
	unsigned needed;
	unsigned char* current = bytes;
	unsigned pcOffset = 0;
	needed = decode(&current)+1;
	if (map_capacity < needed) {
		if (map) delete [] map;
                map = new Fjit_il2pcMap[needed];
		if (map != NULL)
		{
		  map_capacity = needed;
                  map_len = needed - 1;
                }
                else
		{
                  map_capacity = 0;
                  map_len = 0;
		}
	}
	compressed = false;
	for (unsigned i = 0; i <= map_len; i++) {
		map[i] = pcOffset += decode(&current);
	}
	return map_len;
}

/*encode an unsigned, update the buffer ptr and return bytes written*/
unsigned FJit_Encode::encode(unsigned val, __deref_ecount(sizeof(val)) unsigned char** buffer) {
	unsigned len = 0;
	unsigned char bits;
	while (val > 0x7f) {
		bits = (val & 0x7f) | 0x80;
		val = val >> 7;
		**buffer = bits;
		(*buffer)++;
		len++;
	}
	**buffer = (unsigned char) val;
	(*buffer)++;
	return len+1;
}	

/*encode an OpType, update the buffer ptr and return bytes written*/
unsigned FJit_Encode::encodeOpType(OpType * pOpType, __deref_ecount(sizeof(*pOpType)) unsigned char** buffer)
{
   //
   // 0b0RTTTTTT is the bit layout of the encoded type
   // as long as the enum has less than 64 values, this is ok
   //
   _ASSERTE(((unsigned) pOpType->enum_()) < 0x40);
   unsigned encodedType = (pOpType->enum_() & 0x3F) | (pOpType->isReadOnly() << 6);
   unsigned bytesWritten = 0;
   bytesWritten += FJit_Encode::encode(encodedType, buffer);
   bytesWritten += FJit_Encode::encode(PtrToUint(pOpType->cls()), buffer);
   return bytesWritten;
}

void FJit_Encode::decodeOpType(OpType * pOpType, __in unsigned char ** buffer)
{
   unsigned decodedType = FJit_Encode::decode(buffer);
   _ASSERTE(decodedType <= 0x7F); // must be less than 7 bits
   ULONG_PTR enum_val = decodedType & 0x3F;
   bool isReadOnly = (decodedType >> 6) & 0x1;
   ULONG_PTR cls_val = FJit_Encode::decode(buffer);
   pOpType->init((OpTypeEnum)enum_val, (CORINFO_CLASS_HANDLE) cls_val, isReadOnly);
}

/*decode an unsigned, buffer ptr is incremented, callable from FJIT_EETwain */
unsigned FJit_Encode::decode_unsigned(__in unsigned char** buffer) {
	return decode(buffer);
}

/*decode an unsigned, buffer ptr is incremented */
unsigned FJit_Encode::decode(__in unsigned char** buffer) {
	unsigned val = 0;
	unsigned char bits;
	unsigned i = 0;
	do {
		bits = **buffer; 
		val = ((bits & 0x7f) << (7*i++)) + val;
		(*buffer)++;
	} while ( bits > 0x7f );
	return val;
}

#if !defined(SHARED_ENCODER_CODE)

//
// reportDebuggingData is called by FJit::reportDebuggingData to tell
// the encoding to report the IL to native map to the Runtime and
// debugger.
//

int __cdecl mappingCmp(const void* a_, const void* b_) {

    const ICorDebugInfo::OffsetMapping* a = (const ICorDebugInfo::OffsetMapping*) a_;
    const ICorDebugInfo::OffsetMapping* b = (const ICorDebugInfo::OffsetMapping*) b_;

    // Check for duplicate entries
    _ASSERTE( a->ilOffset !=  b->ilOffset );

    if ( a->nativeOffset >  b->nativeOffset )
      return 1;
    else if ( a->nativeOffset <  b->nativeOffset )
      return -1;
    else
      return( a->ilOffset >  b->ilOffset ? 1 : -1 ); 
}

void FJit_Encode::reportDebuggingData(ICorJitInfo* jitInfo, CORINFO_METHOD_HANDLE ftn,
                         UINT prologEnd, UINT epilogStart, LabelTable * stacks, bool DumpMap )
{
    // make sure to decompress the map. (shouldn't be compress yet anyway.)
    map_len = decompress();

    // The map should not be empty, and the first offset should be 0.
    _ASSERTE(map_len);
    
    // Create a table to pass the mappings back to the Debugger via
    // the Debugger's allocate method. Note: we're allocating a little
    // too much memory here, but its probably faster than determining
    // the number of valid IL offsets in the map.
    ICorDebugInfo::OffsetMapping *mapping = map_len > 0 ?
        (ICorDebugInfo::OffsetMapping*) jitInfo->allocateArray(
                                                        (map_len+1) *
                                                        sizeof(mapping[0])) : 
        NULL;

    if (mapping != NULL)
    {
        unsigned int lastNativeOffset = 0xFFFFFFFF;
        unsigned int j = 0;
        if ( map[0] > 0)
        {
            //Assume that all instructions before the IL are part of the
            //prolog
            mapping[j].ilOffset = (unsigned)ICorDebugInfo::PROLOG;
            mapping[j].nativeOffset = 0;
            mapping[j].source = ICorDebugInfo::SOURCE_TYPE_INVALID;
            j++;

            // If the function starts with a try block, the handlers will be jitted
            // before the try block. In this case the handlers are jitted in between
            // the prologEnd and map[0], so that the equality doesn't hold
            // _ASSERTE( map[0] == prologEnd );
        }
        
        for (unsigned int i = 0; i < map_len; i++)
        {
            if (map[i] != lastNativeOffset)
            {
                mapping[j].ilOffset = i; //map[i].ilOffset;
                mapping[j].nativeOffset = map[i];

                // Set a flag if the stack is empty
                unsigned int labelToken = stacks->findLabel(i);
                if ( labelToken != LABEL_NOT_FOUND && stacks->getStackSize(labelToken) == 0 )
		mapping[j].source = ICorDebugInfo::STACK_EMPTY;

                lastNativeOffset = map[i];
                j++;
            }
        }

        //mark the last block as epilog, since it is.
        if (j > 0)
        {
            j--;
            // mapping[j].nativeOffset++; //FJIT says epilog begins on instruction
            // FOLLOWING nativeOffset, debugger assumes that it starts on
            // instruction AT nativeOffset.
            // ASSERTE( mapping[j].nativeOffset == epilogStart);
            	
            mapping[j].ilOffset = (unsigned)ICorDebugInfo::EPILOG;
            mapping[j].source = ICorDebugInfo::SOURCE_TYPE_INVALID;
            j++;
        }
       
        // Sort the array before passing it to the debugger
        if ( j > 2 )
           qsort(&mapping[1], j-1, sizeof(ICorDebugInfo::OffsetMapping), mappingCmp);
        
#if defined(_DEBUG) 
        if ( DumpMap )
	{
	  for ( unsigned int i = 0; i < j; i ++ )
	    printf("Native %x IL %x \n", mapping[i].nativeOffset, mapping[i].ilOffset );
	}
#endif
        // Pass the offset array to the debugger.
        jitInfo->setBoundaries(ftn, j, mapping);
    }
}
#endif // SHARED_ENCODER_CODE
