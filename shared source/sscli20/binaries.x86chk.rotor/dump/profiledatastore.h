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

#ifndef PROFILEDATASTORE_H_0347853098540397
#define PROFILEDATASTORE_H_0347853098540397

#include "holder.h"


struct ProfiledRange;


struct IProfileDataStoreSection
{
    virtual ~IProfileDataStoreSection() {}
};


struct IProfileDataStore
{
    enum ProfileDataType
    {
        CodeType,
        PreloadType
    };

    enum SectionType
    {
        SECTION_TYPE_INVALID                = 0,
        SECTION_PROFILED_HOT_READING        = 1,
        SECTION_PROFILED_HOT_WRITING        = SECTION_PROFILED_HOT_READING << 1,
        SECTION_POTENTIALLY_HOT_READING     = SECTION_PROFILED_HOT_WRITING << 1,
        SECTION_POTENTIALLY_HOT_WRITING     = SECTION_POTENTIALLY_HOT_READING << 1,
        SECTION_COLD                        = SECTION_POTENTIALLY_HOT_WRITING << 1,
        SECTION_UNKNOWN                     = 0xffff,
    };
    
    virtual NewHolder< IProfileDataStoreSection >
        GetProfileReferenceSection( ProfileDataType ) = 0;

    virtual void AddRange(
        IProfileDataStoreSection * section,
        SectionType type,
        SIZE_T start,
        SIZE_T length ) = 0;

    virtual void AddRange(
        IProfileDataStoreSection * section,
        ProfiledRange range ) = 0;

    virtual ~IProfileDataStore() {}
};


struct ProfiledRange
{
public:
    ProfiledRange()
        : start( 0 )
        , length( 0 )
        , type( IProfileDataStore::SECTION_TYPE_INVALID )
    {}

    ProfiledRange( SIZE_T s, SIZE_T l, IProfileDataStore::SectionType t )
        : start( s )
        , length( l )
        , type( t )
    {}

    ProfiledRange( ProfiledRange const & obj )
        : start( obj.start )
        , length( obj.length )
        , type( obj.type )
    {}

    ProfiledRange & operator=( ProfiledRange const & obj )
    {
        start = obj.start;
        length = obj.length;
        type = obj.type;
        return *this;
    }

public:
    SIZE_T start;
    SIZE_T length;
    IProfileDataStore::SectionType type;
};


#endif // PROFILEDATASTORE_H_0347853098540397

