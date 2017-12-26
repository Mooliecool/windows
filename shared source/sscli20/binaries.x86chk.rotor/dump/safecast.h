/*
 *   
 *    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
 *   
 *    The use and distribution terms for this software are contained in the file
 *    named license.txt, which can be found in the root of this distribution.
 *    By using this software in any fashion, you are agreeing to be bound by the
 *    terms of this license.
 *   
 *    You must not remove this notice, or any other, from this software.
 *   
 */

#define SAFE_CAST_H_034712017023740128734

#include <limits.h>

#include "check.h"


#pragma push_macro( "max" )
#pragma push_macro( "min" )

#undef max
#undef min

#pragma warning( push )
#pragma warning( disable: 4267 )  // truncation


namespace utilcode
{

//  This template is for casting with truncation.
//  It's safe because the limits are checked (asserted at runtime).
//
template < typename Target, typename Source >
Target safe_cast( Source value );


//  different specializations, add your own as needed
//

template <>
inline
COUNT_T safe_cast( size_t value )
{
    _ASSERTE( value <= COUNT_T_MAX
        && value >= COUNT_T_MIN );

    return static_cast< COUNT_T >( value );
}

template <>
inline
ULONG safe_cast( SIZE_T value )
{
    _ASSERTE( value <= ULONG_MAX );

    return static_cast< ULONG >( value );
}

}


#pragma warning( pop )

#pragma pop_macro( "max" )
#pragma pop_macro( "min" )


