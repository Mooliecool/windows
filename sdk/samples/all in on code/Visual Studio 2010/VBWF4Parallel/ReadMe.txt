=============================================================================
    CONSOLE APPLICATION : VBWF4Parallel
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates the usage of WF4 Parallel activity, ForEach activity
and ParallelForEach activity.

The Parallel embedded child activities will execute asynchronously, but they
still in the same thread. 

Child statements of ParallelForEach activity will execute asynchronously,just
like Parallel, statements are running in the same thread. You may notice that
only one Delay activity in the ParallelForEach take effects, while remain Delay
activities seem skipped. As a matter of fact, this is the ParallelForEach by 
design feature. Whenever the ParallelForEach's embedded statement goes idle.
the next statement will execute immediately rather than waiting there.


/////////////////////////////////////////////////////////////////////////////
Demo:

1. Open VBWF4Parallel.sln with Visual Studio 2010.
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1.Create a new Workflow Console Application project, name it VBWF4Parallel
Step 2.Author a workflow in the default created Workflow1.xaml file(See the
       workflow existed in the project.) 
Step 3.Press Ctrl+F5 to build the project and run it without debugging. 


/////////////////////////////////////////////////////////////////////////////
Reference:

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx