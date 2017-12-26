=============================================================================
         CONSOLE APPLICATION : CSWF4Parallel
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrate the usage of WF4 Parallel activity, ForEach activity
and ParallelForEach activity.

The Parallal embedded child activities will execute asynchronously, but they
are still in the same thread. 

Child statements of ParallelForEach activity will execute asynchronously,just
like Parallel, statements are running in the same thread.You may notice that
only one Delay activity in the ParallelForEach take effects,while remain Delay
activities seem skipped.as a matter of fact,this is the ParallelForEach by 
design feature.Whenever the ParallelForEach's embedded statement goes idle.
the next statement will execute immediately rather than waiting there.

To run the sample:
1. Open CSWF4Parallel.sln with Visual Studio 2010.
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step 1.Create a new Workflow Console Application project,name it CSWF4Parallel

Step 2.Author a workflow in the default created Workflow1.xaml file(See the
       workflow exsited in the project.) 

Step 3.Press Ctrl+F5 to build the project and run it without debugging. 


/////////////////////////////////////////////////////////////////////////////
References:

Parallel Activity Designer
http://msdn.microsoft.com/en-us/library/ee829519.aspx


/////////////////////////////////////////////////////////////////////////////