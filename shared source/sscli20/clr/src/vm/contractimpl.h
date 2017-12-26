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
// File: CONTRACTIMPL.H
//
// CONTRACTIMPL.H keeps track of contract implementations
// ===========================================================================

#ifndef CONTRACTIMPL_H_ 
#define CONTRACTIMPL_H_

#include "hash.h"
#include "decodemd.h"

class Module;
class MethodDesc;
class StackingAllocator;

// ===========================================================================
struct DispatchSlot
{
protected:
    PTR_TADDR m_slot;

public:
    //------------------------------------------------------------------------
    inline DispatchSlot(PTR_TADDR slot) : m_slot(slot)
    { LEAF_CONTRACT; }

    //------------------------------------------------------------------------
    inline DispatchSlot(const DispatchSlot &slot) : m_slot(slot.m_slot)
    { LEAF_CONTRACT; }

    //------------------------------------------------------------------------
    inline DispatchSlot& operator=(const PTR_TADDR ppb)
    { LEAF_CONTRACT; SetTargetSlot(ppb); return *this; }

    //------------------------------------------------------------------------
    inline DispatchSlot& operator=(const DispatchSlot &slot)
    { LEAF_CONTRACT; m_slot = slot.m_slot; return *this; }

    //------------------------------------------------------------------------
    inline BOOL IsNull()
    { LEAF_CONTRACT; return (m_slot == NULL); }

    //------------------------------------------------------------------------
    inline void SetNull()
    { LEAF_CONTRACT; m_slot = NULL; }

    //------------------------------------------------------------------------
    inline TADDR GetTarget()
    { LEAF_CONTRACT; return *m_slot; }

    //------------------------------------------------------------------------
    inline PTR_TADDR GetTargetSlot()
    { LEAF_CONTRACT; return m_slot; }

    //------------------------------------------------------------------------
    inline void SetTarget(TADDR target)
    { LEAF_CONTRACT; *m_slot = target; }

    //------------------------------------------------------------------------
    inline void SetTargetSlot(PTR_TADDR slot)
    { LEAF_CONTRACT; m_slot = slot; }

    //------------------------------------------------------------------------
    inline void Set(DispatchSlot slot)
    { LEAF_CONTRACT; SetTargetSlot(slot.GetTargetSlot()); }

    //------------------------------------------------------------------------
    MethodDesc *GetMethodDesc();
};



// ===========================================================================
// This value indicates that a slot number is in reference to the
// current class. Thus, no TypeID can have a value of 0. This is stored
// inside a DispatchToken as the TypeID for such cases.
static const UINT32 TYPE_ID_THIS_CLASS = 0;


// ===========================================================================
// The type IDs used in the dispatch map are relative to the implementing
// type, and are a discriminated union between:
//   - a special value to indicate "this" class
//   - a special value to indicate that an interface is not implemented by the type
//   - an index into the InterfaceMap
class DispatchMapTypeID
{
private:
    UINT32 m_typeIDVal;
    DispatchMapTypeID(UINT32 id) { LEAF_CONTRACT; m_typeIDVal = id; }
public:
    // Constructors
    static DispatchMapTypeID ThisClassID() { LEAF_CONTRACT; return DispatchMapTypeID(0); }
    static DispatchMapTypeID InterfaceClassID(UINT32 inum)
    {
        LEAF_CONTRACT;
        _ASSERTE(inum + 2 > inum);
        return DispatchMapTypeID(inum + 2);
    }
    static DispatchMapTypeID InterfaceNotImplementedID() { LEAF_CONTRACT; return DispatchMapTypeID(1); }
    DispatchMapTypeID() { LEAF_CONTRACT; m_typeIDVal = 0; }

    // Accessors
    BOOL IsThisClass() { LEAF_CONTRACT; return (m_typeIDVal == 0); }
    BOOL IsImplementedInterface() { LEAF_CONTRACT; return (m_typeIDVal > 1); }
    UINT32 GetInterfaceNum()
    {
        LEAF_CONTRACT;
        _ASSERTE(IsImplementedInterface());
        return (m_typeIDVal - 2);
    }

