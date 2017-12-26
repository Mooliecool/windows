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

// Any class with a vtable that needs to be instantiated
// during debugging data access must be listed here.

VPTR_CLASS(Thread)

VPTR_CLASS(EEJitManager)
VPTR_CLASS(EEJitDebugInfoStore)

#ifndef FJITONLY 
VPTR_CLASS(EECodeManager)
#else
VPTR_CLASS(Fjit_EETwain)
#endif


VPTR_CLASS(RangeList)
VPTR_CLASS(LockedRangeList)

VPTR_CLASS(Module)
VPTR_CLASS(ReflectionModule)

VPTR_CLASS(AppDomain)
VPTR_CLASS(BaseDomain)
VPTR_CLASS(SharedDomain)
VPTR_CLASS(SystemDomain)

VPTR_CLASS(DomainAssembly)
VPTR_CLASS(DomainModule)

VPTR_CLASS(CNonVirtualThunkMgr)
VPTR_CLASS(CVirtualThunkMgr)
VPTR_CLASS(MethodDescPrestubManager)
VPTR_CLASS(StubLinkStubManager)
VPTR_CLASS(ThePreStubManager)
VPTR_CLASS(ThunkHeapStubManager)
VPTR_CLASS(VirtualCallStubManager)
VPTR_CLASS(VirtualCallStubManagerManager)
VPTR_CLASS(JumpStubStubManager)
VPTR_CLASS(EntryPointStubManager)
VPTR_CLASS(ILStubManager)
VPTR_CLASS(NDirectDispatchStubManager)
VPTR_CLASS(ComPlusDispatchStubManager)
VPTR_CLASS(ReverseInteropStubManager)
VPTR_CLASS(DelegateInvokeStubManager)
VPTR_CLASS(PEFile)
VPTR_CLASS(PEAssembly)
VPTR_CLASS(PEModule)
VPTR_CLASS(PEImageLayout)
VPTR_CLASS(RawImageLayout)
VPTR_CLASS(ConvertedImageLayout)
VPTR_CLASS(MappedImageLayout)
VPTR_CLASS(FlatImageLayout)
VPTR_CLASS(StreamImageLayout)
VPTR_CLASS(ContextTransitionFrame)
VPTR_CLASS(CustomGCFrame)
VPTR_CLASS(DebuggerClassInitMarkFrame)
VPTR_CLASS(DebuggerSecurityCodeMarkFrame)
VPTR_CLASS(DebuggerExitFrame)
VPTR_CLASS(FaultingExceptionFrame)
VPTR_CLASS(FramedMethodFrame)
VPTR_CLASS(FuncEvalFrame)
VPTR_CLASS(GCFrame)
VPTR_CLASS(HelperMethodFrame)
VPTR_CLASS(HelperMethodFrame_1OBJ)
VPTR_CLASS(HelperMethodFrame_2OBJ)
VPTR_CLASS(HelperMethodFrame_PROTECTOBJ)
VPTR_CLASS(InlinedCallFrame)

VPTR_CLASS(InterceptorFrame)
VPTR_CLASS(LeaveRuntimeFrame)
VPTR_CLASS(SecureDelegateFrame)
VPTR_CLASS(SecurityContextFrame)
VPTR_CLASS(MulticastFrame)
VPTR_CLASS(DelegateTransitionFrame)
VPTR_CLASS(NDirectMethodFrameGeneric)
#ifdef _X86_
VPTR_CLASS(NDirectMethodFrameSlim)
#endif // _X86_
VPTR_CLASS(NDirectMethodFrameStandalone)
VPTR_CLASS(NDirectMethodFrameStandaloneCleanup)
VPTR_CLASS(PrestubMethodFrame)
VPTR_CLASS(ProtectByRefsFrame)
VPTR_CLASS(ProtectValueClassFrame)
VPTR_CLASS(ReverseEnterRuntimeFrame)
VPTR_CLASS(SecurityFrame)
VPTR_CLASS(TPMethodFrame)
VPTR_CLASS(UMThkCallFrame)
#ifdef _X86_ 
VPTR_CLASS(TailCallFrame)
#endif
VPTR_CLASS(ExceptionFilterFrame)

#ifdef _DEBUG
VPTR_CLASS(AssumeByrefFromJITStack)
#endif 

#ifdef DEBUGGING_SUPPORTED 
VPTR_CLASS(Debugger)
VPTR_CLASS(EEDbgInterfaceImpl)
VPTR_CLASS(DebugInfoManager)
#endif // DEBUGGING_SUPPORTED

VPTR_CLASS(DebuggerMethodInfoTable)

VPTR_CLASS(LoaderCodeHeap)
VPTR_CLASS(HostCodeHeap)

