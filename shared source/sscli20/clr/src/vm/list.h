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
// @File: list.h
//
// @commn: Bunch of utility classes
//     
// HISTORY:
//   02/03/98:                      	created helper classes
//						SLink, link node for singly linked list, every class that is intrusively
//								linked should have a data member of this type
//						SList, template linked list class, contains only inline 
//								methods for fast list operations, with proper type checking
//						DLink,  link node, every class that are intrusively linked
//								needs to have a data member of type DLink
//						DList:	Template linked list class, contains only inline methods
//							 hence NO CODE BLOAT, we get good type checking
//						Queue:	Queue implementation, based on TList
//						
//						see below for futher info. on how to use these template classes
//
//-----------------------------------------------------------------------------

//#ifndef _H_UTIL
//#error I am a part of util.hpp Please don't include me alone !
//#endif

#ifndef _H_LIST_
#define _H_LIST_

//------------------------------------------------------------------
// struct SLink, to use a singly linked list 
// have a data member of type SLink in your class
// and instantiate the template SList class, passing 
// in the offset to the SLink data member
//--------------------------------------------------------------------

typedef DPTR(struct SLink) PTR_SLink;

struct SLink
{
    PTR_SLink m_pNext;
    SLink() 
    {
        LEAF_CONTRACT;

        m_pNext = NULL;
    }

    void InsertAfter(SLink* pLinkToInsert)
    {
        LEAF_CONTRACT;
        PRECONDITION_MSG(NULL == pLinkToInsert->m_pNext, "This method does not support inserting lists");

        PTR_SLink pTemp = m_pNext;

        m_pNext = PTR_SLink(pLinkToInsert);
        pLinkToInsert->m_pNext = pTemp;
    }

    // find pLink within the list starting at pHead
    // if found remove the link from the list and return the link
    // otherwise return NULL
    static SLink* FindAndRemove(SLink *pHead, SLink* pLink, SLink ** ppPrior);
};

//------------------------------------------------------------------
// class SList, to use a singly linked list 
// have a data member of type SLink in your class
// and instantiate the template SList class, passing 
// in the offset to the SLink data member

// SList has two different behaviours depending on boolean
// fHead variable, 

// if fHead is true, then the list allows only InsertHead  operations
// if fHead is false, then the list allows only InsertTail operations
// the code is optimized to perform these operations
// all methods are inline, and conditional compiled based on template
// argument 'fHead'
// so there is no actual code size increase
//--------------------------------------------------------------
template <class T, int offset, bool fHead = false, typename __PTR = T*>
class SList
{
public:
    // typedef used by the Queue class below
    typedef T ENTRY_TYPE; 

protected:

    // used as sentinel 
    SLink  m_link; // slink.m_pNext == Null
    PTR_SLink m_pHead;
    PTR_SLink m_pTail;

    // get the list node within the object
    SLink* GetLink (T* pLink)
    {
        LEAF_CONTRACT;
        return ((SLink*)((UCHAR *)pLink + offset));
    }
    
    // move to the beginning of the object given the pointer within the object
    T* GetObject (SLink* pLink)
    {
        LEAF_CONTRACT;
        // if offset == 0, compiler will optimize this call 
        // to just return pLink, so if possible always put m_link
        // as the first data member
        if (offset == 0)
        {
            return (T*)__PTR(PTR_HOST_TO_TADDR(pLink));
        }
        else if (pLink == NULL)
        {
            return NULL;
        }
        else
        {
            return (T*)__PTR(PTR_HOST_TO_TADDR(pLink) - offset);         
        }
    }

public:

    SList()
    {
        WRAPPER_CONTRACT;
        Init();
    }

    void Init()
    {
        LEAF_CONTRACT;
        m_pHead = &m_link;
        // NOTE :: fHead variable is template argument 
        // the following code is a compiled in, only if the fHead flag
        // is set to false,
        if (!fHead)
        {
            m_pTail = &m_link;
        }
    }

    bool IsEmpty()
    {
        LEAF_CONTRACT;
        return m_pHead->m_pNext == NULL;
    }

    void InsertTail(T *pObj)
    {
        LEAF_CONTRACT;
        // NOTE : conditional compilation on fHead template variable
        if (!fHead)
        {
            _ASSERTE(pObj != NULL);
            SLink *pLink = GetLink(pObj);

            m_pTail->m_pNext = pLink;
            m_pTail = pLink;
        }
        else 
        {// you instantiated this class asking only for InsertHead operations
            _ASSERTE(0);
        }
    }
	
