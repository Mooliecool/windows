#
# 
#  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
# 
#  The use and distribution terms for this software are contained in the file
#  named license.txt, which can be found in the root of this distribution.
#  By using this software in any fashion, you are agreeing to be bound by the
#  terms of this license.
# 
#  You must not remove this notice, or any other, from this software.
# 
#

# Take out the CRT.
NO_NTDLL                        =1

TARGETNAME      =utilcode$(_CRTFLAVOR)
TARGETPATH      =$(CLR_LIB_DEST)

TARGETTYPE      =LIBRARY

PRECOMPILED_INCLUDE     = ..\stdafx.h
PRECOMPILED_CXX         = 1
PRECOMPILED_SOURCEFILE  = ..\stdafx.cpp
PRECOMPILED_OBJ         = stdafx_utilcode.obj

USER_C_FLAGS     = $(USER_C_FLAGS) -DUNICODE -D_UNICODE $(_CRTIMPFLAGS) -DENABLE_PERF_COUNTERS -DNO_CRT
INCLUDES         = $(INCLUDES);$(VCTOOLS_INC_PATH)

SOURCES=                        \
        ..\apithreadstress.cpp     \
        ..\arraylist.cpp           \
        ..\assemblyfilehash.cpp    \
        ..\ccomprc.cpp             \
        ..\check.cpp               \
        ..\circularlog.cpp         \
        ..\CorImage.cpp            \
        ..\Debug.cpp               \
        ..\ex.cpp                  \
        ..\COMex.cpp               \
        ..\GuidFromName.cpp        \
        ..\jitperf.cpp             \
        ..\md5.cpp                 \
        ..\memorypool.cpp          \
        ..\memoryreport.cpp        \
        ..\regutil.cpp             \
        ..\format1.cpp             \
        ..\GenericStackProbe.cpp \
        ..\sighelper.cpp           \
        ..\util.cpp                \
        ..\UTSem.cpp               \
        ..\PostError.cpp           \
        ..\PrettyPrintSig.cpp      \
        ..\rangetree.cpp           \
        ..\sbuffer.cpp             \
        ..\sstring.cpp             \
        ..\sstring_com.cpp         \
        ..\StgPool.cpp             \
        ..\StgPooli.cpp            \
        ..\StgPoolReadOnly.cpp     \
        ..\splitpat.cpp            \
        ..\timeline.cpp            \
        ..\utilmessagebox.cpp      \
        ..\InternalDebug.cpp       \
        ..\makepath.cpp            \
        ..\log.cpp                 \
        ..\loaderheap.cpp          \
        ..\winfix.cpp              \
        ..\safewrap.cpp            \
        ..\DbgAlloc.cpp            \
        ..\PerfLog.cpp             \
        ..\NamespaceUtil.cpp       \
        ..\OutString.cpp           \
        ..\ilFormatter.cpp         \
        ..\pedecoder.cpp           \
        ..\OpInfo.cpp              \
        ..\wsinfo.cpp              \
        ..\clrhost.cpp             \
        ..\tick.cpp                \
        ..\DacUtil.cpp             \
        ..\Sigparser.cpp         \
      
