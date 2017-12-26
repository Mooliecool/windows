Imports System
Imports System.Windows
Imports System.Windows.Controls

    Public Class TaskListDataTemplateSelector
        Inherits DataTemplateSelector

        ' Methods
        Public Overrides Function SelectTemplate(ByVal item As Object, ByVal container As DependencyObject) As DataTemplate
            If ((Not item Is Nothing) AndAlso TypeOf item Is Task) Then
                Dim task1 As Task = TryCast(item,Task)
                Dim window1 As Window = Application.Current.MainWindow
                If (task1.Priority = 1) Then
                    Return TryCast(window1.FindResource("importantTaskTemplate"),DataTemplate)
                End If
                Return TryCast(window1.FindResource("myTaskTemplate"),DataTemplate)
            End If
            Return Nothing
        End Function

    End Class


