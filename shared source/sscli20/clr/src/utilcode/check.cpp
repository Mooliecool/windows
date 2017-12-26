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
//================================================================================
// Assertion checking infrastructure
//================================================================================

#include "stdafx.h"
#include <check.h>
#include <sstring.h>
#include <ex.h>
#include <contract.h>

#ifdef _DEBUG
size_t CHECK::s_cLeakedBytes = 0;
size_t CHECK::s_cNumFailures = 0;
#endif

BOOL CHECK::s_neverEnforceAsserts = 0;


// Trigger triggers the actual check failure.  The trigger may provide a reason
// to include in the failure message.

void CHECK::Trigger(LPCSTR reason) 
{
    StackScratchBuffer scratch;
    StackSString message;
    const char *messageString = NULL;

    EX_TRY
    {
        FAULT_NOT_FATAL();

        message.AppendASCII(reason);
        message.AppendASCII(": ");
        if (m_message != NULL)
            message.AppendASCII((m_message != (LPCSTR)1) ? m_message : "<runtime check failure>");

#if _DEBUG
        message.AppendASCII("FAILED: ");
        message.AppendASCII(m_condition);
#endif

        messageString = message.GetANSI(scratch);
    }
    EX_CATCH
    {
        messageString = "<exception occurred while building failure description>";
    }
    EX_END_CATCH(SwallowAllExceptions);

#if _DEBUG
    DbgAssertDialog((char*)m_file, m_line, (char *)messageString);
#else
    OutputDebugStringA(messageString);
    DebugBreak();
#endif

}

#ifdef _DEBUG
// Setup records context info after a failure.

void CHECK::Setup(LPCSTR message, LPCSTR condition, LPCSTR file, INT line) 
{
    // 
    // It might be nice to collect all of the message here.  But for now, we will just
    // retain the innermost one.
    //

    if (m_message == NULL)
    {
        m_message = message;
        m_condition = condition;
        m_file = file;
        m_line = line;
    }

#ifdef _DEBUG
    else if (IsInAssert())
    {
        FAULT_NOT_FATAL();

        EX_TRY
        {
            // Try to build a stack of condition failures

            StackSString context;
            context.Printf("%s\n\t%s%s FAILED: %s\n\t\t%s, line: %d",
                           m_condition, 
                           message && *message ? message : "", 
                           message && *message ? ": " : "", 
                           condition,
                           file, line);

            m_condition = AllocateDynamicMessage(context);
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions)
    }
#endif

}

LPCSTR CHECK::FormatMessage(LPCSTR messageFormat, ...)
{
    LPCSTR result = NULL;

    // We never delete this allocated string. A dtor would conflict with places
    // we use this around SEH stuff. We could have some fancy stack-based allocator,
    // but that's too much work for now. In fact we believe that leaking is a reasonable
    // policy, since allocations will only happen on a failed assert, and a failed assert 
    // will generally be fatal to the process.

    // The most common place for format strings will be in an assert; in
    // which case we don't care about leaking.
    // But to be safe, if we're not-inside an assert, then we'll just use
    // the format string literal to avoid allocated (and leaking) any memory.

    if (!IsInAssert())
        result = messageFormat;
    else
    {
        FAULT_NOT_FATAL();

        EX_TRY
        {
            SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        
            // Format it.   
            va_list args;
            va_start( args, messageFormat);

            SString s;
            s.VPrintf(messageFormat, args);

            va_end(args);

            result = AllocateDynamicMessage(s);

        }
        EX_CATCH
        {
            result = messageFormat;    
        }
        EX_END_CATCH(SwallowAllExceptions)
    }

    return result;
}

LPCSTR CHECK::AllocateDynamicMessage(const SString &s)
{
    // Make a copy of it.
    StackScratchBuffer buffer;
    const char * pMsg = s.GetANSI(buffer);

    // Must copy that into our own field.
    size_t len = strlen(pMsg) + 1;    
    char * p = new char[len];
    strcpy(p, pMsg);

    // But we'll keep counters of how much we're leaking for diagnostic purposes.
    s_cLeakedBytes += len;
    s_cNumFailures ++;

    // This should never fire. 
    // Note use an ASSERTE (not a check) to avoid a recursive deadlock.
    _ASSERTE(s_cLeakedBytes < 10000 || !"Warning - check misuse - leaked over 10,000B due to unexpected usage pattern");
    return p;
}

#endif
