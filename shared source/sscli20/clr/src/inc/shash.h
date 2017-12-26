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

#ifndef _SHASH_H_
#define _SHASH_H_

#include "utilcode.h" // for string hash functions
#include "clrtypes.h"
#include "check.h"
#include "iterator.h"


//


//
//
//                                              
//
//
//
//
//
//
//


template < typename ELEMENT > 
class DefaultSHashTraits
{
  public:
    typedef COUNT_T count_t;
    typedef ELEMENT element_t;

    static const COUNT_T s_growth_factor_numerator = 3;
    static const COUNT_T s_growth_factor_denominator = 2;

    static const COUNT_T s_density_factor_numerator = 3;
    static const COUNT_T s_density_factor_denominator = 4;

    static const COUNT_T s_minimum_allocation = 7;

    static const bool s_supports_remove = true;

    static const ELEMENT Null() { return (const ELEMENT) 0; }
    static const ELEMENT Deleted() { return (const ELEMENT) -1; }
    static bool IsNull(const ELEMENT &e) { return e == (const ELEMENT) 0; }
    static bool IsDeleted(const ELEMENT &e) { return e == (const ELEMENT) -1; }

    // No defaults - must specify:
    // 
    // typedef key_t;
    // static key_t GetKey(const element_t &i);
    // static BOOL Equals(key_t k1, key_t k2);
    // static count_t Hash(key_t k);
};

// Hash table class definition

template <typename TRAITS>
class SHash : public TRAITS
{
  private:
    class Index;
    friend class Index;

    class KeyIndex;
    friend class KeyIndex;

  public:
    // explicitly declare local typedefs for these traits types, otherwise 
    // the compiler may get confused
    typedef typename TRAITS::element_t element_t;
    typedef typename TRAITS::key_t key_t;
    typedef typename TRAITS::count_t count_t;

    class Iterator;
    class KeyIterator;

    // Constructor/destructor.  SHash tables always start out empty, with no
    // allocation overhead.  Call Reallocate to prime with an initial size if
    // desired.

    SHash();
    ~SHash();

    // Lookup an element in the table by key.  Returns NULL if no element in the table
    // has the given key.  Note that multiple entries for the same key may be stored - 
    // this will return the first element added.  Use KeyIterator to find all elements
    // with a given key.

    element_t Lookup(key_t key) const;

    // Pointer-based flavor of Lookup (allows efficient access to tables of structures)

    const element_t* LookupPtr(key_t key) const;

    // Add an element to the hash table.  This will never replace an element; multiple
    // elements may be stored with the same key.

    void Add(const element_t &element);

    // Remove the first element matching the key from the hash table.  

    void Remove(key_t key);

    // Remove the specific element.

    void Remove(Iterator& i);
    void Remove(KeyIterator& i);

    // Pointer-based flavor of Remove (allows efficient access to tables of structures)

    void RemovePtr(element_t * element);

    // Remove all elements in the hashtable

    void RemoveAll();

    // Begin and End pointers for iteration over entire table. 

    Iterator Begin() const;
    Iterator End() const;

    // Begin and End pointers for iteration over all elements with a given key.

    KeyIterator Begin(key_t key) const;
    KeyIterator End(key_t key) const;

    // Return the number of elements currently stored in the table

    count_t GetCount() const; 

    // Resizes a hash table for growth.  The new size is computed based
    // on the current population, growth factor, and maximum density factor.

    void Grow();

    // Reallocates a hash table to a specific size.  The size must be big enough
    // to hold all elements in the table appropriately.  
    //
    // Note that the actual table size must always be a prime number; the number
    // passed in will be upward adjusted if necessary.

    void Reallocate(count_t newTableSize);

  private:

    // See if it is OK to grow the hash table by one element.  If not, reallocate
    // the hash table.

    BOOL CheckGrowth();

    // Utility function to add a new element to the hash table.  Note that
    // it is perfectly find for the element to be a duplicate - if so it
    // is added an additional time. Returns TRUE if a new empty spot was used;
    // FALSE if an existing deleted slot.

    static BOOL Add(element_t *table, count_t tableSize, const element_t &element);

    // Utility function to find the first element with the given key in 
    // the hash table.

    static const element_t* Lookup(element_t *table, count_t tableSize, key_t key);

