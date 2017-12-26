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

#ifndef __prefix_assert_h__
#define __prefix_assert_h__

#if _DEBUG

	#if defined(VSASSERT)

		#define PREFAST_ASSUME(cond, text) VSASSERT(cond, text)

	#elif defined(ASSERT)

		#define PREFAST_ASSUME(cond, text) { if (!(cond)) { ASSERT(!text) } }

	#elif defined(NO_ASSERT_ALLOWED)

		#define PREFAST_ASSUME(cond, text)

	#else

		#error _PREFAST_ or VSASSERT or ASSERT must be defined.

	#endif

#else
	
		#define PREFAST_ASSUME(cond, text)
	
#endif

#endif // __prefix_assert_h__