    // Ordering/equality
    BOOL operator ==(const DispatchMapTypeID &that) { LEAF_CONTRACT; return m_typeIDVal == that.m_typeIDVal; }
    BOOL operator !=(const DispatchMapTypeID &that) { LEAF_CONTRACT; return m_typeIDVal != that.m_typeIDVal; }
    BOOL operator <(const DispatchMapTypeID &that) { LEAF_CONTRACT; return m_typeIDVal < that.m_typeIDVal; }

    // To/from UINT32, for encoding/decoding etc.
    UINT32 ToUINT32() const { return m_typeIDVal; }
    static DispatchMapTypeID FromUINT32(UINT32 x) { return DispatchMapTypeID(x); }

};


// ===========================================================================
// This represents the contract used for code lookups throughout the
// virtual stub dispatch mechanism. It is important to know that
// sizeof(DispatchToken) is UINT_PTR, which means it can be thrown around
// by value without a problem.
struct DispatchToken
{
private:
    static const UINT_PTR INVALID_TOKEN = 0xFFFFFFFF;
    // IMPORTANT: This is the ONLY member of this class.
    UINT_PTR     m_token;

    static const UINT_PTR MASK_TYPE_ID       = 0x0000FFFF;
    static const UINT_PTR MASK_SLOT_NUMBER   = 0x0000FFFF;

    static const UINT_PTR SHIFT_TYPE_ID      = 0x10;
    static const UINT_PTR SHIFT_SLOT_NUMBER  = 0x0;

    //------------------------------------------------------------------------
    // Combines the two values into a single 32-bit number.
    static UINT_PTR CreateToken(UINT32 typeID, UINT32 slotNumber)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(((UINT_PTR)typeID & MASK_TYPE_ID) == (UINT_PTR)typeID);
        CONSISTENCY_CHECK(((UINT_PTR)slotNumber & MASK_SLOT_NUMBER) == (UINT_PTR)slotNumber);
        return ((((UINT_PTR)typeID & MASK_TYPE_ID) << SHIFT_TYPE_ID) |
                (((UINT_PTR)slotNumber & MASK_SLOT_NUMBER) << SHIFT_SLOT_NUMBER));
    }

    //------------------------------------------------------------------------
    // Extracts the type ID from a token created by CreateToken
    static UINT32 DecodeTypeID(UINT_PTR token)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(token != INVALID_TOKEN);
        return ((token >> SHIFT_TYPE_ID) & MASK_TYPE_ID);
    }

    //------------------------------------------------------------------------
    // Extracts the slot number from a token created by CreateToken
    static UINT32 DecodeSlotNumber(UINT_PTR token)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(token != INVALID_TOKEN);
        return ((token >> SHIFT_SLOT_NUMBER) & MASK_SLOT_NUMBER);
    }

    //------------------------------------------------------------------------
    DispatchToken(UINT_PTR token)
    {
        CONSISTENCY_CHECK(token != INVALID_TOKEN);
        m_token = token;
    }

