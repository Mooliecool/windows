'****************************** Module Header ******************************\
' Module Name:  ShutdownBehavior.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The ShutdownBehavior class is an attached behavior that is used to shut down the application.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/  

Public Class ShutdownBehavior
    Inherits DependencyObject

    Public Shared Function GetForceShutdown(ByVal obj As DependencyObject) As Nullable(Of Boolean)
        Return DirectCast(obj.GetValue(ShutdownBehavior.ForceShutdownProperty), Nullable(Of Boolean))
    End Function

    Public Shared Sub SetForceShutdown(ByVal obj As DependencyObject, ByVal value As Nullable(Of Boolean))
        obj.SetValue(ShutdownBehavior.ForceShutdownProperty, value)
    End Sub

    ' Using a DependencyProperty as the backing store for ForceShutdown.  This enables 
    ' animation, styling, binding, etc...        
    Public Shared ReadOnly ForceShutdownProperty As DependencyProperty =
        DependencyProperty.RegisterAttached(
            "ForceShutdown",
            GetType(Nullable(Of Boolean)),
            GetType(ShutdownBehavior),
            New UIPropertyMetadata(
                Nothing,
                Function(sender As DependencyObject, e As DependencyPropertyChangedEventArgs)
                    ShutdownBehavior.OnPropertyChangedCallBack(sender, e)
                End Function))

    ''' <summary>
    ''' MenuItem's Click event handler
    ''' </summary>
    Private Shared Sub mnu_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' If the ForceShutdown attached proerty value set on the MenuItem is true, exit the 
        ' application immediately.    
        If ShutdownBehavior.GetForceShutdown(TryCast(sender, DependencyObject)).Value Then
            Application.Current.Shutdown()

            ' If the ForceShutdown attached proerty value set on the MenuItem is false, show a 
            ' messagebox before exiting the application.      
        ElseIf (Not ShutdownBehavior.GetForceShutdown(TryCast(sender, DependencyObject)).Value AndAlso
                (MessageBox.Show("Are you sure to exit the application?",
                                 "Exit", MessageBoxButton.OKCancel) = MessageBoxResult.OK)) Then
            Application.Current.Shutdown()
        End If
    End Sub

    ''' <summary>
    ''' The ForceShutdown property's PropertyChangedCallback method.
    ''' In this method, get the MenuItem the attached property is set on and subscribe to the
    ''' Click event of the MenuItem.
    ''' </summary>
    Private Shared Sub OnPropertyChangedCallBack(ByVal sender As DependencyObject,
                                                 ByVal e As DependencyPropertyChangedEventArgs)
        Dim mnu As MenuItem = TryCast(sender, MenuItem)

        ' This ensures the Click event on the MenuItem is subscribed to only once. 
        If (Not mnu Is Nothing) Then
            RemoveHandler mnu.Click, New RoutedEventHandler(AddressOf ShutdownBehavior.mnu_Click)
            AddHandler mnu.Click, New RoutedEventHandler(AddressOf ShutdownBehavior.mnu_Click)
        End If
    End Sub

End Class

