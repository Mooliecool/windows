//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This file defines logging level for VB compiler. The control guids are in vblogging.ctl.
//
//-------------------------------------------------------------------------------------------------

#pragma once

/* Guidelines for using Levels:

There are six different logging levels. They are intended to allow usefull logging of all VB components,
detailed logging of individual components, stress logging for errors/fatal conditions and
informational logging across a few component to assist with debugging investigations.
































*/

// Logging levels
#define LVL_CRITICAL     1
#define LVL_FATAL        2
#define LVL_ERROR        3
#define LVL_WARNING      4
#define LVL_INFORMATION  5
#define LVL_VERBOSE      6
#define LVL_VERY_VERBOSE 7

#ifdef VBDBGEE
#define DISABLE_LOGGING
#endif // VBDBGEE

// Standard definition for LOG(COMPID, LEVEL, MSG, ...) logging macro
// In order to provide a custom definition - define CUSTOM_LOGGING_MACRO prior to including this file

#ifndef DISABLE_LOGGING

// Set a define that indicates that WMI logging is enabled
#define ENABLED_WMI_LOGGING 1

#ifndef CUSTOM_LOGGING_MACRO

#define WPP_CHECK_FOR_NULL_STRING 1
#define WPP_GLOBALLOGGER 1

#define WPP_COMPID_LEVEL_LOGGER(xflags,lvl) (WPP_CONTROL(0).Logger),
#define WPP_COMPID_LEVEL_ENABLED(xflags,lvl) \
   ((WPP_CONTROL(0).Flags[WPP_FLAG_NO(WPP_BIT_ ## xflags)] & WPP_MASK(WPP_BIT_ ## xflags)) && \
    (WPP_CONTROL(0).Level >= lvl))

#define WPP_LEVEL_COMPID_LOGGER(lvl, xflags)  WPP_COMPID_LEVEL_LOGGER(xflags,lvl)
#define WPP_LEVEL_COMPID_ENABLED(lvl, xflags) WPP_COMPID_LEVEL_ENABLED(xflags,lvl)

#endif // CUSTOM_LOGGING_MACRO

#else // !DISABLE_LOGGING

// WMI logging is not turned on
#undef ENABLED_WMI_LOGGING
// Define the clean and init APIs to be nothing
#define WPP_CLEANUP()
#define WPP_COMPID_LEVEL_ENABLED(xflags,lvl) false
#define WPP_INIT_TRACING(str)

// Define the default logging macros to mean nothing if logging is disabled
#if !defined(CUSTOM_LOGGING_MACRO) && defined(DISABLE_LOGGING)

#pragma warning(push)
#pragma warning (disable:4002) // too many arguments to a macro

#define LOG(x)
#define LOG_FATAL(x)
#define LOG_ERROR(x)
#define LOG_WARN(x)
#define LOG_INFO(x)
#define LOG_VERBOSE(x)
#define LOG_VVERBOSE(x)

#pragma warning(pop)

#endif // !defined(CUSTOM_LOGGING_MACRO) && defined(DISABLE_LOGGING)

#endif // !DISABLE_LOGGING