public:
    //------------------------------------------------------------------------
    DispatchToken()
    {
        LEAF_CONTRACT;
        m_token = INVALID_TOKEN;
    }

    //------------------------------------------------------------------------
    inline BOOL operator==(const DispatchToken &tok) const
    { LEAF_CONTRACT; return m_token == tok.m_token; }

    //------------------------------------------------------------------------
    // Creates a "this" type dispatch token. This means that the type for the
    // token is implied by the type on which one wishes to invoke. In other
    // words, the value returned by GetTypeID is TYPE_ID_THIS_CLASS.
    static DispatchToken CreateDispatchToken(UINT32 slotNumber)
    {
        WRAPPER_CONTRACT;
        return DispatchToken(CreateToken(TYPE_ID_THIS_CLASS, slotNumber));
    }

    //------------------------------------------------------------------------
    // Creates a fully qualified type dispatch token. This means that the ID
    // for the type is encoded directly in the token.
    static DispatchToken CreateDispatchToken(UINT32 typeID, UINT32 slotNumber)
    {
        WRAPPER_CONTRACT;
        return DispatchToken(CreateToken(typeID, slotNumber));
    }


    //------------------------------------------------------------------------
    // Returns the type ID for this dispatch contract
    inline UINT32 GetTypeID()
    {
        WRAPPER_CONTRACT;
        return DecodeTypeID(m_token);
    }

    //------------------------------------------------------------------------
    // Returns the slot number for this dispatch contract
    inline UINT32 GetSlotNumber()
    {
        WRAPPER_CONTRACT;
        return DecodeSlotNumber(m_token);
    }

    //------------------------------------------------------------------------
    inline BOOL IsThisToken()
    {
        WRAPPER_CONTRACT;
        return (GetTypeID() == TYPE_ID_THIS_CLASS);
    }

    //------------------------------------------------------------------------
    inline BOOL IsTypedToken()
    {
        WRAPPER_CONTRACT;
        return (!IsThisToken());
    }

    //------------------------------------------------------------------------
    static DispatchToken From_SIZE_T(SIZE_T token)
    {
        WRAPPER_CONTRACT;
        return DispatchToken((UINT_PTR)token);
    }

    //------------------------------------------------------------------------
    SIZE_T To_SIZE_T()
    {
        WRAPPER_CONTRACT;
        // This is just sanity checking to make sure that DispatchToken::GetToken
        // will fit in a size_t
        C_ASSERT(sizeof(SIZE_T) == sizeof(UINT_PTR));
        return (SIZE_T) m_token;
    }

    //------------------------------------------------------------------------
    inline BOOL IsValid()
    {
        LEAF_CONTRACT;
        return !(m_token == INVALID_TOKEN);
    }
};


// ===========================================================================
class TypeIDProvider
{
protected:
    UINT32      m_nextID;
    UINT32      m_incSize;

public:
    //------------------------------------------------------------------------
    // Ctor
    TypeIDProvider()
        : m_nextID(0), m_incSize(0)
    { LEAF_CONTRACT; }


    //------------------------------------------------------------------------
    void Init(UINT32 idStartValue, UINT32 idIncrementValue)
{
        LEAF_CONTRACT;
        m_nextID = idStartValue;
        m_incSize = idIncrementValue;
    }

    //------------------------------------------------------------------------
    // Returns the next available ID
    inline UINT32 GetNextID()
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(m_nextID != 0);
        CONSISTENCY_CHECK(m_incSize != 0);
        UINT32 id = m_nextID;
        m_nextID += m_incSize;
        return id;
    }

    //------------------------------------------------------------------------
    inline BOOL OwnsID(UINT32 id)
    {
        LEAF_CONTRACT;
        return ((id % m_incSize) == (m_nextID % m_incSize));
    }
};

// ===========================================================================
class TypeIDMap
{
protected:
    HashMap             m_hashMap;
    Crst                m_lock;
    TypeIDProvider      m_idProvider;
    UINT32              m_entryCount;

    //------------------------------------------------------------------------
    // Returns the next available ID
    inline UINT32 GetNextID()
    {
        WRAPPER_CONTRACT;
        CONSISTENCY_CHECK(m_lock.OwnedByCurrentThread());
        UINT32 id = m_idProvider.GetNextID();
        CONSISTENCY_CHECK(id != TYPE_ID_THIS_CLASS);
        return id;
    }

public:
    // This is used for an invalid type ID.
    static const UINT32 INVALID_TYPE_ID = (UINT32) INVALIDENTRY;

    // If we can have more than 65k types, we'll need to revisit this.
    static const UINT32 MAX_TYPE_ID = (UINT32) 0xFFFF;

    // Starting values for shared and unshared domains
    static const UINT32 STARTING_SHARED_DOMAIN_ID = (UINT32) 0x2;
    static const UINT32 STARTING_UNSHARED_DOMAIN_ID = (UINT32) 0x3;
    static const UINT32 STARTING_ZAP_MODULE_ID = (UINT32) 0x2;

