'****************************** Module Header ******************************\
' Module Name:  GameOverBehavior.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The GameOverBehavior is an attached behavior that is used to report the result to the user
' when a game is over.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/ 

Imports VBWPFMVVMPractice.TicTacToeViewModel

Public Class GameOverBehavior
    Inherits DependencyObject

    Public Shared Function GetReportResult(ByVal obj As DependencyObject) As Nullable(Of Boolean)
        Return DirectCast(
            obj.GetValue(GameOverBehavior.ReportResultProperty), 
            Nullable(Of Boolean))
    End Function

    Public Shared Sub SetReportResult(ByVal obj As DependencyObject, ByVal value As Nullable(Of Boolean))
        obj.SetValue(GameOverBehavior.ReportResultProperty, value)
    End Sub

    ' Using a DependencyProperty as the backing store for ReportResult.  This enables 
    ' animation, styling, binding, etc...     
    Public Shared ReadOnly ReportResultProperty As DependencyProperty =
        DependencyProperty.RegisterAttached(
            "ReportResult",
            GetType(Nullable(Of Boolean)),
            GetType(GameOverBehavior),
            New UIPropertyMetadata(
                Nothing,
                Function(sender As DependencyObject, e As DependencyPropertyChangedEventArgs)
                    GameOverBehavior.OnReportResultPropertyChanged(sender, e)
                End Function))

    ''' <summary>
    ''' The ReportResultProperty's PropertyChangedCallback method.
    ''' In the method, get the TicTacToeViewModel instance and subscribe to its GameOverEvent.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Shared Sub OnReportResultPropertyChanged(ByVal sender As DependencyObject,
                                                     ByVal e As DependencyPropertyChangedEventArgs)
        Dim obj As FrameworkElement = TryCast(sender, FrameworkElement)

        ' This ensures that the GameOverEvent is subscribed only once.
        If ((Not obj.DataContext Is Nothing) AndAlso (obj.DataContext.GetType Is GetType(TicTacToeViewModel))) Then
            RemoveHandler TryCast(obj.DataContext, TicTacToeViewModel).GameOverEvent,
                New GameOverEventHandler(AddressOf GameOverBehavior.GameOverBehavior_GameOverEvent)
            AddHandler TryCast(obj.DataContext, TicTacToeViewModel).GameOverEvent,
                New GameOverEventHandler(AddressOf GameOverBehavior.GameOverBehavior_GameOverEvent)
        End If
    End Sub

    ''' <summary>
    ''' In the GameOver event handler, show the result of the current game via a MessageBox.
    ''' </summary>
    Private Shared Sub GameOverBehavior_GameOverEvent(ByVal sender As Object, ByVal e As GameOverEventArgs)
        If e.IsTie Then
            MessageBox.Show("No winner!", "Game Over")
        Else
            MessageBox.Show((e.WinnerName & " has won, Congratulations!"), "Game Over")
        End If
    End Sub

End Class


