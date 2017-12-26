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
//*****************************************************************************
// TokenMapper.h
//
// This helper class tracks mapped tokens from their old value to the new value
// which can happen when the data is optimized on save.
//
//*****************************************************************************
#ifndef __TokenMapper_h__
#define __TokenMapper_h__

//#include "utilcode.h"

typedef CDynArray<mdToken> TOKENMAP;

class TokenMapper
{
public:

	enum
	{
		tkixMethodDef,
		tkixMemberRef,
		tkixFieldDef,

		MAX_TOKENMAP
	};

	static int IndexForType(mdToken tk);

//*****************************************************************************
// Called by the meta data engine when a token is remapped to a new location.
// This value is recorded in the m_rgMap array based on type and rid of the
// from token value.
//*****************************************************************************
	HRESULT Map(mdToken tkImp, mdToken tkEmit);

//*****************************************************************************
// Check the given token to see if it has moved to a new location.  If so,
// return true and give back the new token.
//*****************************************************************************
	int HasTokenMoved(mdToken tkFrom, mdToken &tkTo);

	int GetMaxMapSize() const
	{ return (MAX_TOKENMAP); }

	TOKENMAP *GetMapForType(mdToken tkType);

protected:
// m_rgMap is an array indexed by token type.  For each type, an array of
// tokens is kept, indexed by from rid.  To see if a token has been moved,
// do a lookup by type to get the right array, then use the from rid to
// find the to rid.
	TOKENMAP	m_rgMap[MAX_TOKENMAP];
};

#endif // __CeeGenTokenMapper_h__