    // Utility function to remove the first element with the given key
    // in the hash table.

    void Remove(element_t *table, count_t tableSize, key_t key);

    // Utility function to remove the specific element.

    void RemoveElement(element_t *table, count_t tableSize, element_t *element);

    // Index for whole table iterator.  This is also the base for the keyed iterator.

    class Index : public CheckedIteratorBase< SHash<TRAITS> >
    {
        friend class SHash;
        friend class Iterator;
        friend class Enumerator<const element_t, Iterator>;

        // The methods implementation has to be here for portability
        // Some compilers won't compile the separate implementation in shash.inl
      protected:

        element_t *m_table;
        count_t m_tableSize;
        count_t m_index;

        Index(const SHash *hash, BOOL begin)
        : m_table(hash->m_table),
            m_tableSize(hash->m_tableSize),
            m_index(begin ? 0 : m_tableSize)
        {
            LEAF_CONTRACT;
        }

        const element_t &Get() const
        {
            LEAF_CONTRACT;

            return m_table[m_index];
        }

        void First()
        {
            LEAF_CONTRACT;

            if (m_index < m_tableSize)
                if (IsNull(m_table[m_index]) || IsDeleted(m_table[m_index]))
                    Next();
        }

        void Next()
        {
            LEAF_CONTRACT;

            if (m_index >= m_tableSize)
                return;
            
            for (;;)
            {
                m_index++;
                if (m_index >= m_tableSize)
                    break;
                if (!IsNull(m_table[m_index]) && !IsDeleted(m_table[m_index]))
                    break;
            }
        }

        BOOL Equal(const Index &i) const
        { 
            LEAF_CONTRACT;

            return i.m_index == m_index; 
        }

        CHECK DoCheck() const
        {
            CHECK_OK;
        }
    };

    class Iterator : public Index, public Enumerator<const element_t, Iterator>
    {
        friend class SHash;

      public:
        Iterator(const SHash *hash, BOOL begin)
          : Index(hash, begin)
        {
        }
    };

    // Index for iterating elements with a given key.  
    //
    // Note that the m_index field
    // is artificially bumped to m_tableSize when the end of iteration is reached.
    // This allows a canonical End iterator to be used.

    class KeyIndex : public Index
    {
        friend class SHash;
        friend class KeyIterator;
        friend class Enumerator<const element_t, KeyIterator>;

        // The methods implementation has to be here for portability
        // Some compilers won't compile the separate implementation in shash.inl
      protected:
        key_t       m_key;
        count_t     m_increment;

        KeyIndex(const SHash *hash, BOOL begin)
        : Index(hash, begin),
            m_increment(0)
        {
            LEAF_CONTRACT;
        }

        void SetKey(key_t key)
        {
            LEAF_CONTRACT;

            if (m_tableSize > 0)
            {
                m_key = key;
                count_t hash = Hash(key);

                m_index = hash % m_tableSize;
                m_increment = (hash % (m_tableSize-1)) + 1;

                // Find first valid element
                if (IsNull(m_table[m_index]))
                    m_index = m_tableSize;
                else if (IsDeleted(m_table[m_index])
                        || !Equals(m_key, GetKey(m_table[m_index])))
                    Next();
            }
        }

        void Next()
        {
            LEAF_CONTRACT;

            while (TRUE)
            {
                m_index += m_increment;
                if (m_index >= m_tableSize)
                    m_index -= m_tableSize;

                if (IsNull(m_table[m_index]))
                {
                    m_index = m_tableSize;
                    break;
                }

                if (!IsDeleted(m_table[m_index])
                        && Equals(m_key, GetKey(m_table[m_index])))
                {
                    break;
                }
            }
        }
    };

    class KeyIterator : public KeyIndex, public Enumerator<const element_t, KeyIterator>
    {
        friend class SHash;

      public:

        operator Iterator &()
        {
            return *(Iterator*)this;
        }

        operator const Iterator &()
        {
            return *(const Iterator*)this;
        }

        KeyIterator(const SHash *hash, BOOL begin)
          : KeyIndex(hash, begin)
        {
        }
    };

    // Test for prime number.
    static BOOL IsPrime(COUNT_T number);

    // Find the next prime number >= the given value.  

    static COUNT_T NextPrime(COUNT_T number);

