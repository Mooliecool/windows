=============================================================================
        CONSOLE APPLICATION : CppStackCorruption Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CppStackCorruption is designed to show stack corruption and its consequences.
It demonstrates two typical situations of statck corruption: 

A. Stack Overrun
   1. Array indexing errors cause the stack overrun.
   2. Static buffer overrun on the stack.

B. Calling Convention Mismatch

You can run the example in either Debug or Release mode. The Release 
configuration is better because the disassembly is simpler and the asserts 
are off. In this example, we also disable the optimizations in the Release 
configuration. It makes sure that the sample functions are run by the 
processor.


/////////////////////////////////////////////////////////////////////////////
Symptoms:

Unlike stack overflow, stack corruption does not have an explicit error 
called "stack corruption" or "stack overrun". Instead, it is presented in the 
form of:

1. Crash (generally with an Access Violation error resulting from the EIP 
register pointing to an invalid memory).

2. Unpredictable behavior of the application.

3. Security holes.


/////////////////////////////////////////////////////////////////////////////
Causes:

Stack Overrun and Calling Convention Mismatch are the two typical causes 
(situations) of the Stack Corruption problems:

A. Stack Overrun

As the book 'Advanced Windows Debugging' writes, A stack overrun occurs when 
a thread indiscriminately overwrites portions of its call stack reserved for 
other purposes. This can include, but is not limited to, overwriting the 
return address for a particular frame, overwriting entire frames, or even 
exhausting the stack completely. The net effect of stack overruns ranges from 
crashes to unpredictable behavior and even serious security holes. Stack 
overruns have become one of the most common attack angles for malicious 
software, as they can potentially allow the attacker to gain complete control 
of the computer on which the faulty software runs, by overwriting the return 
address of the current function on the stack.

1. Array indexing errors cause the stack overrun. (See ArrayIndexingError)
Array indexing errors are a source of memory overruns. Careful bounds 
checking and index management will help prevent this type of memory overrun.

2. Static buffer overrun on the stack. (See: OverrunStaticBuffer)
A static buffer overrun occurs when a buffer, which has been declared on the 
stack, is written to with more data than it was allocated to hold. The less 
apparent versions of this error occur when unverified user input data is 
copied directly to a static variable using operations such as CopyMemory, 
strcat, strcpy, or wcscpy, causing potential stack corruption.

The above overruns may overwrite the return address for the current frame, 
or overwrite entire frames, or even exhaust the stack completely, and thus 
cause the stack corruption. 

    a) If the overwritten return address points to an invalid memory, the 
    application crashes with Access Violation once the current function 
    returns. (See: ArrayIndexingError/b)

    b) If the overwritten return address happens to point to a valid memory, 
    the application will have unpredictable behavior depending on the content 
    in that memory. The application may also crash at some point.

    c) If the overwritten return address points to malicious codes that 
    attackers injected, it will cause serious security problems.
    (See: ArrayIndexingError/a)

B. Calling Convention Mismatch (See: MismatchCallingConvention)

When the called function takes parameters, the mismatch of the calling 
convention results in the incorrect cleanup of the stack for parameters:

    a) The caller uses _cdecl to call a _stdcall function: 
    The stack space for the function parameters is double freed.

    b) The caller uses _stdcall to call a _cdecl function:
    The stack space for the function parameters is not freed by mistake.

The concequence of the mismatch of the calling convention is unpredictable. 
It may crash the application soon after the function returns, or it may have 
no impact on the process's execution at all.


/////////////////////////////////////////////////////////////////////////////
Debugging:

Here we take the AV-crash caused by stack overrun as an example.

Step1. Fire up the application under the debugger and let it run until the 
crash occurs.

Step2. Dump the call-stack:

