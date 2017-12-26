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

#ifndef _SHASH_INL_
#define _SHASH_INL_

template <typename TRAITS>
SHash<TRAITS>::SHash()
  : m_table(NULL),
    m_tableSize(0),
    m_tableCount(0),
    m_tableOccupied(0),
    m_tableMax(0)
{
    LEAF_CONTRACT;

#ifndef __GNUC__ // these crash GCC
    COMPILE_TIME_ASSERT(s_growth_factor_numerator > s_growth_factor_denominator);
    COMPILE_TIME_ASSERT(s_density_factor_numerator < s_density_factor_denominator);
#endif
}

template <typename TRAITS>
SHash<TRAITS>::~SHash()
{
    LEAF_CONTRACT;

    delete [] m_table;
}

template <typename TRAITS>
typename SHash<TRAITS>::count_t SHash<TRAITS>::GetCount() const 
{
    LEAF_CONTRACT;

    return m_tableCount;
}

template <typename TRAITS>
typename SHash< TRAITS>::element_t SHash<TRAITS>::Lookup(key_t key) const
{
    CONTRACT(element_t)
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        POSTCONDITION(IsNull(RETVAL) || Equals(key, GetKey(RETVAL)));
    }
    CONTRACT_END;

    const element_t *pRet = Lookup(m_table, m_tableSize, key);
    RETURN ((pRet != NULL) ? (*pRet) : Null());
}

template <typename TRAITS>
const typename SHash< TRAITS>::element_t* SHash<TRAITS>::LookupPtr(key_t key) const
{
    CONTRACT(const element_t *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        POSTCONDITION(RETVAL == NULL || Equals(key, GetKey(*RETVAL)));
    }
    CONTRACT_END;

    RETURN Lookup(m_table, m_tableSize, key);
}

template <typename TRAITS>
void SHash<TRAITS>::Add(const element_t &element)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        POSTCONDITION(Equals(GetKey(element), GetKey(*LookupPtr(GetKey(element)))));
    }
    CONTRACT_END;

    CheckGrowth();

    if (Add(m_table, m_tableSize, element))
        m_tableOccupied++;
    m_tableCount++;

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::Remove(key_t key)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(s_supports_remove);
        PRECONDITION(!(IsNull(Lookup(key))));
    }
    CONTRACT_END;

    Remove(m_table, m_tableSize, key);

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::Remove(Iterator& i)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(s_supports_remove);
        PRECONDITION(!(IsNull(*i)));
        PRECONDITION(!(IsDeleted(*i)));
    }
    CONTRACT_END;

    RemoveElement(m_table, m_tableSize, (element_t*)&(*i));

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::Remove(KeyIterator& i)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(s_supports_remove);
        PRECONDITION(!(IsNull(*i)));
        PRECONDITION(!(IsDeleted(*i)));
    }
    CONTRACT_END;

    RemoveElement(m_table, m_tableSize, (element_t*)&(*i));

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::RemovePtr(element_t * p)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(s_supports_remove);
        PRECONDITION(!(IsNull(*p)));
        PRECONDITION(!(IsDeleted(*p)));
    }
    CONTRACT_END;

    RemoveElement(m_table, m_tableSize, p);

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::RemoveAll()
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    }
    CONTRACT_END;

    delete [] m_table;

    m_table = NULL;
    m_tableSize = 0;
    m_tableCount = 0;
    m_tableOccupied = 0;
    m_tableMax = 0;

    RETURN;
}

template <typename TRAITS>
typename SHash<TRAITS>::Iterator SHash<TRAITS>::Begin() const
{
    LEAF_CONTRACT;

    Iterator i(this, TRUE);
    i.First();
    return i;
}

template <typename TRAITS>
typename SHash<TRAITS>::Iterator SHash<TRAITS>::End() const
{
    LEAF_CONTRACT;

    return Iterator(this, FALSE);
}

template <typename TRAITS>
typename SHash<TRAITS>::KeyIterator SHash<TRAITS>::Begin(key_t key) const
{
    LEAF_CONTRACT;

    KeyIterator k(this, TRUE);
    k.SetKey(key);
    return k;
}

template <typename TRAITS>
typename SHash<TRAITS>::KeyIterator SHash<TRAITS>::End(key_t key) const
{
    LEAF_CONTRACT;

    return KeyIterator(this, FALSE);
}

template <typename TRAITS>
BOOL SHash<TRAITS>::CheckGrowth()
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    }
    CONTRACT_END;

    if (m_tableOccupied == m_tableMax)
    {
        Grow();
        RETURN TRUE;
    }
        
    RETURN FALSE;
}

template <typename TRAITS>
void SHash<TRAITS>::Grow()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
    }
    CONTRACT_END;

    count_t newSize = (count_t) (m_tableCount 
                                 * s_growth_factor_numerator / s_growth_factor_denominator
                                 * s_density_factor_denominator / s_density_factor_numerator);
    if (newSize < s_minimum_allocation)
        newSize = s_minimum_allocation;

    // handle potential overflow
    if (newSize < m_tableCount)
        ThrowOutOfMemory();

    Reallocate(newSize);

    RETURN;
}