    // Instance members

    element_t   *m_table;               // pointer to table
    count_t     m_tableSize;            // allocated size of table
    count_t     m_tableCount;           // number of elements in table
    count_t     m_tableOccupied;        // number, includes deleted slots
    count_t     m_tableMax;             // maximum occupied count before reallocating
};

// disables support for removing elements - produces slightly faster implementation

template <typename PARENT>
class NoRemoveSHashTraits : public PARENT
{
public:
    // explicitly declare local typedefs for these traits types, otherwise 
    // the compiler may get confused
    typedef typename PARENT::element_t element_t;
    typedef typename PARENT::count_t count_t;

    static const bool s_supports_remove = false;
    static const element_t Deleted() { UNREACHABLE(); }
    static bool IsDeleted(const element_t &e) { return false; }
};

// PtrHashTraits is a template to provides useful defaults for pointer hash tables
// It relies on methods GetKey and Hash defined on ELEMENT

template <typename ELEMENT, typename KEY> 
class PtrSHashTraits : public DefaultSHashTraits<ELEMENT *>
{
  public:

    // explicitly declare local typedefs for these traits types, otherwise 
    // the compiler may get confused
    typedef DefaultSHashTraits<ELEMENT *> PARENT;
    typedef typename PARENT::element_t element_t;
    typedef typename PARENT::count_t count_t;

    typedef KEY key_t;

    static key_t GetKey(const element_t &e) 
    { 
        WRAPPER_CONTRACT;
        return e->GetKey(); 
    }
    static BOOL Equals(key_t k1, key_t k2) 
    { 
        LEAF_CONTRACT;
        return k1 == k2; 
    }
    static count_t Hash(key_t k) 
    { 
        WRAPPER_CONTRACT;
        return ELEMENT::Hash(k);
    }
};

template <typename ELEMENT, typename KEY>
class PtrSHash : public SHash< PtrSHashTraits<ELEMENT, KEY> >
{
};

// STRHashTraits and WSTRHashTraits are traits classes useful for string-keyed
// pointer hash tables.  They relies only on GetKey defined on ELEMENT.

template <typename ELEMENT> 
class StringSHashTraits : public PtrSHashTraits<ELEMENT, const char *>
{
  public:

    // explicitly declare local typedefs for these traits types, otherwise 
    // the compiler may get confused
    typedef PtrSHashTraits<ELEMENT, const char *> PARENT;
    typedef typename PARENT::element_t element_t;
    typedef typename PARENT::key_t key_t;
    typedef typename PARENT::count_t count_t;

    static BOOL Equals(key_t k1, key_t k2) 
    { 
        LEAF_CONTRACT;

        if (k1 == NULL && k2 == NULL)
            return TRUE;
        if (k1 == NULL || k2 == NULL)
            return FALSE;
        return strcmp(k1, k2) == 0; 
    }
    static count_t Hash(key_t k) 
    { 
        LEAF_CONTRACT;

        if (k == NULL)
            return 0;
        else
            return HashStringA(k); 
    }
};

template <typename ELEMENT>
class StringSHash : public SHash< StringSHashTraits<ELEMENT> >
{
};

template <typename ELEMENT> 
class WStringSHashTraits : public PtrSHashTraits<ELEMENT, const WCHAR *>
{
  public:
    // explicitly declare local typedefs for these traits types, otherwise 
    // the compiler may get confused
    typedef PtrSHashTraits<ELEMENT, const WCHAR *> PARENT;
    typedef typename PARENT::element_t element_t;
    typedef typename PARENT::key_t key_t;
    typedef typename PARENT::count_t count_t;

    static BOOL Equals(key_t k1, key_t k2) 
    { 
        LEAF_CONTRACT;

        if (k1 == NULL && k2 == NULL)
            return TRUE;
        if (k1 == NULL || k2 == NULL)
            return FALSE;
        return wcscmp(k1, k2) == 0; 
    }
    static count_t Hash(key_t k) 
    { 
        LEAF_CONTRACT;

        if (k == NULL)
            return 0;
        else
            return HashString(k); 
    }
};

template <typename ELEMENT>
class WStringSHash : public SHash< WStringSHashTraits<ELEMENT> >
{
};

#include "shash.inl"

#endif // _SHASH_H_
