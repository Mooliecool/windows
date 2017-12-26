=============================================================================
        CONSOLE APPLICATION : CppResourceLeaks Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CppResourceLeaks is designed to show typical resource leaks and their 
consequences. It focuses on two situations of resource leaks:

1. Handle Leak:

A handle leak is a type of software bug that occurs when a computer program 
asks for a handle to a resource but does not free the handle when it is no 
longer used. If this occurs frequently or repeatedly over an extended period 
of time, a large number of handles may be marked in-use and thus unavailable, 
causing performance problems or a crash. The leak of handle itself is not a 
big problem. The problem is that handle leak causes the leak of kernel paged 
pool.

2. Memory Leak:

A memory leak is a particular type of unintentional memory consumption by a 
computer program where the program fails to release memory when no longer 
needed. This condition is normally the result of a bug in a program that 
prevents it from freeing up memory that it no longer needs. Memory is 
allocated to a program, and that program subsequently loses the ability to 
access it due to program logic flaws.


/////////////////////////////////////////////////////////////////////////////
Symptoms:

A. Handle Leaks

1. System running out of non-paged pool.

2. Failing handle allocations.

3. Degradation in system performance.

4. Excessive paging.

5. Tools like Task Manager and PerfMon show increasing handle count.

B. Memory Leaks

1. System running out of virtual memory.

2. Failing memory allocations.

3. Degradation in system performance.

4. Excessive paging.

5. Tools like Task Manager and PerfMon show increasing memory counters for 
Working Set Size, Commit Size and Virtual Memory Size.


/////////////////////////////////////////////////////////////////////////////
Causes:

A. Handle Leaks

1. The programmer mistakenly forget to close the handles. The OPEN and CLOSE 
operations are not in pairs.

2. Incorrect error handling.

3. Other process inject (duplicate) the handle to the current one, who is not
aware of the injection, and thus, does not close the injected handle.

B. Memory Leaks

1. Incorrect error handling.

2. Synchronization issues.

3. Heap block caching (like BSTR etc).

C. Heap Fragmentation

1. Mixing long-term with short-term allocations.

2. Mixing small size with large size allocations.

3. Using realloc to shrink blocks.

4. Heap leaks.


/////////////////////////////////////////////////////////////////////////////
Detections and Debugging:

A. Handle Leaks

- Step1. Is It Even a Handle Leak?

The first step of investigating a potential resource leak is to confirm that
there really is one. Handle leaks can be easily detected by using Task 
Manager. You can display the number of handles for a given process by 
clicking the Process tab of Task Manager followed by selecting the View and 
Select Columns submenu. This brings up a dialog box that displays a host of 
options that Task Manager is capable of displaying. Check the Handle Count 
check box, and click OK. If the Handles column shows a big number or its 
value continues going up and does not go down after letting the application 
sit idle for a while, the application is likely leaking handles. 

In the example of LeakFileHandle, the handle count increases by 2 and doesn't 
go down.

- Step2. Initial analysis

