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
//-----------------------------------------------------------------------------
// @File: list.cpp
//
// @commn: Bunch of utility classes
//     
// HISTORY:
//   02/03/98:                    :	created helper classes
//						DLink,  link node, every class that are intrusively linked
//								needs to have a data member of type DLink
//						DList:	Base list class contains the base implementation of all 
//								methods
//						TList:	Template linked list class, delegates all method calls
//								 to DList class, hence NO CODE BLOAT, we get good type checking
//						Queue:	Queue implementation, based on TList
//						
//						see below for futher info. on how to use these template classes
//
//-----------------------------------------------------------------------------

#include "common.h"

#include "list.h"

//----------------------------------------------------------------------------
// SLink::FindAndRemove(SLink *pHead, SLink* pLink)  
//		Find and remove
//----------------------------------------------------------------------------
SLink* SLink::FindAndRemove(SLink *pHead, SLink* pLink, SLink** ppPrior)
{
    LEAF_CONTRACT;

	_ASSERTE(pHead != NULL);
	_ASSERTE(pLink != NULL);

	SLink* pFreeLink = NULL;
    *ppPrior = NULL;

	while (pHead->m_pNext != NULL)
	{
		if (pHead->m_pNext == pLink)
		{
			pFreeLink = pLink;
			pHead->m_pNext = pLink->m_pNext;
            *ppPrior = pHead;
            break;
		}
        pHead = pHead->m_pNext;
	}
	
	return pFreeLink;
}


//----------------------------------------------------------------------------
//	void DLink::Remove (DLink* pLink)  
//		Remove the node from the list
//		the node has to be part of the circular list

void DLink::Remove (DLink* pLink)
{
    LEAF_CONTRACT;

	_ASSERTE(pLink != NULL);
	_ASSERTE(pLink->m_pNext != NULL);
	_ASSERTE(pLink->m_pPrev != NULL);

    DLink* pNext = pLink->m_pNext;
    DLink* pPrev = pLink->m_pPrev;

    pPrev->m_pNext = pNext;
    pNext->m_pPrev = pPrev;
    
    pLink->m_pNext = NULL;
    pLink->m_pPrev = NULL;

} // DList::Remove 
 



