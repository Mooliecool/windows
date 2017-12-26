=============================================================================
         CONSOLE APPLICATION : CppStackOverflow Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CppStackOverflow is designed to show how stack overflow happens in C++ 
applications. When a thread is created, 1MB of virtual memory is reserved for 
use by the thread as a stack. Unlike the heap, it does not expand as needed. 
Its initial size can be changed via /STACK linker switch (See Project 
Property Pages / Linker / System / Stack Reserve Size). When too much memory 
is used on the call stack the stack is said to overflow, typically resulting 
in a program crash. This class of software bug is usually caused by one of 
two types of programming errors: allocation of large stack variable and 
deeply recursive function calls.


/////////////////////////////////////////////////////////////////////////////
Symptoms:

The application crashes with the Stack Overflow error.


/////////////////////////////////////////////////////////////////////////////
Causes:

There are three possible causes of the Stack Overflow error:

1. A thread uses the entire stack reserved for it. This is often caused by 
deep recursion or allocation of very large stack variable.

2. A thread cannot extend the stack because the page file is maxed out, and 
therefore no additional pages can be committed to extend the stack. 

3. A thread cannot extend the stack because the system is within the brief 
period used to extend the page file. 


/////////////////////////////////////////////////////////////////////////////
Debugging:

A. Visual Studio

Step1. Set a breakpoint (F9) at the end of the StackOverflow() function where 
it is calling StackOverflow() again recursively.

Step2. Press F5 to start debugging.

Step3. Press Ctrl+Alt+M or Click the Memory button in the Debug / Windows 
menu item to bring up the Memory window.

Step4. Type ESP in the Memory window, and press ENTER.  Note the block of  
memory on the stack set to zero.

Step5. Press F5 to recursively call StackOverflow. This hits the breakpoint 
again.

Step6. Type ESP in the Memory window, and press ENTER. This time the block of 
memory on the stack was filled with 0x01 bytes. But if you scroll down, the 
block of 0x00 bytes is below. The recursive calls to StackOverflow means that 
the stack space will not get cleaned up until all of the nested functions 
return. In this case, that will never happen.

Step7. Press F5 for several more times. Eventually, you get a stack overflow 
error.

B. Windbg

Step1. See what event caused the debugger to break in:

0:000> .lastevent
Last event: 2388.2f4c: Stack overflow - code c00000fd (!!! second chance !!!)
  debugger time: Thu Apr  2 16:35:59.191 2009 (GMT+8)

Step2 Dump the callstack:

0:000> k
ChildEBP RetAddr  
0013abd4 00da1453 CppStackOverflow!_chkstk+0x27
00153370 00da1453 CppStackOverflow!StackOverflow+0x93 
0016bb0c 00da1453 CppStackOverflow!StackOverflow+0x93 
001842a8 00da1453 CppStackOverflow!StackOverflow+0x93 
0019ca44 00da1453 CppStackOverflow!StackOverflow+0x93 
001b51e0 00da1453 CppStackOverflow!StackOverflow+0x93 
001cd97c 00da1453 CppStackOverflow!StackOverflow+0x93 
001e6118 00da1453 CppStackOverflow!StackOverflow+0x93 
001fe8b4 00da1453 CppStackOverflow!StackOverflow+0x93 
00217050 00da1453 CppStackOverflow!StackOverflow+0x93 
0022f7ec 00da1515 CppStackOverflow!StackOverflow+0x93 
0022f8c4 00da1b18 CppStackOverflow!wmain+0x25 
0022f914 00da195f CppStackOverflow!__tmainCRTStartup+0x1a8 
0022f91c 76e04911 CppStackOverflow!wmainCRTStartup+0xf 
0022f928 77d6e4b6 kernel32!BaseThreadInitThunk+0xe
0022f968 77d6e489 ntdll!__RtlUserThreadStart+0x23
0022f980 00000000 ntdll!_RtlUserThreadStart+0x1b

Step3. Calculate the approximate size of the current stack:

0:000> ?? (0x0022f980 - @esp)
unsigned int 0xf4db4

The current size is 0xf4db4 bytes (i.e. 979KB), which is near the limit (1MB).


/////////////////////////////////////////////////////////////////////////////
Detections:

1. Detecting stack overflow at compile time.

The VC++ compiler itself is capable of detecting infinite recursion to some 
extent. When the compiler finds such a problem, it throws the following 
warning in the error list.

warning C4717: 'StackOverflow' : recursive on all control paths, function 
will cause runtime stack overflow

If we enable PREfast in the compiler (Project Properties / Code Analysis / 
General node. Change the value of "Enable Code Analysis For C/C++ on Build" 
to "Yes (/analyze)"), the compiler will also report the overuse of the stack:

warning C6262: Function uses '100012' bytes of stack: exceeds 
/analyze:stacksize'16384'. Consider moving some data to heap

2. Detecting stack overflow at run-time.

The application directly crashes with the Stack Overflow error at run-time.


/////////////////////////////////////////////////////////////////////////////
Fixes:

1. Limit the recursion depth.

2. Keep large allocations on the heap rather than the stack.


/////////////////////////////////////////////////////////////////////////////
References:

Wiki: Stack overflow
http://en.wikipedia.org/wiki/Stack_overflow

Debugging Tools for Windows: Debugging a Stack Overflow
http://msdn.microsoft.com/en-us/library/cc267849.aspx

How to trap stack overflow in a Visual C++ application
http://support.microsoft.com/kb/315937

MSDN: 6.1.4 Stack Overflow Handling
http://msdn.microsoft.com/en-us/library/aa227159.aspx


/////////////////////////////////////////////////////////////////////////////