If we could identify what type of object the handle is associated with, it 
might give us a better clue to the source of the leak. For example, if all 
the preceding handles are thread handles, we could focus our efforts in those 
parts of the code. Process Explorer is an excellent tool that has the 
capability to show different handles and associated types in a process. 
(http://technet.microsoft.com/en-us/sysinternals/bb896653.aspx)
The newly created handles are highlighted for a short period. This facilitates  
the detection of the leaky handles. 

In the example of LeakFileHandle, Process Explorer displays the addition of 
the following two handles after the execution.

File	C:\Users\Jialiang Ge\AppData\Local\Temp\HLeEF15.tmp
File	C:\Users\Jialiang Ge\AppData\Local\Temp\HLeEF15.tmp

With the information of the leaky handles, we can try to spot the culprit by 
a simple code review. If the problem cannot be easily spotted, go on to the  
next step.

- Step3. Using Handle Leak Detection Tools

Several tools are available to help efficiently track down handle leaks. They 
are Application Verifier and !htrace.

Application Verifier

Application Verifier option "Disable invalid handle usage" or 
"appverif -enable Handles". This functionality is provided by appverif on 
Windows XP and later versions of OS. By activating Application Verifier, 
stack trace information is saved each time the process opens a handle, 
closes a handle, or references an invalid handle. It is this stack trace 
information that !htrace displays.

Handle Leak Detection Tools (!htrace)

!htrace, an extension command of windbg, can help you detect where the leak 
is occurring. Htrace stands for handle trace, and the basic idea behind the 
command is to enable the operating system to track all calls (with associated 
stack traces) that result in handles being opened and closed. When a leak has 
been identified, you can then use the !htrace extension command to display 
all the stack traces in the debugger. After all stack traces are shown, you 
can track down sporadic handle leaks in a much easier fashion.

The general strategy for using !htrace is

1. Prior to starting the actual reproducing of the leak, enable handle 
tracing (using !htrace 每enable).
2. Run the reproduction and let the process handle leaks.
3. Use !htrace 每diff to find the offending stacks.

Repeating steps 1每3 will give you enough information to narrow the problem 
down in the code and find the leak by using code reviews.

Before !htrace can be used, Application Verifier must be activated for the 
target process, and the Detect invalid handle usage option must be selected. 
By activating Application Verifier, stack trace information is saved each 
time the process opens a handle, closes a handle, or references an invalid
handle. It is this stack trace information that !htrace displays.

First, let's enable the handle trace:

0:000> !htrace -enable
Handle tracing enabled.
Handle tracing information snapshot successfully taken.

The 每enable switch is a two-step operation. First, it enables stack tracing, 
and second, it takes a snapshot of the current state of the process with 
regard to handles (as indicated by the second line in the output). As soon as 
stack tracing has been enabled, Windows starts recording all calls that 
result in handle creation and deletion. 

0:000> g
(19ec.153c): Break instruction exception - code 80000003 (first chance)
eax=7ffde000 ebx=00000000 ecx=00000000 edx=7798b412 esi=00000000 edi=00000000
eip=7794433c esp=0065fd0c ebp=0065fd38 iopl=0         nv up ei pl zr na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000246
ntdll!DbgBreakPoint:
7794433c cc              int     3

The next time you take a snapshot 
(using the 每snapshot option), the !htrace extension command queries the 
operating system for all stack traces that result in handle creation and 
deletion and displays them.

0:000> !htrace
--------------------------------------
Handle = 0x0000003c - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca310c: ntdll!ZwDuplicateObject+0x0000000c
0x75e68ad9: KERNELBASE!DuplicateHandle+0x00000069
0x77be34e1: kernel32!DuplicateHandleStub+0x000000b3
** WARNING: Unable to verify checksum for CppResourceLeaks.exe
0x00ab1627: CppResourceLeaks!LeakFileHandle+0x00000207
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
Handle = 0x00000038 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca2e3c: ntdll!ZwCreateFile+0x0000000c
0x75e9679f: KERNELBASE!CreateFileW+0x0000035e
0x77be51b3: kernel32!CreateFileWStub+0x00000069
0x00ab158f: CppResourceLeaks!LeakFileHandle+0x0000016f
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
Handle = 0x00000034 - CLOSE
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca2d3c: ntdll!NtClose+0x0000000c
0x75e87772: KERNELBASE!GetTempFileNameW+0x00000293
0x00ab14fe: CppResourceLeaks!LeakFileHandle+0x000000de
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
Handle = 0x00000034 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca2e3c: ntdll!ZwCreateFile+0x0000000c
0x75e9679f: KERNELBASE!CreateFileW+0x0000035e
0x75e87762: KERNELBASE!GetTempFileNameW+0x00000208
0x00ab14fe: CppResourceLeaks!LeakFileHandle+0x000000de
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
...
...
--------------------------------------
Handle = 0x00000024 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

...
--------------------------------------
Handle = 0x00000020 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

...

--------------------------------------
Parsed 0xC stack traces.
Dumped 0xC stack traces.

The output of !htrace shows all stack traces recorded for openning and 
closing handles. Some entries are in pairs. For example,

Handle = 0x00000034 - CLOSE
Thread ID = 0x000017ec, Process ID = 0x000017e8
...
--------------------------------------
Handle = 0x00000034 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8
...

It means that the handle 0x00000034 was opened and closed properply and was 
not leaked. !htrace has a handy command option, -diff, that correlates all 
paths that resulted in creation and deletion (since the last snapshot) and 
reports only the stack traces that do not have a delete stack associated.

0:000> !htrace -diff
Handle tracing information snapshot successfully taken.
0xd9 new stack traces since the previous snapshot.
Ignoring handles that were already closed...
Outstanding handles opened since the previous snapshot:
--------------------------------------
Handle = 0x0000003c - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca310c: ntdll!ZwDuplicateObject+0x0000000c
0x75e68ad9: KERNELBASE!DuplicateHandle+0x00000069
0x77be34e1: kernel32!DuplicateHandleStub+0x000000b3
0x00ab1627: CppResourceLeaks!LeakFileHandle+0x00000207
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
Handle = 0x00000038 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca2e3c: ntdll!ZwCreateFile+0x0000000c
0x75e9679f: KERNELBASE!CreateFileW+0x0000035e
0x77be51b3: kernel32!CreateFileWStub+0x00000069
0x00ab158f: CppResourceLeaks!LeakFileHandle+0x0000016f
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------
Handle = 0x00000024 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8
...
--------------------------------------
Handle = 0x00000020 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8
...
--------------------------------------
Displayed 0x4 stack traces for outstanding handles opened since the previous 
snapshot.

Therefore, our focus should be on the handles 0x3c, 0x38, 0x24, 0x20. 
!handle <handle> tells the type of the handle. For example, 

0:000> !handle 0x3c 
Handle 3c
  Type         	File

The call-stacks in the output of !htrace give the hint of the culprit 
functions that open the leaky handles. Take the 
CppResourceLeaks!LeakFileHandle function as an example:

--------------------------------------
Handle = 0x00000038 - OPEN
Thread ID = 0x000017ec, Process ID = 0x000017e8

0x77ca2e3c: ntdll!ZwCreateFile+0x0000000c
0x75e9679f: KERNELBASE!CreateFileW+0x0000035e
0x77be51b3: kernel32!CreateFileWStub+0x00000069
0x00ab158f: CppResourceLeaks!LeakFileHandle+0x0000016f
0x00ab1843: CppResourceLeaks!wmain+0x00000023
0x00ab1e48: CppResourceLeaks!__tmainCRTStartup+0x000001a8
0x00ab1c8f: CppResourceLeaks!wmainCRTStartup+0x0000000f
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b

One exception is that, if the call-stack looks very convoluted and does not 
make any sense for the target process, the handle is likedly to be injected 
(duplicated) from another process. Take the LeakInjectedHandle as an example:

0:000> !htrace -diff
Handle tracing information snapshot successfully taken.
0x1 new stack traces since the previous snapshot.
Ignoring handles that were already closed...
Outstanding handles opened since the previous snapshot:
--------------------------------------
Handle = 0x00000114 - OPEN
Thread ID = 0x00001700, Process ID = 0x00000d30

0x77ca310c: ntdll!ZwDuplicateObject+0x0000000c
0x75e68ad9: KERNELBASE!DuplicateHandle+0x00000069
0x77be34e1: kernel32!DuplicateHandleStub+0x000000b3
0x00101991: +0x00101991
0x00101b78: +0x00101b78
0x00102188: +0x00102188
0x00101fcf: +0x00101fcf
0x77be36d6: kernel32!BaseThreadInitThunk+0x0000000e
0x77c9883c: ntdll!__RtlUserThreadStart+0x00000070
0x77c9880f: ntdll!_RtlUserThreadStart+0x0000001b
--------------------------------------

Another example:
--------------------------------------
Handle = 0x000007D8 - OPEN
Thread ID = 0x00001700, Process ID = 0x00000d30

0x01001363: 09htarget!XcptFilter+0x00000009
0x010014D3: 09htarget!_NULL_IMPORT_DESCRIPTOR+0x000000CB
0x7C816FD7: kernel32!BaseProcessStart+0x00000023
--------------------------------------

The Process ID and Thread ID reported by !htrace -diff belong to the 
injecting process. We can therefore find out who duplicated the handle to 
the leaking proecss.

B. Memory Leaks

- Step1. Is It Even a Memory Leak?

We use Task Manager or Process Explorer to identify the potential memory 
leak. First, bring up Task Manager and select the Memory Usage and Virtual 
Memory Size columns in Pre-Vista machines, and the Memory - Working Set and 
Memory - Commit Size columns in Post-Vista machines. Virtual memory (aka. 
Commit Size or Private Bytes) indicates how much memory the process is using 
overall (both in and out of physical memory), whereas the Mem Usage (aka. 
Working Set) column shows how much physical memory the process is consuming. 
Typically, the best indicator for memory leaks is an increase in virtual 
memory size (e.g. Commmit Size) and not fluctuations in working set size.

Virtual Memmory
  Private Bytes (aka. Commit Size or Virtual Memory Size) 
  Peak Private Bytes
  Virtual Size
  Page Faults
  Page Fault Delta

Physical Memory
  Working Set (aka. Mem Usage)
    WS Private: physical memory owned by just this process
    WS Shareable: physical memory that could be shared with other processes
    WS Shared: shareable memory that is current shared with other processes 
	  (It is a subset of shareable WS. It tells the possible size of memory 
	  that will still be in use after the current process exits)
  Peak Working Set

Note: Private Bytes may be bigger than Working Set Size when much virtual 
memory is paged out. Private Bytes may also be smaller than Working Set Size 
because the memory allocations are in pages (influencing working set size) 
while the virtual memory size is in bytes which must be smaller than or equal 
to one page of memory.

In the example of LeakHeapMemory, we can observe in Task Manager the 
continous increase of both virtual memory size and memory usage.

- Step2. Using Memory Leak Detection Tools

Several tools are available to help efficiently track down memory leaks. They 
are UMDH, LeakDiag, !address, !heap, Pageheap, and CRTDBG.

UMDH
http://support.microsoft.com/kb/268343

UMDH works by taking snapshots of the virtual memory usage of a process at 
different points in time and logging them to text files. UMDH can compare the 
log files taken at 2 instances of time and list the allocations that are 
leaking. It requires GFLAGS "User Stack Trace Database Option" (+ust) to be 
enabled in order to record the stack traces of the memory allocation requests 
made by the process. Allocations and assocated stack traces in the log file 
are tagged with "BackTracexxxxx". UMDH uses these tags as identifiers for 
reporting leaks.

Note: UMDH tracks heap allocations only. In other words, it cannot track 
allocations that are originating from non-heap-related memory activity (such 
as calls to VirtualAlloc).

First, we need to enable stack traces for memory allocations. To accomplish 
this, we use the gflags tool and enable "Create user mode stack trace 
database" for CppResourceLeaks.exe.

	gflags -i CppResourceLeaks.exe +ust

The command needs to be run as administrator. It does not enable stack 
tracing for processes that are already running, but it enables stack tracing 
for all future executions of CppResourceLeaks.exe. Alternatively, you can set 
the flag through the GFLAGS user interface. Use the -ust option for gflags to 
disable the stack tracing when you are finished debugging. 

Second, we need to configure the debug symbols. One of the most important 
steps to using UMDH is to make sure that you have good symbol files (.dbg or 
.pdb file) to get a good stack trace. You can use the Microsoft Symbol Server 
to obtain debug symbol files (http://support.microsoft.com/kb/311503/). UMDH 
is capable of using dbghelp.dll to reading symbol files and resolving 
addresses to symbolic function names.

Third, start CppResourceLeaks.exe and get its process ID from Task Manager.

Fourth, use UMDH to get a heap dump before the apparent leak happens with the 
following command:

	umdh -p:PID -f:CppResourceLeaks1.log

Now you have a complete heap dump of the CppResourceLeaks process in the 
CppResourceLeaks1.log file. This file shows all of the allocations that were 
made and the callstacks where the allocations were made. The log in this 
state is not readable as the symbols are not resolved. UMDH can be instructed 
to resolve the symbols by simulating a log comparison with an empty log, 
using the command:
 
	umdh -v CppResourceLeaks1.log  > CppResourceLeaks1.txt

The resulting log contains something like:

+    2018 (  2018 -     0)      1 allocs	BackTrace73F28
+       1 (     1 -     0)	BackTrace73F28	allocations

	ntdll!RtlAllocateHeap+00000274
	kernel32!ConsoleAllocateCaptureBuffer+0000006F
	kernel32!ReadConsoleInternal+0000007E
	kernel32!ReadConsoleA+00000040
	kernel32!ReadFileImplementation+00000075
	MSVCR90D!_read_nolock+0000062C
	MSVCR90D!_read+00000219
	MSVCR90D!_filbuf+00000113
	MSVCR90D!getc+00000208
	MSVCR90D!_fgetchar+00000010
	MSVCR90D!getchar+0000000A
	CppResourceLeaks!LeakHeapMemory+0000003D (...\cppresourceleaks.cpp, 221)
	CppResourceLeaks!wmain+00000023 (...\cppresourceleaks.cpp, 260)
	CppResourceLeaks!__tmainCRTStartup+000001A8 (crtexe.c, 583)
	CppResourceLeaks!wmainCRTStartup+0000000F (crtexe.c, 403)
	kernel32!BaseThreadInitThunk+0000000E
	ntdll!__RtlUserThreadStart+00000070
	ntdll!_RtlUserThreadStart+0000001B

Fifth, while memory is leaking, take the second snapshot of the heap:

	umdh -p:PID -f:CppResourceLeaks2.log

Last, use UMDH to compare the UMDH logs:

	UMDH CppResourceLeaks1.log CppResourceLeaks2.log > cmp12.txt

In cmp12.txt, each log entry has the following syntax:

+ BYTES_DELTA (NEW_BYTES - OLD_BYTES) NEW_COUNT allocs BackTrace TRACEID
+ COUNT_DELTA (NEW_COUNT - OLD_COUNT) BackTrace TRACEID allocations
 ... stack trace ...

where:

 BYTES_DELTA - increase in bytes between before and after log
 NEW_BYTES - bytes in after log
 OLD_BYTES - bytes in before log
 COUNT_DELTA - increase in allocations between before and after log
 NEW_COUNT - number of allocations in after log
 OLD_COUNT - number of allocations in before log
 TRACEID - decimal index of the stack trace in the trace database (can be 
 used to search for allocation instances in the original UMDH logs).

For example (LeakHeapMemory()), 

+ 2a98c90 ( 2a98c90 -     0)  15cf4 allocs	BackTrace74050
+   15cf4 ( 15cf4 -     0)	BackTrace74050	allocations

	ntdll!RtlAllocateHeap+00000274
	CppResourceLeaks!LeakHeapMemory+00000083 (...\cppresourceleaks.cpp, 228)
	CppResourceLeaks!wmain+00000023 (...\cppresourceleaks.cpp, 260)
	CppResourceLeaks!__tmainCRTStartup+000001A8 (crtexe.c, 583)
	CppResourceLeaks!wmainCRTStartup+0000000F (crtexe.c, 403)
	kernel32!BaseThreadInitThunk+0000000E
	ntdll!__RtlUserThreadStart+00000070
	ntdll!_RtlUserThreadStart+0000001B
	
-    2018 (     0 -  2018)      0 allocs	BackTrace73F28
-       1 (     0 -     1)	BackTrace73F28	allocations

	ntdll!RtlAllocateHeap+00000274
	kernel32!ConsoleAllocateCaptureBuffer+0000006F
	kernel32!ReadConsoleInternal+0000007E
	kernel32!ReadConsoleA+00000040
	kernel32!ReadFileImplementation+00000075
	MSVCR90D!_read_nolock+0000062C
	MSVCR90D!_read+00000219
	MSVCR90D!_filbuf+00000113
	MSVCR90D!getc+00000208
	MSVCR90D!_fgetchar+00000010
	MSVCR90D!getchar+0000000A
	CppResourceLeaks!LeakHeapMemory+0000003D (...\cppresourceleaks.cpp, 221)
	CppResourceLeaks!wmain+00000023 (...\cppresourceleaks.cpp, 260)
	CppResourceLeaks!__tmainCRTStartup+000001A8 (crtexe.c, 583)
	CppResourceLeaks!wmainCRTStartup+0000000F (crtexe.c, 403)
	kernel32!BaseThreadInitThunk+0000000E
	ntdll!__RtlUserThreadStart+00000070
	ntdll!_RtlUserThreadStart+0000001B

According to the comparison, the first call-stack appears for 15cf4 times, 
and 2a98c90 bytes were leaked because of the LeakHeapMemory function.

!address

The !address extension command comes in very handy when you want to get a 
quick overview of where the memory in your process is really located. The 
command gives statistics, such as memory region usage in heaps, stack, free, 
and so on.

For example (LeakHeapMemory()), 

0:000> !address -summary
 ProcessParametrs 00381a18 in range 00380000 0039c000
 Environment 00380810 in range 00380000 0039c000

-------------------- Usage SUMMARY --------------------------
    TotSize (      KB)   Pct(Tots) Pct(Busy)   Usage
    11c4000 (   18192) : 00.87%    02.27%    : RegionUsageIsVAD
   4f132000 ( 1295560) : 61.78%    00.00%    : RegionUsageFree
     397000 (    3676) : 00.18%    00.46%    : RegionUsageImage
     200000 (    2048) : 00.10%    00.26%    : RegionUsageStack
       2000 (       8) : 00.00%    00.00%    : RegionUsageTeb
   2f760000 (  777600) : 37.08%    97.01%    : RegionUsageHeap
          0 (       0) : 00.00%    00.00%    : RegionUsagePageHeap
       1000 (       4) : 00.00%    00.00%    : RegionUsagePeb
          0 (       0) : 00.00%    00.00%    : RegionUsageProcessParametrs
          0 (       0) : 00.00%    00.00%    : RegionUsageEnvironmentBlock
       Tot: 7fff0000 (2097088 KB) Busy: 30ebe000 (801528 KB)

-------------------- Type SUMMARY --------------------------
    TotSize (      KB)   Pct(Tots)  Usage
   4f132000 ( 1295560) : 61.78%   : <free>
     398000 (    3680) : 00.18%   : MEM_IMAGE
     1be000 (    1784) : 00.09%   : MEM_MAPPED
   30968000 (  796064) : 37.96%   : MEM_PRIVATE

-------------------- State SUMMARY --------------------------
    TotSize (      KB)   Pct(Tots)  Usage
   2f4cf000 (  774972) : 36.95%   : MEM_COMMIT
   4f132000 ( 1295560) : 61.78%   : MEM_FREE
    19ef000 (   26556) : 01.27%   : MEM_RESERVE

Largest free region: Base 30f00000 - Size 2a970000 (697792 KB)

The column Pct(Tots) means the percentage of the entry in total virtual 
memory. The column Pct(Busy) means the percentage of the entry in busy 
virtual memory.

RegionUsageIsVAD - memory allocated by VirtualAlloc in VMM
RegionUsageHeap - memory allocated by heap manager

From the output

    11c4000 (   18192) : 00.87%    02.27%    : RegionUsageIsVAD
   2f760000 (  777600) : 37.08%    97.01%    : RegionUsageHeap

we see that most used memory is heap alloc, instead of virtual alloc, so it's 
a heap memory leak.

!heap -s, !heap -a, and !heap -x -v

The !heap -s command allows you to get a detailed look at the heap summary of 
the process and the suspicious heaps. Judging from the pattern of allocations 
in the !heap extension command output (e.g. there are tons of blocks 
allocated of same user size), chances are good that we can locate the heap 
blocks that are leaked. Furthermore, by looking around at the heap block 
contents (e.g. does it contain ASCII characters? does it correspond to the 
address of some function / symbol?) we may see how / why the block was 
allocated. 

Please note that because a lot of changes happened to the heap manager in 
Windows Vista and the later operating system, the allocation of heap entries 
may vary. For example, the allocated block may be bigger than requested, or 
the allocation granually grows in size.

To prove that this is indeed a leak, you can search for references to the 
block in the process's memory space. If these potentially leaked blocks were 
being used (perhaps cached), there would need to be a reference somewhere in 
memory that points to that heap block. If there are no references, it means 
that we definitely have a leak. The !heap -x -v allows you to search the 
entire memory space of the process for the presence of a specified address.

For example (LeakHeapMemory()), 

0:000> !heap -s
  Heap     Flags   Reserv  Commit  Virt   Free  List   UCR  Virt  Lock  Fast 
                    (k)     (k)    (k)     (k) length      blocks cont. heap 
-----------------------------------------------------------------------------
00150000 00000002   16384  16352  16352      2     0     1    0      0   L  
00250000 00008000      64     12     12     10     1     1    0      0      
00380000 00001002      64     44     44      9     2     1    0      0   L  
-----------------------------------------------------------------------------

The heap 00150000 occupies abnormally large memory.

0:000> !heap -a 00120000
        ...
        00246240: 00200 . 00200 [01] - busy (1f4)
        00246440: 00200 . 00200 [01] - busy (1f4)
        00246640: 00200 . 00200 [01] - busy (1f4)
        00246840: 00200 . 00200 [01] - busy (1f4)
        00246a40: 00200 . 00200 [01] - busy (1f4)
        00246c40: 00200 . 00200 [01] - busy (1f4)
        00246e40: 00200 . 00200 [01] - busy (1f4)
        00247040: 00200 . 00200 [01] - busy (1f4)
        00247240: 00200 . 00200 [01] - busy (1f4)
        ...

We find a large number of blocks with the same user allocation size (1f4). 
This is usually a good indicator that they are potentially leaked blocks. 
The next step is to find out what these blocks actually contain. If we were 
leaking memory, it would be reasonable to expect data related to our 
application contained within those blocks:

0:000> db 00246c40+0x8
00246c48  41 6c 6c 2d 49 6e 2d 4f-6e 65 20 43 6f 64 65 20  All-In-One Code 
00246c58  46 72 61 6d 65 77 6f 72-6b 00 00 00 00 00 00 00  Framework.......

Before we come to the conclusion that this is in fact a leak, we should 
verify it by searching for references to the block in the process's memory 
space. 

0:000> !heap -x -v 00246c40+0x8
Entry     User      Heap      Segment       Size  PrevSize  Unused    Flags
-----------------------------------------------------------------------------
00246c40  00246c48  00150000  00150640       200       200         c  busy 

Search VM for address range 00246c40 - 00246e3f : 

The search yielded zero results. As stated before, if a currently allocated 
heap block is not referenced anywhere in memory, we can safely say that we 
are leaking that block.

!heap -l

The !heap -l command causes debugger to look for leaked heap blocks. It 
automates the act of dumping out all heap blocks (!heap -s) and 
systematically searching for any potentially leaked blocks (!heap -x -v). 
Please note that !heap -l does not work if full page heap is enabled for the 
process.

For example (LeakHeapMemory()), 

0:000> !heap -l
Searching the memory for potential unreachable busy blocks.
Heap 00150000
Heap 00250000
Heap 00380000
Scanning VM ...
Scanning references from 32822 busy blocks (16 MBytes) ....
Entry     User      Heap      Segment       Size  PrevSize  Unused    Flags
-----------------------------------------------------------------------------
00154640  00154648  00150000  00150000       200       200         c  busy 
00154840  00154848  00150000  00150000       200       200         c  busy 
00154a40  00154a48  00150000  00150000       200       200         c  busy 
00154e40  00154e48  00150000  00150000       200       200         c  busy 
00155040  00155048  00150000  00150000       200       200         c  busy 
00155240  00155248  00150000  00150000       200       200         c  busy 
00155640  00155648  00150000  00150000       200       200         c  busy 
00155840  00155848  00150000  00150000       200       200         c  busy 
...

29050 potential unreachable blocks were detected.

Pageheap, and !heap -p -a

After you have identified a potential leak culprit using the above !heap 
commands, it would be useful to see which stack trace made the allocation to 
begin with. If we had that, we could find out exactly what the code was doing 
and what it was allocating.

First, we need to enable stack tracing using Application Verifier. Second, 
run !heap -p -a upon the address that we thought was leaking. Not only will 
we see general information about the leaked address (such as which heap it's 
in and the trace ID), but we also get the full stack trace of the code that 
made the allocation. From here, it is a trivial exercise to code review and 
find the culprit code.

Note, while using page heap, !heap -s, !heap -a, !heap -x -v and !heap -l may 
not work at all! We should find the culprit memory block and run !heap -p -a 
upon it directly.

For example (LeakHeapMemory()), 

0:000> !address 0b768e08
Usage:                  PageHeap
Base Address:           0b768000
End Address:            0b769000
Region Size:            00001000
Type:                   00020000	MEM_PRIVATE
State:                  00001000	MEM_COMMIT
Protect:                00000004	PAGE_READWRITE
More info:              !heap -p 0x150000
More info:              !heap -p -a 0xb768e08

0:000> !heap -p -a 0xb768e08
    address 0b768e08 found in
    _DPH_HEAP_ROOT @ 151000
    in busy allocation (  DPH_HEAP_BLOCK:  UserAddr  UserSize - VirtAddr VirtSize)
                                 b72e700:  b768e08   1f4 -      b768000  2000
    7c83d9aa ntdll!RtlAllocateHeap+0x00000e9f
    0039fd2c vfbasics!AVrfpRtlAllocateHeap+0x000000b1
    00401046 CppResourceLeaks!LeakHeapMemory+0x00000046

Not only do we see general information about the leaked address (such as 
which heap it's in and the trace ID), but we also get the full stack trace of 
the code that made the allocation. From here, it is a trivial exercise to 
code review and find the culprit code.

CRTDBG
http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx

Debug version of C Run Time Library has facilities to debug C Run Time Heap 
related problems like leaks and corruption. It requires application to be 
rebuilt with debug CRT. 

When _DEBUG is defined the following functions call the respective _xxx_dbg() 
versions which provide extra debugging capabilities: malloc, realloc, calloc, 
expand, free, msize.

When _CRTDBG_MAP_ALLOC is defined the _xxx_dbg() versions of the CRT heap 
functions are call directly instead of the standard versions. This enables 
the _xxx_dbg() funtions to record the location (source file path & line 
number) where the allocation function is being invoked. The information can 
be used to identify the location in the source code where the block was 
allocated.

After enabling the above flags, debug CRT heap manager can perform various 
types of checking like heap corruptions and heap leaks in run-time or debug-
time:

	_CrtSetDbgFlag() retrieves and modifies the behavior of the debug heap
	_CrtCheckMemory() performs integrity check on memory blocks
	_CrtDumpMemoryLeaks() dumps all heap blocks when memory leaks occurs

For example ((LeakCRTHeapMemory()), 

First, we enable the debug heap functions, include the following statements 
in the program:

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

Note: The #include statements must be in the order shown here. If you change 
the order, the functions you use may not work properly. 

By including crtdbg.h, you map the malloc and free functions to their debug 
versions, _malloc_dbg and _free_dbg, which keep track of memory allocation 
and deallocation. This mapping occurs only in a debug build (in which _DEBUG 
is defined). Release builds use the ordinary malloc and free functions.

At the end of the function LeakCRTHeapMemory, add the line 

	_CrtDumpMemoryLeaks();

When you run your program under the debugger, _CrtDumpMemoryLeaks displays 
memory leak information in the Output window. The memory leak information 
looks like this:

	Detected memory leaks!
	Dumping objects ->
	...\cppresourceleaks.cpp(257) : {101} normal block at 0x00511F40, 500 bytes long.
	 Data: <                > CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD 
	Object dump complete.


/////////////////////////////////////////////////////////////////////////////
Fixes:

A. Handle Leaks

1. Be careful to close the handles after they are created.

2. Consider employing an auto acquire/release construct. Very similar to auto 
pointers, this construct allows you to acquire a handle at any given scope 
and automatically free it when the auto construct goes out of scope. 

B. Memory Leaks

1. Be careful that the allocation and deallocation of memory on the heap 
should be paired and should target the same heap. The most common pairs are:

	new				-		delete
	malloc			-		free
	GlobalAlloc		-		GlobalFree
	LocalAlloc		-		LocalFree
	CoTaskMemAlloc	-		CoTaskMemFree
	SysAllocString	-		SysFreeString

2. Consider using an auto construct that automatically deletes memory when 
the variable goes out of scope, such as auto_ptr in STL.

3. Consider overloading the allocation APIs used in your application. This 
allows for trapping all calls to memory allocations, thereby giving you hooks 
to all memory allocations performed by your applications. The allocation 
hooks can then be used to track memory allocations, simulate failures in 
memory allocations, and much more.


/////////////////////////////////////////////////////////////////////////////
References:

Mario Hewardt & Daniel Pravat, Advanced Windows Debugging Ch. 9 (2007), at 
http://advancedwindowsdebugging.com/.  Copyright 2008 by Pearson Education, 
Inc. This material may be distributed only subject to the terms and 
conditions set forth in the Open Publication License, v1.0 or later (the 
latest version is presently available at http://www.opencontent.org/openpub/).  
Excerpted by Jialiang Ge, 2009. 

Wiki: Handle leak
http://en.wikipedia.org/wiki/Handle_leak

Debug Tutorial Part 5: Handle Leaks
http://www.codeproject.com/KB/debug/cdbntsd5.aspx

Use !htrace to debug handle leak
http://blogs.msdn.com/junfeng/archive/2008/04/21/use-htrace-to-debug-handle-leak.aspx

Resource Leaks: Detecting, Locating, and Repairing Your Leaky GDI Code
http://msdn.microsoft.com/en-us/magazine/cc301756.aspx

Wiki: Memory leak
http://en.wikipedia.org/wiki/Memory_leak

The poor man's way of identifying memory leaks
http://blogs.msdn.com/oldnewthing/archive/2005/08/15/451752.aspx

Umdhtools.exe: How to use Umdh.exe to find memory leaks
http://support.microsoft.com/kb/268343

MSDN: Memory Leak Detection and Isolation
http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx


/////////////////////////////////////////////////////////////////////////////