    void InsertHead(T *pObj)
    {
        LEAF_CONTRACT;
        // NOTE : conditional compilation on fHead template variable
        if (fHead)
        {
            _ASSERTE(pObj != NULL);
            SLink *pLink = GetLink(pObj);
            
            pLink->m_pNext = m_pHead->m_pNext;
            m_pHead->m_pNext = pLink;
        }
        else
        {// you instantiated this class asking only for InsertTail operations
            _ASSERTE(0);
        }
    }

    T*	RemoveHead()
    {
        LEAF_CONTRACT;
        SLink* pLink = m_pHead->m_pNext;
        if (pLink != NULL)
        {
            m_pHead->m_pNext = pLink->m_pNext;
        }
        // conditionally compiled, if the instantiated class
        // uses Insert Tail operations
        if (!fHead)
        {
            if(m_pTail == pLink)
            {
                m_pTail = m_pHead;
            }
        }

        return GetObject(pLink);
    }
    
    T*	GetHead()
    {
        WRAPPER_CONTRACT;
        return GetObject(m_pHead->m_pNext);
    }
    
    T*	GetTail()
    {
        WRAPPER_CONTRACT;

        // conditional compile
        if (fHead)
        {	// you instantiated this class asking only for InsertHead operations
            // you need to walk the list yourself to find the tail
            _ASSERTE(0);
        }
        return (m_pHead != m_pTail) ? GetObject(m_pTail) : NULL;
    }

    T *GetNext(T *pObj)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(pObj != NULL);
        return GetObject(GetLink(pObj)->m_pNext);
    }

    T* FindAndRemove(T *pObj)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(pObj != NULL);
                
        SLink   *prior;
        SLink   *ret = SLink::FindAndRemove(m_pHead, GetLink(pObj), &prior);
        
        if (ret == m_pTail)
            m_pTail = prior;
        
        return GetObject(ret);
    }
};




//----------------------------------------------------------------------------
// @class: DLink 
// @commn: Linked list implementation below uses DLink as the element
//			any class whose instance is going to be a part of a linked list 
//			should contain a data member of type DLink
//
//----------------------------------------------------------------------------
struct DLink
{
public:
	DLink* m_pPrev;	// pointer to the prev node
    DLink* m_pNext;	// pointer to next node

	
	DLink()			//@constructor
	{
        WRAPPER_CONTRACT;

		Init();
	}

    void Init ()		// initialize pointers to NULL
	{
        LEAF_CONTRACT;
		m_pPrev = m_pNext = NULL;
	}

	// insert self node after pLink
	//static void InsertAfter (DLink* pPrev, DLink *pLink);
	//static DLink* Find(DLink* pHead, DLink* pLink);
	static void Remove(DLink* pLink);
};


//----------------------------------------------------------------------------
// Template class: DList 
//		
//     This templated class is an implementation of intrusive linked lists
//     i.e. a linked list where the storage for the linked fields is within the
//     structure to be linked itself. Class DList is actually a template,
//     taking two arguments:
//     1. type T: implements a link-list of pointers to T
//     2. offset: represents the offset of the link within the class T. 
//
//     Sample usage of this class is as follows:
// 
//     To make a linked list of Foo, the class Foo should look like this
//     class Foo 
//     {
//         .....
//         DLink m_Node;
//         .....
//     }
//  
//     Now to declare a linked list, just do the following:
//
//     typedef DList<Foo, offsetof(Foo, m_Node)> FooList;
// 
// NOTES: All methods in the class are inline, they do minimal work
//		  the base class DList provides the actual implementation
//		  so there isn't any code bloat due to use of templates
//		  Advantages: you get properly typed list 
//----------------------------------------------------------------------------
template <class T, int offset>
class DList 
{
	// node to keep track of the head and tail
	DLink		m_Base;    
public:

	// typedef used by the Queue class below
	typedef T ENTRY_TYPE; 

	// Init
    void Init ()
	{
        LEAF_CONTRACT;
		m_Base.m_pNext = &m_Base;
		m_Base.m_pPrev = &m_Base;
	}
    
	//@method: IsEmpty (), check if list is empty
    bool IsEmpty () const
	{
        LEAF_CONTRACT;
		return (m_Base.m_pNext == &m_Base);
	}

	// InsertHead
    void InsertHead (ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		DLink *pLink = GetLink (pObj);
		_ASSERTE(pLink);
		InsertHead(pLink);
	}

	// InsertTail
    void InsertTail (ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		DLink *pLink = GetLink (pObj);
		_ASSERTE(pLink);
		InsertTail(pLink);
	}

