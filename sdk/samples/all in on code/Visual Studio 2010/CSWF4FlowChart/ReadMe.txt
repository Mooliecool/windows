=============================================================================
    CONSOLE APPLICATION : CSWF4FlowChart
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

A new control flow activity called Flowchart has been added to make it 
possible for developers to use the Flowchart model to define a workflow. The 
Flowchart more closely resembles the concepts and thought processes that 
many analysts and developers go through when creating solutions or designing 
business processes.  Therefore, it made sense to provide an activity to make 
it easy to model the conceptual thinking and planning that had already been 
done.  The Flowchart enables concepts such as returning to previous steps and 
splitting logic based on a single condition, or a Switch / Case logic.  

This code sample demonstrate the use of Workflow Foundation 4 Flowchart in a 
Guess Number game. 

To run the sample:
1. Open CSWF4FlowChart.sln with Visual Studio 2010.
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a new Workflow Console Workflow named it CSWF4FlowChart;

2. Create a new code file and name it ReadNumberActivity.cs. Fill the file 
   with the following code:

   using System;
   using System.Activities;

   namespace CS_WF4_SequenceWF
   {
       public sealed class ReadNumberActivity : CodeActivity
       {
           // Define an activity out argument of type int
           public OutArgument<int> playerInputNumber { get; set; }

           protected override void Execute(CodeActivityContext context)
           {
               playerInputNumber.Set(context,Int32.Parse(Console.ReadLine()));
           }
       }
   }

3. Delete the default created Workflow1.xaml and create a new Activity 
   GuessNumberGameInFlowChart.xaml. Author this workflow just like the one 
   already existed in the project. 

4. Open Program.cs file, change the code as follows:

   using System.Activities;
   using System.Activities.Statements;

   namespace CSWF4FlowChart
   {
       class Program
       {
           static void Main(string[] args)
           {
               WorkflowInvoker.Invoke(new GuessNumberGameInFlowChart());
           }
       }
   }


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Flowchart Workflows
http://msdn.microsoft.com/en-us/library/dd489448.aspx

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx


/////////////////////////////////////////////////////////////////////////////