# ==++==
#
#   
#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
#   
#    The use and distribution terms for this software are contained in the file
#    named license.txt, which can be found in the root of this distribution.
#    By using this software in any fashion, you are agreeing to be bound by the
#    terms of this license.
#   
#    You must not remove this notice, or any other, from this software.
#   
#
# ==--==
#
# Build instructions for this directory
#

!if "$(FEATURE_PAL)" != "1" || "$(ROTOR_TOOLSET)" == "MSVC"

# WarningControl.h disables too many warnings.  VM folder will not use it.
COMPILER_WARNINGS       =
!if "$(_TGTCPU)"=="i386"
COMPILER_WARNINGS       = $(COMPILER_WARNINGS) -W4
!endif

!endif

INCLUDES                = ..;..\$(_TGTCPU);$(O);..\..\ceegen\inc;..\..\classlibnative\inc;..\..\md\inc;..\..\xmlparser;..\..\fusion\inc;$(VCTOOLS_INC_PATH);$(INCLUDES);..\..\debug\inc\$(_TGTCPU)

TARGETNAME                      =cee_$(_CORFLAVOR)
TARGETPATH                      =$(CLR_LIB_DEST)
TARGETTYPE                      =LIBRARY

USER_C_FLAGS                    = $(USER_C_FLAGS) -DUNICODE -D_UNICODE
# helpful undocumented switch that shows the include hierarchy:  -d1Bshow_incl

NO_NTDLL                        =1
!undef USE_LIBCMT

!if "$(_BUILDTYPE)" == "chk"
USER_C_FLAGS = $(USER_C_FLAGS) -DBUILDENV_CHECKED=1
!ENDIF

!IF !$(FREEBUILD)
USER_C_FLAGS = $(USER_C_FLAGS) -DWRITE_BARRIER_CHECK=1
ASM_DEFINES=$(ASM_DEFINES)     -DWRITE_BARRIER_CHECK=1
!ENDIF


!IF "$(LTCG)" == "1"
USER_C_FLAGS = $(USER_C_FLAGS) -wd4702
!ENDIF

NTTARGETFILE0 = $(O)/asmconstants.inc

!IF "$(PROFILING_SUPPORTED_BUILD)"=="1"
USER_C_FLAGS                     = $(USER_C_FLAGS) -DENABLE_PERF_COUNTERS
!ENDIF

# helpful undocumented switch that shows the include hierarchy:  -d1Bshow_incl

!if "$(PLATFORM_UNIX)" != "1"
# Profile guided optimization
!IF "$(_BUILDARCH)"=="x86"
# USER_C_FLAGS= $(USER_C_FLAGS) -d2pv$(CLRBASE)\src\inc\mscorwks.pgd
!ENDIF
!endif

PRECOMPILED_INCLUDE             = ..\common.h
PRECOMPILED_CXX                 = 1
PRECOMPILED_SOURCEFILE          = ..\common.cpp

#This supresses the annoying warnings from build.exe
CONDITIONAL_INCLUDES=$(CONDITIONAL_INCLUDES) vgmem.h unistd.h alphaops.h stddef.h macwin32.h time.h urlmon.h iostream.h opcode.def assert.h sys\mman.h \usr\include\malloc.h ole2int.h sys\param.h

#=============================================================
# Lets keep the list of sources in ALPHABETICAL order please!
# These all seem to be relative to the previous directory
# because this file is included by wks\SOURCES and svr\SOURCES
#=============================================================
SOURCES1 = \
        ..\AppDomain.cpp \
        ..\AppDomainHelper.cpp \
        ..\AppDomainNative.cpp \
        ..\appdomainstack.cpp \
        ..\array.cpp \
        ..\Assembly.cpp \
        ..\AssemblyName.cpp \
        ..\AssemblyNative.cpp \
        ..\AssemblySpec.cpp \
        ..\AssemblySink.cpp \
        ..\binder.cpp \
        ..\cachelinealloc.cpp \
        ..\ceeload.cpp \
        ..\ceemain.cpp \
        ..\certificatecache.cpp \
        ..\class.cpp       \
        ..\clrex.cpp \
        ..\clsload.cpp \
        ..\codeman.cpp \
        ..\ConfigHelper.cpp \
	..\ConstrainedExecutionRegion.cpp \
        ..\COMArrayHelpers.cpp \
        ..\COMArrayInfo.cpp \
        ..\ComCallableWrapper.cpp \
        ..\COMCurrency.cpp \
        ..\COMDateTime.cpp \
        ..\COMDecimal.cpp \
        ..\COMDelegate.cpp \
        ..\COMDynamic.cpp \
        ..\COMIsolatedStorage.cpp \
        ..\COMMemoryFailPoint.cpp \
        ..\COMMethodRental.cpp \
        ..\COMNumber.cpp \
        ..\Domainfile.cpp \
        ..\testhookmgr.cpp \