	// InsertAfter
    void InsertAfter (ENTRY_TYPE* pPrevObj,ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		DLink *pLink = GetLink (pObj);
		DLink *pPrev = GetLink (pPrevObj);
		_ASSERTE(pLink);
		_ASSERTE(pPrev);
		InsertAfter (pPrev, pLink);
	}

	// InsertBefore
    void InsertBefore (ENTRY_TYPE* pCurr,ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		_ASSERTE(pCurr);
		DLink *pLink = GetLink (pObj);
		DLink *pPrev = GetPrevLink (GetLink(pCurr));
		_ASSERTE(pLink);
		
		if (pPrev == NULL)
			InsertHead(pLink);
		else
			InsertAfter (pPrev,pLink);
	}

	// Remove
    void Remove (ENTRY_TYPE* pLink)
	{
        WRAPPER_CONTRACT;

		DLink::Remove (GetLink(pLink));
	}
    
	// GetNext
    ENTRY_TYPE* GetNext (ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		DLink *pLink = GetLink(pObj);
		_ASSERTE(pLink);
		return GetObject(GetNextLink(pLink));

	}

	// GetPrev
    ENTRY_TYPE* GetPrev (ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		DLink *pLink = GetLink(pObj);
		_ASSERTE(pLink);
		return GetObject(GetPrevLink(pLink));
	}
   
	// GetHead
    ENTRY_TYPE* GetHead ()
	{
        WRAPPER_CONTRACT;

		return GetObject(GetHeadLink());
	}
	
	// GetTail
    ENTRY_TYPE* GetTail ()
	{
        WRAPPER_CONTRACT;

		return GetObject(GetTailLink());
	}

	// RemoveHead
	ENTRY_TYPE* RemoveHead ()
	{
        WRAPPER_CONTRACT;

		DLink* pLink = GetHeadLink();
		//check for null
		if (pLink != NULL)
			DLink::Remove (pLink);
		return GetObject(pLink);
	}
    
	// RemoveTail
    ENTRY_TYPE* RemoveTail ()
	{
        WRAPPER_CONTRACT;

		DLink* pLink = GetTailLink();
		//check for null
		if (pLink != NULL)
			DLink::Remove (pLink);
		return GetObject(pLink);
	}

	ENTRY_TYPE* Find(ENTRY_TYPE* pObj)
	{
        WRAPPER_CONTRACT;

		_ASSERTE(pObj);
		DLink *pLink = GetLink(pObj);
		_ASSERTE(pLink);
		return GetObject(Find(pLink));
	}

	ENTRY_TYPE*	FindAndRemove(ENTRY_TYPE *pObj)
	{
        WRAPPER_CONTRACT;

		_ASSERTE(pObj);
		DLink *pLink = GetLink(pObj);
		_ASSERTE(pLink);
		pLink = Find(pLink);
		if (pLink)
			DLink::Remove(pLink);
		return GetObject(pLink);
	}

protected:

	void InsertHead (DLink* pLink);
	void InsertTail (DLink *pLink);
	void InsertAfter (DLink* pPrev, DLink* pLink);

	DLink* Find(DLink* pLink)
		{
            WRAPPER_CONTRACT;

			DLink* currNode = GetHeadLink();

			// Search the list for the given element
			// and remove.  
			//
			while ( currNode && currNode != pLink )
			{
				currNode = GetNextLink(currNode);
			}

			return currNode;
		}

	DLink* GetHeadLink()
	{
        WRAPPER_CONTRACT;

		return (IsEmpty ()) ? NULL : m_Base.m_pNext;
	}

	DLink* GetTailLink()
	{
        WRAPPER_CONTRACT;

		return (IsEmpty ()) ? NULL : m_Base.m_pPrev;
	}

	DLink* GetNextLink(DLink* pLink)
	{
        LEAF_CONTRACT;

		return (pLink->m_pNext == &m_Base) ? NULL : pLink->m_pNext;
	}

	DLink* GetPrevLink(DLink* pLink)
	{
        LEAF_CONTRACT;

		return (pLink->m_pPrev == &m_Base) ? NULL : pLink->m_pPrev;
	}

	// get the list node within the object
    DLink* GetLink (ENTRY_TYPE* pObj)
	{
        LEAF_CONTRACT;

		return ((DLink*)((UCHAR *)pObj + offset));
	}
    
