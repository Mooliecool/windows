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
namespace System.Diagnostics {
    
	using System;
   // Constants representing the importance level of messages to be logged.
   // This level can be used to organize messages, and also to filter which
   // messages are displayed.
   //
   // An attached debugger can enable or disable which messages will
   // actually be reported to the user through the COM+ debugger
   // services API.  This info is communicated to the runtime so only
   // desired events are actually reported to the debugger.  
    //                       NOTE: The following constants mirror the constants 
    // declared in the EE code (DebugDebugger.h). Any changes here will also
    // need to be made there.
    // Constants representing the importance level of messages to be logged.
    // This level can be used to organize messages, and also to filter which
    // messages are displayed.
	[Serializable()]
    internal enum LoggingLevels
    {
    	TraceLevel0         = 0,
    	TraceLevel1         = 1,
    	TraceLevel2         = 2,
    	TraceLevel3         = 3,
    	TraceLevel4         = 4,
    
    	StatusLevel0        = 20,
    	StatusLevel1        = 21,
    	StatusLevel2        = 22,
    	StatusLevel3        = 23,
    	StatusLevel4        = 24,
    
    	
    	WarningLevel        = 40,
    
    	ErrorLevel          = 50,
    
    	PanicLevel          = 100,
    }

}