0:000> k
ChildEBP RetAddr  
WARNING: Frame IP not in any known module. Following frames may be wrong.
0029fe70 0029feb8 0xbad4
0029fe74 009211a5 0x29feb8
0029feb8 75c436d6 CppStackCorruption!__tmainCRTStartup+0x10f
0029fec4 770b883c kernel32!BaseThreadInitThunk+0xe
0029ff04 770b880f ntdll!__RtlUserThreadStart+0x70
0029ff1c 00000000 ntdll!_RtlUserThreadStart+0x1b

The stack looks broken.

Step3. Check the instruction that was processed when the crash happened by 
dumping the EIP register, and looking at the memory that EIP is pointing to.

0:000> r eip
eip=0000bad4

0:000> dd eip
0000bad4  ???????? ???????? ???????? ????????
0000bae4  ???????? ???????? ???????? ????????
0000baf4  ???????? ???????? ???????? ????????
0000bb04  ???????? ???????? ???????? ????????
0000bb14  ???????? ???????? ???????? ????????
0000bb24  ???????? ???????? ???????? ????????
0000bb34  ???????? ???????? ???????? ????????
0000bb44  ???????? ???????? ???????? ????????

0:000> !address eip
 ProcessParametrs 003f29a0 in range 003f0000 0040d000
 Environment 003f07f8 in range 003f0000 0040d000
    00000000 : 00000000 - 00010000
                    Type     00000000 
                    Protect  00000001 PAGE_NOACCESS
                    State    00010000 MEM_FREE
                    Usage    RegionUsageFree

The contents of that memory location are a series of question marks, which we 
know indicate inaccessible memory. The output of !address also proves it. 
From this trivial exercise, we can hypothesize that the instruction pointer 
the processor uses to control the flow of execution in our application has 
gotten into a corrupt state. Both the corrupted stack and the corrupted eip 
tell us that, it's very likely to be a overwritting of the return address of 
some function that causes the corruptions. Next, we need to find out the 
erroneous function.

Unfortunately, finding the erroneous function is the most difficult part of 
the problem, because the record of the previous function call is cleaned up 
in the meantime. Microsoft is currently in progress of developing a tracing 
tool named TTT (tttrace.exe). TTT stands for Time Travel Tracing, it captures 
the execution of the program at instruction level, preserving scheduling 
information of all the threads and all memory updates. Once this information 
is captured in a trace file, it can be opened in windbg (which is the only 
debugger that supports it) and then it can be used to replay the execution of 
the program. It allows you almost everything that you can do during normal 
live debugging and since it is a recorded trace so you can go forward and 
backward in time with it. This gives you the unique ability to debug the 
problems efficiently and without holding the machine. Moreover if you see the 
effect of a bug after the cause is gone, then you can rewind the trace and 
then investigate it. Therefore, the job of finding the erroneous function for 
the crash problem caused by stack corruption will be much easier with the 
help of TTT.


/////////////////////////////////////////////////////////////////////////////
Detections:

1. Detect stack overrun at compile time.

Because stack buffer overruns are such common problems, there is a tool that 
can help detect these errors at compile time: PREfast.

PREfast is the codename for the /analyze compiler switch and SAL annotations:
http://blogs.msdn.com/vcblog/archive/2008/02/05/prefast-and-sal-annotations.aspx
To enable PREfast, open the project's properties, and turn to the Code 
Analysis / General node. Change the value of "Enable Code Analysis For C/C++ 
on Build" to "Yes (/analyze)", then rebuild the project. You will see warning 
messages of buffer overrun in the Error List window of Visual Studio. In this 
example, it throws these warnings for ArrayIndexingError:

warning C6386: Buffer overrun: accessing 'n', the writable size is '8' bytes, 
but '12' bytes might be written
warning C6201: Index '2' is out of valid index range '0' to '1' for possibly 
stack allocated buffer 'n'	
warning C6201: Index '3' is out of valid index range '0' to '1' for possibly 
stack allocated buffer 'n'	

, and these warnings for StaticBufferOverrun:

warning C6204: Possible buffer overrun in call to 'wcscpy': use of unchecked 
parameter 'pszSource'
warning C4996: 'wcscpy': This function or variable may be unsafe. Consider 
using wcscpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. 

