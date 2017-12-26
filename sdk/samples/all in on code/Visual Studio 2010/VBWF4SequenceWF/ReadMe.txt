=============================================================================
    CONSOLE APPLICATION : VBWF4SequenceWF
=============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates the usage of WF4 Sequence workflow in a Guess Number
Game workflow. this sample will also involve the usage of Variable, IFElse 
Activity, DoWhile Activity and Customized Activity. 

/////////////////////////////////////////////////////////////////////////////
Demo:

1. Open VBWF4SequenceWF.sln with Visual Studio 2010
2. Press Ctrl+F5.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2010
2. .NET Framework 4.0


/////////////////////////////////////////////////////////////////////////////
Implementation:

1.Create A Workflow Console Application, name it VBWF4SequenceWF;
2.Create a CodeActivity name it ReadNumberActivity.vb, fill the file with code:

Imports System.Activities

Namespace VBWF4SequenceWF

    Public NotInheritable Class ReadNumberActivity
        Inherits CodeActivity

        ' Define an activity out argument of type int
        Public Property playerInputNumber() As OutArgument(Of Int32)
            Get
                Return _playerInputNumber
            End Get
            Set(ByVal value As OutArgument(Of Integer))
                _playerInputNumber = value
            End Set
        End Property

        Private _playerInputNumber As OutArgument(Of Integer)

        Protected Overrides Sub Execute(ByVal context As CodeActivityContext)

            playerInputNumber.Set(context, Int32.Parse(Console.ReadLine()))

        End Sub
    End Class

End Namespace


/////////////////////////////////////////////////////////////////////////////
Reference:

A Developer's Introduction to Windows Workflow Foundation (WF) in .NET 4
http://msdn.microsoft.com/en-us/library/ee342461.aspx