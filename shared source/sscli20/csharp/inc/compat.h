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
// File: compat.h
//
// ===========================================================================

#ifndef COMPATDEF
#error you must define COMPATDEF macro before including COMPAT.H!
#endif
#ifndef COMPATNAME
#error you must define COMPATNAME macro before including COMPAT.H!
#endif

////////////////////////////////////////////////////////////////////////////////
// Compatibility switches
//
// This adds values to CompatibilitMode enum ( = Compatibility + Name),
//   does the parsing in CCompilerConfig::CommitChanges function, and 
//   adds accessors to COptionData class ( Is + Name + Mode()).
// If you add any names or values, you need to edit the help-text
//   in msgs.rc (IDS_BadCompatMode and IDS_OD_COMPATIBILITY)
//

//		
//		Name			Value		Default Text (case insensitive)
COMPATDEF(	ECMA1,			0,		L"ISO-1")
COMPATDEF(	None,			1,		L"Default") // Keep this last
// CompatibilityNone is always the default 'latest and greatest'

// alternate names for parsing (NOTE: no value column)
//COMPATNAME(	None,					L"default")

#undef COMPATDEF
#undef COMPATNAME