    //------------------------------------------------------------------------
    void Init(UINT32 idStartValue, UINT32 idIncrementValue);

    //------------------------------------------------------------------------
    // Ctor
    TypeIDMap()
        : m_lock("TypeIDMap", CrstSyncHashLock, CrstFlags(CRST_REENTRANCY))
    { WRAPPER_CONTRACT; }

    //------------------------------------------------------------------------
    // Dtor
    ~TypeIDMap()
    { LEAF_CONTRACT; }

    //------------------------------------------------------------------------
    // Returns the ID of the type if found. If not found, returns INVALID_TYPE_ID
    UINT32 LookupTypeID(PTR_MethodTable pMT);

    //------------------------------------------------------------------------
    // Returns the ID of the type if found. If not found, returns NULL.
    PTR_MethodTable LookupType(UINT32 id);

    //------------------------------------------------------------------------
    // Returns the ID of the type if found. If not found, assigns the ID and
    // returns the new ID.
    UINT32 GetTypeID(PTR_MethodTable pMT);

    //------------------------------------------------------------------------
    inline UINT32 GetCount()
        { LEAF_CONTRACT; return m_entryCount; }

    //------------------------------------------------------------------------
    void Clear()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
        } CONTRACTL_END;
        m_hashMap.Clear();
        m_idProvider.Init(0, 0);
    }

    //------------------------------------------------------------------------
    class Iterator
    {
        HashMap::Iterator m_it;

    public:
        //--------------------------------------------------------------------
        inline Iterator(TypeIDMap *map)
            : m_it(map->m_hashMap.begin())
        {
            WRAPPER_CONTRACT;
            while (IsValid() && ((UINT32)m_it.GetKey() < MAX_TYPE_ID)) {
                ++m_it;
            }
        }

        //--------------------------------------------------------------------
        inline BOOL IsValid()
        {
            WRAPPER_CONTRACT;
            return !m_it.end();
        }

        //--------------------------------------------------------------------
        inline BOOL Next()
        {
            // We want to skip the entries that are ID->Type, and just
            // enumerate the Type->ID entries to avoid duplicates.
            do {
                ++m_it;
            } while (IsValid() && ((UINT32)m_it.GetKey() < MAX_TYPE_ID));
            return IsValid();
        }

        //--------------------------------------------------------------------
        inline MethodTable *GetType()
        {
            WRAPPER_CONTRACT;
            return (MethodTable *) m_it.GetKey();
        }

        //--------------------------------------------------------------------
        inline UINT32 GetID()
        {
            WRAPPER_CONTRACT;
            return (UINT32) m_it.GetValue();
        }
};
};


// ===========================================================================
struct DispatchMapEntry
{
private:
    static const UINT16 INVALID_CHAIN_DELTA = (UINT16)(-1);

    DispatchMapTypeID m_typeID;
    UINT16           m_slotNumber;

    UINT16           m_targetChainDelta;
    UINT16           m_targetSlotOrIndex;

    enum {
        e_IS_VALID      = 0x1,
        e_IS_VIRTUAL    = 0x2
    };
    UINT16           m_flags;

    //------------------------------------------------------------------------
    void Init(DispatchMapTypeID typeID,
              UINT32        slotNumber,
              UINT32        targetChainDelta,
              UINT32        targetSlotOrIndex,
              BOOL          isVirtual)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(isVirtual);

        m_typeID = typeID;
        m_slotNumber = (UINT16) slotNumber;
        m_targetChainDelta = (UINT16) targetChainDelta;
        m_targetSlotOrIndex = (UINT16) targetSlotOrIndex;

        // Set the flags
        m_flags = e_IS_VALID;
        if (isVirtual) m_flags |= e_IS_VIRTUAL;
    }

