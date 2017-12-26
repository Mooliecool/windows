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

#ifndef __objectlist_h__
#define __objectlist_h__


#include "arraylist.h"
#include "holder.h"

#define INVALID_COMPRESSEDSTACK_INDEX ((DWORD)-1)
#ifdef _DEBUG
#define FREE_LIST_SIZE 128
#else
#define FREE_LIST_SIZE 1024
#endif



class ObjectList
{
public:
	class Iterator 
    	{
		friend class ObjectList;

		  protected:
		   ArrayList::Iterator _iter;

		  public:
			
		void *GetElement() 
		{
			LEAF_CONTRACT; 
			PVOID ptr = _iter.GetElement();
			if (((DWORD)(size_t)ptr & 0x1) == 0)
			{
				return ptr;
			}
			else
			{
				return NULL;			
			}
		}
			
		DWORD GetIndex() 
		{
			LEAF_CONTRACT; 
			return _iter.GetIndex(); 
		}

		BOOL Next()
		{
			LEAF_CONTRACT; 
			return _iter.Next(); 
		}
   	};

	ObjectList() DAC_EMPTY();

	DWORD AddToList( PVOID ptr );
	void RemoveFromList( PVOID ptr );
	void RemoveFromList( DWORD index, PVOID ptr );
	PVOID Get( DWORD index );

	ObjectList::Iterator Iterate()
	{
		LEAF_CONTRACT;
		ObjectList::Iterator i;
		i._iter = this->allEntries_.Iterate();
		return i;
	}

private:
    ArrayList allEntries_;
    DWORD freeIndexHead_;
    Crst listLock_;
};

class UnsynchronizedBlockAllocator
{
public:
    UnsynchronizedBlockAllocator( size_t blockSize );
    ~UnsynchronizedBlockAllocator( void );

    PVOID Allocate( size_t size );

private:
    ArrayList blockList_;

    size_t blockSize_;
    size_t offset_;
    DWORD index_;

};

#endif // __objectlist_h__
