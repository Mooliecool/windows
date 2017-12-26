//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Template Utilities for the std::map class
//
//-------------------------------------------------------------------------------------------------

#pragma once

class MapUtil
{
public:

    template <typename TKey,typename TValue>
    static 
    bool ContainsKey( const std::map<TKey,TValue>& map, TKey& key )
    {
        return map.find(key) != map.end();
    }

    //-------------------------------------------------------------------------------------------------
    //
    // Try and find the value for the specified key
    //
    //-------------------------------------------------------------------------------------------------
    template <typename TKey,typename TValue>
    static 
    bool TryGetValue( 
        _In_ const std::map<TKey,TValue>& map, 
        _In_ const TKey& key,
        _Out_ TValue& value)
    {
        auto it = map.find(key);
        if ( it == map.end() )
        {
            return false;
        }

        value = it->second;
        return true;
    }

};