public:
    //------------------------------------------------------------------------
    // Initializes this structure.
    inline void InitVirtualMapping(DispatchMapTypeID typeID,
                            UINT32 slotNumber,
                            UINT32 targetSlotNumber)
    {
        WRAPPER_CONTRACT;
        Init(typeID, slotNumber, INVALID_CHAIN_DELTA, targetSlotNumber, TRUE);
    }

    //------------------------------------------------------------------------
    // Initializes this structure.
    inline void InitNonVirtualMapping(DispatchMapTypeID typeID,
                               UINT32 slotNumber,
                               UINT32 targetChainDelta,
                               UINT32 targetImplIndex)
    {
        WRAPPER_CONTRACT;
        Init(typeID, slotNumber, targetChainDelta, targetImplIndex, FALSE);
    }

    //------------------------------------------------------------------------
    inline DispatchMapTypeID GetTypeID()
        { LEAF_CONTRACT; return m_typeID; }

    //------------------------------------------------------------------------
    inline UINT32 GetSlotNumber()
        { LEAF_CONTRACT; return (UINT32) m_slotNumber; }

    //------------------------------------------------------------------------
    // Returns TRUE if the source is mapped to the target slot number
    // virtually (i.e., get the most overridden value for the slot).
    inline BOOL  IsVirtuallyMapped()
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(IsValid());
        return (m_flags & e_IS_VIRTUAL) ? TRUE : FALSE;
    }

    //------------------------------------------------------------------------
    // This is only valid if the mapping is virtual (i.e.,
    // IsVirtuallyMapped() returns TRUE.
    inline UINT32 GetTargetSlotNumber()
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(IsValid());
        CONSISTENCY_CHECK(IsVirtuallyMapped());
        return (UINT32) m_targetSlotOrIndex;
    }
    inline void SetTargetSlotNumber(UINT32 targetSlotNumber)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(IsValid());
        CONSISTENCY_CHECK(IsVirtuallyMapped());
        m_targetSlotOrIndex = (UINT16) targetSlotNumber;
    }


    //------------------------------------------------------------------------
    // Ctor - just blanks everything out - need to call Init*Mapping function.
    inline DispatchMapEntry() : m_flags(0)
        { LEAF_CONTRACT; }

    inline BOOL IsValid()
        { LEAF_CONTRACT; return (m_flags & e_IS_VALID); }
};

// ===========================================================================
// This represents an entry in the dispatch mapping. Conceptually, there is a
// source to target mapping, and a flag to state whether or not it is to be
// mapped virtually or statically. There are additional housekeeping flags.
struct DispatchMapBuilderNode
{
    // This represents the type and slot for this mapping
    DispatchMapTypeID      m_typeID;
    UINT32                 m_slotNumber;

    // These represent the target, and type of mapping
    MethodDesc            *m_pMDTarget;
    UINT32                 m_chainDelta;
    UINT32                 m_targetSlotOrIndex;
    BOOL                   m_fIsVirtualMapping;

    // Flags
    UINT32                 m_flags;

    enum {
        e_ENTRY_IS_METHODIMPL = 1
    };

    // Next entry in the list
    DispatchMapBuilderNode *m_next;

    //------------------------------------------------------------------------
    void Init(DispatchMapTypeID typeID,
              UINT32 slotNumber,
              UINT32 chainDelta,
              UINT32 targetSlotOrIndex,
              MethodDesc *pMDTarget,
              BOOL fIsVirtual)
{
        WRAPPER_CONTRACT;
        CONSISTENCY_CHECK(CheckPointer(pMDTarget, NULL_OK));
        // Remember type and slot
        m_typeID = typeID;
        m_slotNumber = slotNumber;
        // Set the target MD
        m_pMDTarget = pMDTarget;
        // Save the target info.
        m_chainDelta = chainDelta;
        m_targetSlotOrIndex = targetSlotOrIndex;
        // Remember virtualness
        m_fIsVirtualMapping = fIsVirtual;
        // Initialize the flags
        m_flags = 0;
        // Default to null link
        m_next = NULL;
    }