template <typename TRAITS>
void SHash<TRAITS>::Reallocate(count_t newTableSize)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INSTANCE_CHECK;
        PRECONDITION(newTableSize >= 
                     (count_t) (GetCount() * s_density_factor_denominator / s_density_factor_numerator));
    }
    CONTRACT_END;

    // Allocation size must be a prime number.  This is necessary so that hashes uniformly
    // distribute to all indices, and so that chaining will visit all indices in the hash table.
    newTableSize = NextPrime(newTableSize);

    element_t *newTable = new element_t [newTableSize];

    element_t *p = newTable, *pEnd = newTable + newTableSize;
    while (p < pEnd)
    {
        *p = Null();
        p++;
    }

    // Move all entries over to new table.

    for (Iterator i = Begin(), end = End(); i != end; i++)
    {
        if (!IsNull(*i) && !IsDeleted(*i))
            Add(newTable, newTableSize, *i);
    }


    delete [] m_table;

    m_table = newTable;
    m_tableSize = newTableSize;
    m_tableMax = (count_t) (newTableSize * s_density_factor_numerator / s_density_factor_denominator);
    m_tableOccupied = m_tableCount;

    RETURN;
}

template <typename TRAITS>
const typename SHash<TRAITS>::element_t * SHash<TRAITS>::Lookup(element_t *table, count_t tableSize, key_t key)
{
    CONTRACT(const element_t *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(RETVAL == NULL || Equals(key, GetKey(*RETVAL)));
    }
    CONTRACT_END;

    if (tableSize == 0)
        RETURN NULL;

    count_t hash = Hash(key);
    count_t index = hash % tableSize; 
    count_t increment = 0; // delay computation

    while (TRUE)
    {
        element_t& current = table[index];
            
        if (IsNull(current))
            RETURN NULL;

        if (!IsDeleted(current)
            && Equals(key, GetKey(current)))
        {
            RETURN &current;
        }

        if (increment == 0)
            increment = (hash % (tableSize-1)) + 1; 

        index += increment;
        if (index >= tableSize)
            index -= tableSize;
    }
}

template <typename TRAITS>
BOOL SHash<TRAITS>::Add(element_t *table, count_t tableSize, const element_t &element)
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(Equals(GetKey(element), GetKey(*Lookup(table, tableSize, GetKey(element)))));
    }
    CONTRACT_END;

    key_t key = GetKey(element);

    count_t hash = Hash(key);
    count_t index = hash % tableSize; 
    count_t increment = 0; // delay computation

    while (TRUE)
    {
        element_t& current = table[index];
            
        if (IsNull(current))
        {
            table[index] = element;
            RETURN TRUE;
        }

        if (IsDeleted(current))
        {
            table[index] = element;
            RETURN FALSE;
        }

        if (increment == 0)
            increment = (hash % (tableSize-1)) + 1; 

        index += increment;
        if (index >= tableSize)
            index -= tableSize;
    }
}

#ifdef _MSC_VER
#pragma warning (disable: 4702) // Workaround bogus unreachable code warning
#endif
template <typename TRAITS>
void SHash<TRAITS>::Remove(element_t *table, count_t tableSize, key_t key)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(s_supports_remove);
        PRECONDITION(Lookup(table, tableSize, key) != NULL);
    }
    CONTRACT_END;

    count_t hash = Hash(key);
    count_t index = hash % tableSize; 
    count_t increment = 0; // delay computation

    while (TRUE)
    {
        element_t& current = table[index];
            
        if (IsNull(current))
            RETURN;

        if (!IsDeleted(current)
            && Equals(key, GetKey(current)))
        {
            table[index] = Deleted();
      	    m_tableCount--;
            RETURN;
        }

        if (increment == 0)
            increment = (hash % (tableSize-1)) + 1; 

        index += increment;
        if (index >= tableSize)
            index -= tableSize;
    }
}
#ifdef _MSC_VER
#pragma warning (default: 4702)
#endif

template <typename TRAITS>
void SHash<TRAITS>::RemoveElement(element_t *table, count_t tableSize, element_t *element)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(s_supports_remove);
        PRECONDITION(table <= element && element < table + tableSize);
        PRECONDITION(!IsNull(*element) && !IsDeleted(*element));
    }
    CONTRACT_END;

    *element = Deleted();
    m_tableCount--;
    RETURN;
}

template <typename TRAITS>
BOOL SHash<TRAITS>::IsPrime(COUNT_T number)
{
    CONTRACT(BOOL)
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    // This is a very low-tech check for primality, which doesn't scale very well.  
    // There are more efficient tests if this proves to be burdensome for larger
    // tables.

    if ((number&1) == 0)
        RETURN FALSE;

    COUNT_T factor = 3;
    while (factor * factor <= number)
    {
        if ((number % factor) == 0)
            RETURN FALSE;
        factor += 2;
    }

    RETURN TRUE;
}

template <typename TRAITS>
COUNT_T SHash<TRAITS>::NextPrime(COUNT_T number)
{
    CONTRACT(COUNT_T)
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(IsPrime(RETVAL));
    }
    CONTRACT_END;

    static const COUNT_T primes[] = {
        11,17,23,29,37,47,59,71,89,107,131,163,197,239,293,353,431,521,631,761,919,
        1103,1327,1597,1931,2333,2801,3371,4049,4861,5839,7013,8419,10103,12143,14591,
        17519,21023,25229,30293,36353,43627,52361,62851,75431,90523, 108631, 130363, 
        156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403,
        968897, 1162687, 1395263, 1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 
        4999559, 5999471, 7199369 };

    for (int i = 0; i < (int) (sizeof(primes) / sizeof(primes[0])); i++) {
        if (primes[i] >= number)
            RETURN primes[i];
    }

    if ((number&1) == 0)
        number++;

    while (number != 1) {
        if (IsPrime(number))
            RETURN number;
        number +=2;
    }

    // overflow
    ThrowOutOfMemory();
}

#endif // _SHASH_INL_
