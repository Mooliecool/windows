'****************************** Module Header ******************************\
' Module Name:  ChangeDimensionBehavior.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The ChangeDimensionBehavior class is an attached behavior that is used to change the 
' dimension of the game.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/ 

Public Class ChangeDimensionBehavior
    Inherits DependencyObject

    Public Shared Function GetDimension(ByVal obj As DependencyObject) As Integer
        Return CInt(obj.GetValue(ChangeDimensionBehavior.DimensionProperty))
    End Function

    Public Shared Sub SetDimension(ByVal obj As DependencyObject, ByVal value As Integer)
        obj.SetValue(ChangeDimensionBehavior.DimensionProperty, value)
    End Sub

    ' Using a DependencyProperty as the backing store for Dimension.  This enables  
    ' animation, styling, binding, etc...     
    Public Shared ReadOnly DimensionProperty As DependencyProperty =
        DependencyProperty.RegisterAttached(
            "Dimension", GetType(Integer), GetType(ChangeDimensionBehavior),
            New UIPropertyMetadata(
                0,
                Function(sender As DependencyObject, e As DependencyPropertyChangedEventArgs)
                    ChangeDimensionBehavior.OnPropertyChangedCallBack(sender, e)
                End Function))

    ''' <summary>
    '''  Attached property Dimension's PropertyChangeCallback method.
    ''' In this method, get the MenuItem the attached property is set on and subscribe the Click
    ''' on the MenuItem.
    ''' </summary>
    Private Shared Sub OnPropertyChangedCallBack(ByVal sender As DependencyObject,
                                                 ByVal e As DependencyPropertyChangedEventArgs)
        Dim mnu As MenuItem = TryCast(sender, MenuItem)

        ' This ensures that the Click event of the MenuItem is subscribed only once.
        If (Not mnu Is Nothing) Then
            RemoveHandler mnu.Click, New RoutedEventHandler(
                AddressOf ChangeDimensionBehavior.mnu_Click)
            AddHandler mnu.Click, New RoutedEventHandler(
                AddressOf ChangeDimensionBehavior.mnu_Click)
        End If
    End Sub

    Private Shared Sub mnu_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' Get the value of the attached property set on the MenuItem.     
        Dim dimension As Integer =
            ChangeDimensionBehavior.GetDimension(TryCast(sender, DependencyObject))
        If (Application.Current.MainWindow.DataContext.GetType Is GetType(TicTacToeViewModel)) Then

            ' Change the Dimension property of the TicTacToeViewModel by invoking the 
            ' ChangeDimensionCommand on the TicTacToeViewModel.    
            TryCast(Application.Current.MainWindow.DataContext, 
                TicTacToeViewModel).ChangeDimensionCommand.Execute(dimension)
        End If
    End Sub

End Class




