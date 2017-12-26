=============================================================================
    CONSOLE APPLICATION : VBWF4FlowChart
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrate the usage of WF4 Sequence workflow in a Guess Number
Game workflow. this sample will also involve the usage of Variable, IFElse 
Activity, DoWhile Activity and Cutomized Activity. 


/////////////////////////////////////////////////////////////////////////////
Demo:

1. Open VBWF4FlowChart.sln with Visual Studio 2010.
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Create a new Workflow Console Workflow named it VBWF4FlowChart;

2. Create a new code file name it ReadNumberActivity.vb, file the file with 
   the following code:

Imports System
Imports System.Activities

Public Class ReadNumberActivity
    Inherits CodeActivity

    Public Property playerInputNumber() As OutArgument(Of Integer)
        Get
            Return _playerInputNumber
        End Get
        Set(ByVal value As OutArgument(Of Integer))
            _playerInputNumber = value
        End Set
    End Property
    Private _playerInputNumber As OutArgument(Of Integer)


    Protected Overrides Sub Execute(ByVal context As System.Activities.CodeActivityContext)

        playerInputNumber.Set(context, Integer.Parse(Console.ReadLine()))

    End Sub
End Class

3. Delete the default created Workflow1.xaml file thenc reate a new Activity name 
   it GuessNumberGameInflowChart.xaml;author the workflow just like the one exsited 
   in the project. 

4. Open MainModule.vb file, change the code as follow:

Imports System.Activities
Imports System.Activities.Statements

Module MainModule

    Dim s As Sequence

    Sub Main()
        WorkflowInvoker.Invoke(New GuessNumberGameInFlowChart())
    End Sub

End Module


/////////////////////////////////////////////////////////////////////////////
Reference:

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx