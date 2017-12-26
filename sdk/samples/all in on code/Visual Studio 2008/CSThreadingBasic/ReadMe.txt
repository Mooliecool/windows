============================================================================
    Console Application : CSThreadingBasic Project Overview
============================================================================


/////////////////////////////////////////////////////////////////////////////
Demo:

After running the sample, you are expected to see the following output,

Main Thread's managed thread id: 9

Method2: Current thread's managed thread id: 6
0

Method3: Current thread's managed thread id: 6
1

Method1: Current thread's managed thread id: 10
2

Method4: Current thread's managed thread id: 12 Message
3


/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to create threads using C#.NET in three 
different approaches. And it also shows how to create a thread that require
a parameter. In the target threads, it also shows how to use lock keyword to
ensure a code snippet executed without interruption.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. The first way directly create a new thread manually using Thread.Start().

2. The second and third way both use the thread from the thread pool to
execute the target function. But the ThreadPool.QueueUserWorkItem has better 
performance than the ThreadStart.BeginInvoke

3. The forth way use the ParameterizedThreadStart to show how to create a 
thread calling target function with parameter. We pass the parameters when we
call the thread.Start() function

4. In the target function execution, the codes use lock keyword to ensure a
code snippet's execute to be atomic.


////////////////////////////////////////////////////////////////////////////

Reference:

C# Threading Programming Guid,
http://msdn.microsoft.com/en-us/library/ms173178(VS.80).aspx

Thread Class document,
http://msdn.microsoft.com/en-us/library/system.threading.thread.aspx

ThreadPool.QueueWorkItem,
http://msdn.microsoft.com/en-us/library/kbf0f1ct.aspx

////////////////////////////////////////////////////////////////////////////