SOURCES2 = \
        ..\COMModule.cpp \
        ..\MarshalNative.cpp \
        ..\COMObject.cpp \
        ..\COMReflectionCommon.cpp \
        ..\COMStreams.cpp \
        ..\COMString.cpp \
        ..\COMStringBuffer.cpp \
        ..\COMSynchronizable.cpp \
        ..\COMSystem.cpp \
        ..\COMThreadPool.cpp \
        ..\COMUtilNative.cpp \
        ..\COMVariant.cpp \
        ..\COMVarArgs.cpp \
        ..\COMWaitHandle.cpp \
        ..\CorHost.cpp \
        ..\CustomMarshalerInfo.cpp \
        ..\CrossDomainCalls.cpp \
        ..\crst.cpp        \
        ..\contexts.cpp \
        ..\CustomAttribute.cpp \
        ..\debugHelp.cpp \
        ..\decodeMD.cpp \
        ..\DebugDebugger.cpp \
        ..\DebugInfoStore.cpp \
        ..\DynamicMethod.cpp \
        ..\ecall.cpp       \
        ..\eeconfig.cpp \
        ..\eeconfigfactory.cpp \
        ..\eecontract.cpp \
        ..\eedbginterfaceimpl.cpp \
        ..\eehash.cpp\
        ..\EEMessageBox.cpp\
        ..\EEPolicy.cpp \
        ..\EETwain.cpp \
        ..\EventStore.cpp  \
        ..\excep.cpp       \
        ..\ExState.cpp \
        ..\fcall.cpp       \
        ..\Field.cpp \
        ..\fptrstubs.cpp \
        ..\Fjit_EETwain.cpp \
        ..\frames.cpp \
        ..\FusionBind.cpp \

SOURCES3 = \
        ..\fusioninit.cpp \
        ..\FusionSink.cpp \
        ..\fusionwrap.cpp \
        ..\gccommon.cpp \
        ..\GCDecode.cpp \
        ..\gcEEsvr.cpp \
        ..\gcEEwks.cpp \
        ..\gchost.cpp \
        ..\gcscan.cpp \
        ..\gcsvr.cpp \
        ..\gcwks.cpp \
        ..\genericdict.cpp \
        ..\generics.cpp \
        ..\genmeth.cpp \
        ..\handletable.cpp \
        ..\handletableCache.cpp \
        ..\handletableCore.cpp \
        ..\handletableScan.cpp \
        ..\hash.cpp \
        ..\hosting.cpp \
        ..\HostExecutionContext.cpp \
        ..\ILMarshalers.cpp \
        ..\ILStubCache.cpp \
        ..\ILStubResolver.cpp \
        ..\instmethhash.cpp \
        ..\InteropConverter.cpp \
        ..\interoputil.cpp \
        ..\InvokeUtil.cpp \
        ..\contractImpl.cpp \
        ..\jithelpers.cpp \
        ..\JITInterface.cpp \
        ..\list.cpp \
        ..\ListLock.cpp \
        ..\memberload.cpp \
        ..\ManagedMdImport.cpp \
        ..\message.cpp \
        ..\Method.cpp \
        ..\MethodIter.cpp \
        ..\MethodImpl.cpp \
        ..\MethodTable.cpp \
        ..\ml.cpp \
        ..\mlcache.cpp \
        ..\mlgen.cpp \
        ..\mlinfo.cpp \
        ..\NativeOverlapped.cpp \
        ..\NewCompressedStack.cpp \
        ..\DllImport.cpp \
        ..\DllImportCallback.cpp \
        ..\FieldMarshaler.cpp \
        ..\object.cpp \
        ..\ObjectClone.cpp \
        ..\objecthandle.cpp \
        ..\ObjectList.cpp
        
SOURCES4 = \
        ..\OleVariant.cpp \
        ..\PEFile.cpp      \
        ..\PEImage.cpp      \
        ..\PEImageLayout.cpp      \
        ..\pendingload.cpp \
        ..\Prestub.cpp \
        ..\Precode.cpp \
        ..\ProfToEEInterfaceImpl.cpp \
        ..\ReflectClassWriter.cpp \
        ..\ReflectionInvocation.cpp \
        ..\remoting.cpp \
        ..\RuntimeHandles.cpp \
        ..\rwlock.cpp \
        ..\SafeHandle.cpp \
        ..\SecurityAttributes.cpp \
        ..\SecurityConfig.cpp \
        ..\SecurityDeclarative.cpp \
        ..\SecurityDescriptor.cpp \
        ..\SecurityHostProtection.cpp \
        ..\SecurityDescriptorAppdomain.cpp \
        ..\SecurityDescriptorAssembly.cpp \
        ..\securitymeta.cpp \
        ..\SecurityImperative.cpp \
        ..\SecurityPolicy.cpp \
        ..\SecurityStackWalk.cpp \
        ..\SecurityTransparentAssembly.cpp \
        ..\siginfo.cpp \
        ..\SigFormat.cpp \
        ..\SimpleRWLock.cpp \
        ..\SourceLine.cpp \
        ..\spinlock.cpp \
        ..\StackingAllocator.cpp \
        ..\stackwalk.cpp \
        ..\StackBuilderSink.cpp \
        ..\StackCompressor.cpp \
        ..\strongname.cpp \
        ..\stublink.cpp \
        ..\StringLiteralMap.cpp \
        ..\stubmgr.cpp \
        ..\syncblk.cpp \
        ..\synch.cpp \
        ..\SyncClean.cpp \
        ..\synchronizationcontextnative.cpp \
        ..\threads.cpp \
        ..\tokeniter.cpp \
        ..\typectxt.cpp \
        ..\typedesc.cpp \
        ..\typehandle.cpp \
        ..\typehash.cpp \
        ..\typeparse.cpp \
        ..\typestring.cpp \
        ..\util.cpp \
        ..\validator.cpp \
        ..\vars.cpp \
        ..\verifier.cpp \
        ..\VirtualCallStub.cpp \
        ..\Win32Threadpool.cpp \


SOURCES_PAL = \
        ..\gmheap.cpp \
        

#============================================================
# Was the file you added to the list inserted in ALPHABETICAL
# order - if not please rerun the ALPHABETIZE IMA!
#============================================================

#
# For proper incremental building, this needs to be the relative
# path from the {wks|srv} directory to the processor directory
#

I386_SOURCES_DIR = ..\i386

I386_SOURCES = \
    $(I386_SOURCES_DIR)\asmhelpers.asm \
    $(I386_SOURCES_DIR)\cgenx86.cpp \
    $(I386_SOURCES_DIR)\ExcepX86.cpp \
    $(I386_SOURCES_DIR)\fptext.asm \
    $(I386_SOURCES_DIR)\gmsasm.asm \
    $(I386_SOURCES_DIR)\gmsx86.cpp \
    $(I386_SOURCES_DIR)\jithelp.asm \
    $(I386_SOURCES_DIR)\JITinterfaceX86.cpp \
    $(I386_SOURCES_DIR)\Profiler.cpp \
    $(I386_SOURCES_DIR)\remotingx86.cpp \
    $(I386_SOURCES_DIR)\StubLinkerX86.cpp \
    ..\ExInfo.cpp \
    $(SOURCES_NONPAL) \
    $(SOURCES_COMINTEROP)

ROTOR_X86_SOURCES_DIR = ..\rotor_x86

ROTOR_X86_SOURCES = \
    $(SOURCES_PAL) \
    $(ROTOR_X86_SOURCES_DIR)\cgenx86.cpp \
    $(ROTOR_X86_SOURCES_DIR)\excepx86.cpp \
    $(ROTOR_X86_SOURCES_DIR)\asmhelpers.asm \
    $(ROTOR_X86_SOURCES_DIR)\jithelp.asm \
    $(ROTOR_X86_SOURCES_DIR)\jitinterfacex86.cpp \
    ..\exinfo.cpp \
    $(ROTOR_X86_SOURCES_DIR)\profiler.cpp \
    $(ROTOR_X86_SOURCES_DIR)\remotingx86.cpp \
    $(ROTOR_X86_SOURCES_DIR)\stublinkerx86.cpp \
    $(ROTOR_X86_SOURCES_DIR)\gmsx86.cpp \
    $(ROTOR_X86_SOURCES_DIR)\gmsasm.asm

PPC_SOURCES_DIR = ..\ppc

PPC_SOURCES = \
    $(SOURCES_PAL) \
    $(PPC_SOURCES_DIR)\cgenppc.cpp \
    $(PPC_SOURCES_DIR)\excepppc.cpp \
    $(PPC_SOURCES_DIR)\asmhelpers.s \
    $(PPC_SOURCES_DIR)\jithelp.s \
    $(PPC_SOURCES_DIR)\jitinterfaceppc.cpp \
    ..\exinfo.cpp \
    $(PPC_SOURCES_DIR)\profiler.cpp \
    $(PPC_SOURCES_DIR)\remotingppc.cpp \
    $(PPC_SOURCES_DIR)\gmsppc.cpp \
    $(PPC_SOURCES_DIR)\gmsasm.s