	// move to the beginning of the object given the pointer within the object
	ENTRY_TYPE* GetObject (DLink* pLink)
	{
        LEAF_CONTRACT;

		return (offset && (pLink == NULL)) // check for null
				? NULL 
				: (ENTRY_TYPE*)((UCHAR *)pLink - offset);
	}

};



//----------------------------------------------------------------------------
// DList::InsertHead   
//		Insert node at the head of list. 
//
template <class T, int offset>
void
DList<T,offset>::InsertHead (DLink* pLink)
{
    LEAF_CONTRACT;

	_ASSERTE (pLink->m_pNext == NULL);
	_ASSERTE (pLink->m_pPrev == NULL);

    pLink->m_pNext = m_Base.m_pNext;
    m_Base.m_pNext->m_pPrev = pLink;
    
    m_Base.m_pNext = pLink;
    pLink->m_pPrev = &m_Base;
    
} // DList::InsertHead ()


//----------------------------------------------------------------------------
// DList::InsertTail   
//		Insert node at the tail of the list.
// 
template <class T, int offset>
void
DList<T,offset>::InsertTail (DLink *pLink)
{
    LEAF_CONTRACT;

	_ASSERTE (pLink->m_pNext == NULL);
	_ASSERTE (pLink->m_pPrev == NULL);

    pLink->m_pPrev = m_Base.m_pPrev;
    m_Base.m_pPrev->m_pNext = pLink;
    
    m_Base.m_pPrev = pLink;
    pLink->m_pNext = &m_Base;
    
} // DList::InsertTail ()


//----------------------------------------------------------------------------
// DList::InsertAfter   
//  
//
template <class T, int offset>
void 
DList<T,offset>::InsertAfter (DLink* pPrev,DLink* pLink)
{
    LEAF_CONTRACT;

    DLink* pNext;
    _ASSERTE (pPrev != NULL);
	_ASSERTE (pLink->m_pNext == NULL);
	_ASSERTE (pLink->m_pPrev == NULL);

    pNext = pPrev->m_pNext;
    
    pLink->m_pNext = pNext;
    pNext->m_pPrev = pLink;
    
    pPrev->m_pNext = pLink;
    pLink->m_pPrev = pPrev;

} // DList::InsertAfter ()


//----------------------------------------------------------------------------
// class: Queue 
//
//     This class implements a queue using the linked list implementation
//     provided by template TList class, the data members of the queue
//	   are of type T, the template class TList should also be of type T.
//		TList can be either a DList or an SList which supports InsertHead operations.
//
// NOTES: All methods in the class are inline, they do minimal work
//		  the m_list data member provides the actual implementation
//		  so there isn't any code bloat due to use of templates
//		  ADVANTAGES: you get properly typed queue 
//
//----------------------------------------------------------------------------

template <class TList>
class Queue
{
	// entry type depends on the type of the list
	typedef typename TList::ENTRY_TYPE ENTRY_TYPE;

	// length of the queue
	unsigned	m_len;

public:

	void	Init()
		{
            WRAPPER_CONTRACT;

			m_list.Init ();
		}

    void	Enqueue (ENTRY_TYPE* pLink)
		{
            WRAPPER_CONTRACT;

			m_len++;
			m_list.InsertTail(pLink);
		}

    ENTRY_TYPE*		Dequeue ()
		{
            WRAPPER_CONTRACT;

			ENTRY_TYPE* pObj = m_list.RemoveHead();
			if (pObj != NULL)
			{
				_ASSERTE(m_len > 0);
				m_len--;
			}
			return pObj;
		}

	ENTRY_TYPE*		FindAndRemove(ENTRY_TYPE* pLink)
		{
            WRAPPER_CONTRACT;

			ENTRY_TYPE* p = m_list.FindAndRemove(pLink);
			if (p != NULL)
			{
				_ASSERTE(m_len > 0);
				m_len--;
			}
			return p;
		}

    ENTRY_TYPE*		GetNext (ENTRY_TYPE* pLink)
	{
        WRAPPER_CONTRACT;

		return m_list.GetNext (pLink);
	}

    ENTRY_TYPE*		GetHead ()
	{
        WRAPPER_CONTRACT;

		return m_list.GetHead ();
	}

    ENTRY_TYPE*		Tail ()
	{
        WRAPPER_CONTRACT;

		return m_list.GetTail();
	}

    bool	IsEmpty ()
	{
        WRAPPER_CONTRACT;

		return m_list.IsEmpty ();
	}

	unsigned Length()
	{
        LEAF_CONTRACT;

		return m_len;
	}

private:
    TList	m_list;
};

#endif // _H_LIST_

// End of file: list.h
