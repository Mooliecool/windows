// ==++==
//
//   Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// ===========================================================================
// File: timerids.h
//
// Defined the timerids. Each timer id specifying a section of code
// that can be timed.
//
// The columns represent:
//   ID used to represent the code section programmatically
//   name used output the code section
//   subtotal id: all code section with the same subtotal id get a subtotal.
//                they must be adjacent to each-other.
// ===========================================================================

#define LAST_TIMER_GROUP_ID -1

TIMER_GROUP (0, "Parser")
TIMER_GROUP (LAST_TIMER_GROUP_ID, "")    //end marker

TIMERID(TIME_ParserMethodBody,	    		"ParserMethodBody",		            0)
TIMERID(TIME_ParserDecls,                       "ParserDecls",                              0)