    //------------------------------------------------------------------------
    inline BOOL IsMethodImpl()
    {
        WRAPPER_CONTRACT;
        return (m_flags & e_ENTRY_IS_METHODIMPL);
    }

    //------------------------------------------------------------------------
    inline void SetIsMethodImpl(BOOL fIsMethodImpl)
    {
        WRAPPER_CONTRACT;
        m_flags = fIsMethodImpl ? m_flags | e_ENTRY_IS_METHODIMPL :
                                  m_flags & ~(e_ENTRY_IS_METHODIMPL);
    }
};

// ===========================================================================
class DispatchMapBuilder
{
public:
    class Iterator;

    //------------------------------------------------------------------------
    DispatchMapBuilder(StackingAllocator *allocator)
        : m_pHead(NULL), m_cEntries(0), m_pAllocator(allocator)
    { LEAF_CONTRACT; CONSISTENCY_CHECK(CheckPointer(m_pAllocator)); }

    //------------------------------------------------------------------------
    inline StackingAllocator *GetAllocator()
    { LEAF_CONTRACT; return m_pAllocator; }

    //------------------------------------------------------------------------
    // If TRUE, it points to a matching entry.
    // If FALSE, it is at the insertion point.
    BOOL Find(DispatchMapTypeID typeID, UINT32 slotNumber, Iterator &it);

    //------------------------------------------------------------------------
    // If TRUE, contains such an entry.
    // If FALSE, no such entry exists.
    BOOL Contains(DispatchMapTypeID typeID, UINT32 slotNumber);

    //------------------------------------------------------------------------
    // This is used when building a MT, and things such as implementation
    // table index and chain delta can't be calculated until later on. That's
    // why we use an MD to get the information later.
    BOOL InsertMDMapping(DispatchMapTypeID typeID,
                         UINT32 slotNumber,
                         MethodDesc *pMDTarget,
                         BOOL fVirtualMapping,
                         BOOL fIsMethodImpl = FALSE);

    //------------------------------------------------------------------------
    inline UINT32 Count()
    { LEAF_CONTRACT; return m_cEntries; }

    //------------------------------------------------------------------------
    class Iterator
    {
        friend class DispatchMapBuilder;

    protected:
        DispatchMapBuilderNode **m_cur;

        //--------------------------------------------------------------------
        inline DispatchMapBuilderNode **EntryNodePtr()
        { LEAF_CONTRACT; return m_cur; }

        //--------------------------------------------------------------------
        inline DispatchMapBuilderNode *EntryNode()
        { LEAF_CONTRACT; CONSISTENCY_CHECK(IsValid()); return *m_cur; }

public:
        //--------------------------------------------------------------------
        // Creates an iterator that is pointing to the first entry of the map.
        inline Iterator(DispatchMapBuilder *pMap)
            : m_cur(&pMap->m_pHead)
        { LEAF_CONTRACT; }

        //--------------------------------------------------------------------
        // Creates an iterator this is pointing to the same location as 'it'.
        inline Iterator(Iterator &it)
            : m_cur(it.m_cur)
        { LEAF_CONTRACT; }

        //--------------------------------------------------------------------
        inline BOOL IsValid()
        { LEAF_CONTRACT; return (*m_cur != NULL); }

        //--------------------------------------------------------------------
        inline BOOL Next()
        {
            WRAPPER_CONTRACT;
            if (!IsValid()) {
                return FALSE;
            }
            m_cur = &((*m_cur)->m_next);
            return (IsValid());
        }

        //--------------------------------------------------------------------
        inline DispatchMapTypeID GetTypeID()
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            return EntryNode()->m_typeID;
        }