2. Detect stack overrun and calling convention mismatch at run-time.

Unfortunately, Application Verifier cannot detect stack overrun and calling 
convention mismatch effectively at run-time. 

In VC++ compiler, there is an option to enable the stack frame runtime error 
checking: Project Properties / C/C++ / Code Generation / Basic Runtime Check. 
Stack frame checking is enabled when the option is set to "Stack Frames 
(/RTCs)". The stack frame checking switch is on in Debug build, and it is off 
in Release build by default. The option helps protect against a number of 
different stack corruptions:

	Each time a function is called, it initializes all local variables to 
	nonzero values to prevent them from retaining old values from prior 
	function calls.
	
	It verifies the stack pointer (esp register) to ensure that stack 
	corruptions caused by calling convention mismatches do not occur.
	
	Protects against buffer overruns and underruns of local variables.

When any of the three corruptions happens, the application is broken into a 
debugger with a break instruction exception. The callstack is like this:

0:000> k
ChildEBP RetAddr  
0014efc4 00ac1ba9 ntdll!DbgBreakPoint
0014fe18 00ac1bef CppStackCorruption!failwithmessage+0x1ea
0014fe28 00ac18a6 CppStackCorruption!_RTC_Failure+0x37
0014fe4c 00ac1091 CppStackCorruption!_RTC_CheckEsp+0x18
0014fe54 00ac11fc CppStackCorruption!wmain+0x11
0014fe98 75c436d6 CppStackCorruption!__tmainCRTStartup+0x10f
0014fea4 770b883c kernel32!BaseThreadInitThunk+0xe
0014fee4 770b880f ntdll!__RtlUserThreadStart+0x70
0014fefc 00000000 ntdll!_RtlUserThreadStart+0x1b

It is important to note that the /RTC compiler options are designed to work 
with debug builds and, as such, have no impact on released builds. The /RTC 
switch is meant solely to test your code during development.

Other viable options to detect stack corruption at run-time include Rational's
Purify or NuMega's BoundsChecker.


/////////////////////////////////////////////////////////////////////////////
Fixes:

1. Array indexing errors are generally caused by the developers' carelessness. 
We can use PREfast to eliminate such errors.

2. While copying a buffer, if the source buffer can be of variable length 
without upper boundaries, allocating the target buffer memory on the stack is 
not proper. Without knowing the size of the source at compile time, it is 
impossible to allocate a buffer on the stack that could hold the source. If 
this is the case, allocating the buffer from the heap is a better approach.

3. It is recommended to replace all occurences of the unsafe buffer-copy APIs 
such as strcat, strcpy, or wcscpy with the secure ones like StringCchCopy, 
strcpy_s, and wcscpy_s, _mbscpy_s. The secure APIs allow you to specify the 
size of the destination string to ensure that no more than the specified 
number of characters are ever copied to the destination. We can use PREfast 
to find all unsafe uses of buffer-copy APIs, and replace them one by one.

4. The developer should study the calling convention of the target function 
when dynamically loading a DLL. The calling convention can be studied by 
either reading the prolog and epilog of the target function, or by reading 
the document of the library if it exists, or by looking at the linker 
decoration of the export.


/////////////////////////////////////////////////////////////////////////////
References:

Mario Hewardt & Daniel Pravat, Advanced Windows Debugging Ch. 5 (2007), at 
http://advancedwindowsdebugging.com/.  Copyright 2008 by Pearson Education, 
Inc. This material may be distributed only subject to the terms and 
conditions set forth in the Open Publication License, v1.0 or later (the 
latest version is presently available at http://www.opencontent.org/openpub/).  
Excerpted by Jialiang Ge, 2009. 

Avoiding Buffer Overruns
http://msdn.microsoft.com/en-us/library/ms717795.aspx

Debug Tutorial Part 2: The Stack
http://www.codeproject.com/KB/debug/cdbntsd2.aspx


/////////////////////////////////////////////////////////////////////////////