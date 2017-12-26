'****************************** Module Header ******************************\
' Module Name:  RelayCommand.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The RelayCommand class provides an implementation of the ICommand interface.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class RelayCommand
    Implements ICommand

    Private ReadOnly _canExecute As Predicate(Of Object)
    Private ReadOnly _execute As Action(Of Object)

    Public Event CanExecuteChanged As EventHandler Implements ICommand.CanExecuteChanged
       
    ''' <summary>
    ''' Creates a new command that can always execute.
    ''' </summary>
    ''' <param name="execute">The execution logic</param>
    Public Sub New(ByVal execute As Action(Of Object))
        Me.New(execute, Nothing)
    End Sub

    ''' <summary>
    ''' Creates a new command.
    ''' </summary>
    ''' <param name="execute">The execution logic</param>
    ''' <param name="canExecute">The execution status logic</param>
    Public Sub New(ByVal execute As Action(Of Object), ByVal canExecute As Predicate(Of Object))
        If (execute Is Nothing) Then
            Throw New ArgumentNullException("execute")
        End If
        Me._execute = execute
        Me._canExecute = canExecute
    End Sub

    Public Function CanExecute(ByVal parameter As Object) As Boolean Implements ICommand.CanExecute
        Return ((Me._canExecute Is Nothing) OrElse Me._canExecute.Invoke(parameter))
    End Function

    Public Sub Execute(ByVal parameter As Object) Implements ICommand.Execute
        Me._execute.Invoke(parameter)
    End Sub

End Class