        //--------------------------------------------------------------------
        inline UINT32 GetSlotNumber()
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            return EntryNode()->m_slotNumber;
        }

        //--------------------------------------------------------------------
        inline MethodDesc *GetTargetMD()
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            return EntryNode()->m_pMDTarget;
        }

        //--------------------------------------------------------------------
        UINT32 GetTargetSlot();


        //--------------------------------------------------------------------
        inline void SetTarget(MethodDesc *pMDTarget)
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            CONSISTENCY_CHECK(CheckPointer(pMDTarget));
            EntryNode()->m_pMDTarget = pMDTarget;
        }

        //--------------------------------------------------------------------
        inline BOOL IsVirtualMapping()
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            return EntryNode()->m_fIsVirtualMapping;
        }

        //--------------------------------------------------------------------
        inline BOOL IsMethodImpl()
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            return EntryNode()->IsMethodImpl();
        }

        //--------------------------------------------------------------------
        inline void SetIsMethodImpl(BOOL fIsMethodImpl)
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(IsValid());
            EntryNode()->SetIsMethodImpl(fIsMethodImpl);
        }

        inline void SkipThisTypeEntries()
        {
            while (IsValid() && GetTypeID() == DispatchMapTypeID::ThisClassID()) {
                Next();
            }
        }
};

protected:
    DispatchMapBuilderNode *m_pHead;
    UINT32                  m_cEntries;
    StackingAllocator      *m_pAllocator;

    //------------------------------------------------------------------------
    DispatchMapBuilderNode *NewEntry();

    //------------------------------------------------------------------------
    BOOL Insert(DispatchMapTypeID typeID,
                UINT32 slotNumber,
                UINT32 chainDelta,
                UINT32 targetSlotOrIndex,
                MethodDesc *pMDTarget,
                BOOL fVirtualMapping,
                BOOL fIsMethodImpl);


};

typedef DPTR(class DispatchMap)           PTR_DispatchMap;
// ===========================================================================
class DispatchMap
{
protected:
    BYTE                            m_rgMap[0];

    static const UINT32 SCOPE_VIRTUAL = 0;
    static const UINT32 SCOPE_NON_VIRTUAL_BASE = 1;
    static const INT32  ENCODING_TYPE_DELTA = 1;
    static const INT32  ENCODING_SLOT_DELTA = 1;
    static const INT32  ENCODING_TARGET_SLOT_DELTA = 1;
    static const INT32  ENCODING_TARGET_INDEX_DELTA = 1;

public:
    //------------------------------------------------------------------------
    // Need to make sure that you allocate GetObjectSize(pMap) bytes for any
    // instance of DispatchMap, as this constructor assumes that m_rgMap is
    // large enough to store cbMap bytes, which GetObjectSize ensures.
    DispatchMap(
                BYTE *pMap,
                UINT32 cbMap);


    //------------------------------------------------------------------------
    static void CreateEncodedMapping(MethodTable                 *pMT,
                                     DispatchMapBuilder          *pMapBuilder,
                                     StackingAllocator           *pAllocator,
                                     BYTE                       **ppbMap,
                                     UINT32                      *pcbMap);

    //------------------------------------------------------------------------
    static UINT32 GetObjectSize(UINT32 cbMap)
    {
        LEAF_CONTRACT;
        return (UINT32)(sizeof(DispatchMap) + cbMap);
    }

    //------------------------------------------------------------------------
    UINT32 GetMapSize();

#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif


    //------------------------------------------------------------------------
    enum {
        e_IT_FLAG_THIS_CLASS_ONLY = 0x1,
        e_IT_FLAG_DISABLE_ZAP_ID_MAP = 0x2,
    };

    //------------------------------------------------------------------------
    class EncodedMapIterator
    {
        friend class DispatchMap;
    protected:
        DispatchMapEntry m_e;

        // These fields are for decoding the implementation map
        Decoder          m_d;
        // Keep count of the number of types in the list
        INT32            m_numTypes;
        INT32            m_curType;
        DispatchMapTypeID           m_curTypeId;
        BOOL             m_fCurTypeHasNegativeEntries;

        // Keep count of the number of entries for the current type
        INT32            m_numEntries;
        INT32            m_curEntry;
        UINT32           m_curSlot;

        UINT32           m_curTargetSlot;
        UINT32           m_curTargetIndex;

        // Private flag, continuing from the last public flag
        UINT32           m_flags;

        //--------------------------------------------------------------------
        inline BOOL IsZapMapDisabled()
            { LEAF_CONTRACT; return (m_flags & e_IT_FLAG_DISABLE_ZAP_ID_MAP); }

        //--------------------------------------------------------------------
        inline BOOL IsThisClassOnly()
            { LEAF_CONTRACT; return (m_flags & e_IT_FLAG_THIS_CLASS_ONLY); }

        //--------------------------------------------------------------------
        void Invalidate();

        //--------------------------------------------------------------------
        void Init(BYTE *pbMap);

public:
        //--------------------------------------------------------------------
        EncodedMapIterator(MethodTable *pMT, UINT32 flags = 0);

        //--------------------------------------------------------------------
        // This should be used only when a dispatch map needs to be used
        // separately from its MethodTable.
        EncodedMapIterator(DispatchMap *pMap, Module *pModule, UINT32 flags = 0);

        //--------------------------------------------------------------------
        EncodedMapIterator(BYTE *pbMap, UINT32 flags = e_IT_FLAG_DISABLE_ZAP_ID_MAP);

        //--------------------------------------------------------------------
        inline BOOL IsValid()
        { LEAF_CONTRACT; return (m_curType < m_numTypes); }

        //--------------------------------------------------------------------
        BOOL Next();

        //--------------------------------------------------------------------
        inline DispatchMapEntry *Entry()
        { LEAF_CONTRACT; CONSISTENCY_CHECK(IsValid()); return &m_e; }
    };


public:
    //------------------------------------------------------------------------
    class Iterator
    {
    protected:

        // This is for generating entries from the encoded map
        EncodedMapIterator m_mapIt;

    public:
        //--------------------------------------------------------------------
        Iterator(MethodTable *pMT, UINT32 flags = 0);

        //--------------------------------------------------------------------
        BOOL IsValid();

        //--------------------------------------------------------------------
        BOOL Next();

        //--------------------------------------------------------------------
        DispatchMapEntry *Entry();
    };
};


#ifdef LOGGING 
struct StubDispatchStats {
    // DispatchMap stats
    UINT32 m_cDispatchMap;              // Number of DispatchMaps created
    UINT32 m_cbDispatchMap;             // Total size of created maps
    UINT32 m_cNGENDispatchMap;
    UINT32 m_cbNGENDispatchMap;

    // DispatchImplementationTable stats
    UINT32 m_cDispatchTable;            // Number of DispatchImplementationTables created
    UINT32 m_cDispatchTableSlots;       // Total number of slots in the tables
    UINT32 m_cDispatchTableVirt;        // Number of virtuals
    UINT32 m_cDispatchTableNonVirt;     // Number of non-virtuals
    UINT32 m_cNGENDispatchTable;        // Number of DispatchImplementationTables created
    UINT32 m_cNGENDispatchTableSlots;   // Total number of slots in the tables
    UINT32 m_cNGENDispatchTableVirt;    // Number of virtuals
    UINT32 m_cNGENDispatchTableNonVirt; // Number of non-virtuals
    UINT32 m_cNGENWriteableSlots;       // Number of slots that can be backpatched.

    // Type table stats
    UINT32 m_cNGENTypeTableEntries;     // Number of entries

    // Some comparative stats with the old world (simulated)
    UINT32 m_cVTables;                  // Number of vtables out there
    UINT32 m_cVTableSlots;              // Total number of slots.
    UINT32 m_cVTableDuplicateSlots;     // Total number of duplicated slots
    UINT32 m_cNGENVTables;              // Number of vtables out there
    UINT32 m_cNGENVTableSlots;          // Total number of slots.
    UINT32 m_cNGENVTableDuplicateSlots; // Total number of duplicated slots

    UINT32 m_cCacheLookups;
    UINT32 m_cCacheMisses;

    UINT32 m_cbComInteropData;
};

extern StubDispatchStats g_sdStats;
#endif // LOGGING


#endif // !CONTRACTIMPL_H_

