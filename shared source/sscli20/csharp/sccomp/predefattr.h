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
// File: predefattr.h
//
// Contains a list of the predefined attributes.
// ===========================================================================

#if !defined(PREDEFATTRDEF)
#error Must define PREDEFATTRDEF macro before including predefattr.h
#endif

//            id                            type

// special behaviour attributes
PREDEFATTRDEF(PA_ATTRIBUTEUSAGE,            PT_ATTRIBUTEUSAGE           )
PREDEFATTRDEF(PA_OBSOLETE,                  PT_OBSOLETE                 )
PREDEFATTRDEF(PA_CLSCOMPLIANT,              PT_CLSCOMPLIANT             )
PREDEFATTRDEF(PA_CONDITIONAL,               PT_CONDITIONAL              )
PREDEFATTRDEF(PA_REQUIRED,                  PT_REQUIRED                 )
PREDEFATTRDEF(PA_FIXED,                     PT_FIXEDBUFFER              )
PREDEFATTRDEF(PA_DEBUGGABLE,                PT_DEBUGGABLE               )

// magic attributes
PREDEFATTRDEF(PA_NAME,                      PT_INDEXERNAME              )

// extra checking for interop only
PREDEFATTRDEF(PA_DLLIMPORT,                 PT_DLLIMPORT                )
PREDEFATTRDEF(PA_COMIMPORT,                 PT_COMIMPORT                )
PREDEFATTRDEF(PA_GUID,                      PT_GUID                     )
PREDEFATTRDEF(PA_IN,                        PT_IN                       )
PREDEFATTRDEF(PA_OUT,                       PT_OUT                      )
PREDEFATTRDEF(PA_STRUCTOFFSET,              PT_FIELDOFFSET              )
PREDEFATTRDEF(PA_STRUCTLAYOUT,              PT_STRUCTLAYOUT             )
PREDEFATTRDEF(PA_PARAMARRAY,                PT_PARAMS                   )
PREDEFATTRDEF(PA_COCLASS,                   PT_COCLASS                  )
PREDEFATTRDEF(PA_DEFAULTCHARSET,            PT_DEFAULTCHARSET           )
PREDEFATTRDEF(PA_DEFAULTVALUE,              PT_DEFAULTVALUE             )
PREDEFATTRDEF(PA_UNMANAGEDFUNCTIONPOINTER,  PT_UNMANAGEDFUNCTIONPOINTER )

PREDEFATTRDEF(PA_COMPILATIONRELAXATIONS,    PT_COMPILATIONRELAXATIONS   )
PREDEFATTRDEF(PA_RUNTIMECOMPATIBILITY,      PT_RUNTIMECOMPATIBILITY     )
PREDEFATTRDEF(PA_FRIENDASSEMBLY,            PT_FRIENDASSEMBLY           )
PREDEFATTRDEF(PA_KEYFILE,                   PT_KEYFILE                  )
PREDEFATTRDEF(PA_KEYNAME,                   PT_KEYNAME                  )
PREDEFATTRDEF(PA_DELAYSIGN,                 PT_DELAYSIGN                )

PREDEFATTRDEF(PA_DEFAULTMEMBER,             PT_DEFAULTMEMBER            )

PREDEFATTRDEF(PA_TYPEFORWARDER,             PT_TYPEFORWARDER            )
