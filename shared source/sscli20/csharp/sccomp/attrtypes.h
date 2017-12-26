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

#if !defined(ATTRDEF)
#error Must define ATTRDEF macro before including attrtypes.h
#endif

ATTRDEF(ANT_DebugViewAttribute, "System.Diagnostics.DebuggerDisplayAttribute", 0, false)
ATTRDEF(ANT_DebugProxyAttribute, "System.Diagnostics.DebuggerTypeProxyAttribute", 0, false)
ATTRDEF(ANT_DebugVisualizerAttribute, "System.Diagnostics.DebuggerVisualizerAttribute", 0, false)
ATTRDEF(ANT_UIKindEnum, "System.Diagnostics.VisualizerUIType", SERIALIZATION_TYPE_I4, true)
ATTRDEF(ANT_Type, "System.Type", SERIALIZATION_TYPE_TYPE, true)
#ifdef LOCALONLY
ATTRDEF(ANT_DebugBrowsableAttribute, "System.Diagnostics.DebuggerBrowsableAttribute", 0, false)
ATTRDEF(ANT_DebugBrowsableEnum, "System.Diagnostics.DebuggerBrowsableState", SERIALIZATION_TYPE_I4, true)
ATTRDEF(ANT_InteropGuidAttribute, "System.Runtime.InteropServices.GuidAttribute", 0, false)
#endif // LOCALONLY
