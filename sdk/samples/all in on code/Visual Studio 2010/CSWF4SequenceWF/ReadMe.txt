=============================================================================
       CONSOLE APPLICATION : CSWF4SequenceWF
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrate the use of Workflow Foundation 4 Sequence in a Guess 
Number game. The sample also involves the use of Variable, IFElse Activity, 
DoWhile Activity and Cutomized Activity. 

To run the code sample:
1. Open CSWF4SequenceWF.sln with Visual Studio 2010
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Creation:

1.Create A Workflow Console Application, name it CSWF4SequenceWF;

2.Create a CodeActivity name it ReadNumberActivity.cs. Fill the file with code:

  using System;
  using System.Activities;

  namespace CSWF4SequenceWF
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

3.Delete the default created Workflow1.xaml and create a new Activity 
  GuessNumberGameSequenceWF.xaml. Author this workflow just like the one 
  already existed in the project. 

4.Open Prgram.cs file, chage the code as follows:

  using System.Activities;
  namespace CSWF4SequenceWF
  {
      class Program
      {
          static void Main(string[] args)
          {
              WorkflowInvoker.Invoke(new GuessNumberGameSequenceWF() );
          }
      }
  }


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Windows Workflow Tutorial: Introduction to Sequential Workflows
http://msdn.microsoft.com/en-us/library/dd692925.aspx

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx


/////////////////////////////////////////////////////////////////////////////