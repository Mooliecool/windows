Imports System     
Imports System.Windows     
Imports System.Windows.Controls     

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub ClickClearEventHistory(ByVal sender As Object, ByVal args As RoutedEventArgs)
            tbEventEvents.Clear()
        End Sub
        Public Sub ehDragEnter(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbDragEnter.IsChecked.Value = True Then
                LogEvent(EventFireStrings.DragEnter, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehDragLeave(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbDragLeave.IsChecked.Value = True Then
                LogEvent(EventFireStrings.DragLeave, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehDragOver(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbDragOver.IsChecked.Value = True Then
                LogEvent(EventFireStrings.DragOver, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehDrop(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbDrop.IsChecked.Value = True Then
                LogEvent(EventFireStrings.Drop, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehPreviewDragEnter(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbPreviewDragEnter.IsChecked.Value = True Then
                LogEvent(EventFireStrings.PreviewDragEnter, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehPreviewDragLeave(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbPreviewDragLeave.IsChecked.Value = True Then
                LogEvent(EventFireStrings.PreviewDragLeave, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehPreviewDragOver(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbPreviewDragOver.IsChecked.Value = True Then
                LogEvent(EventFireStrings.PreviewDragOver, args)
            Else
                Return
            End If
        End Sub
        Public Sub ehPreviewDrop(ByVal sender As Object, ByVal args As DragEventArgs)
            If cbPreviewDrop.IsChecked.Value = True Then
                LogEvent(EventFireStrings.PreviewDrop, args)
            Else
                Return
            End If
        End Sub
        Public Sub LogEvent(ByVal eventMessage As String, ByVal args As DragEventArgs)
            tbEventEvents.AppendText(eventMessage)
            If cbVerbose.IsChecked.Value = True Then
                tbEventEvents.AppendText("     Source Object: " + args.Source.ToString() + Environment.NewLine)
                tbEventEvents.AppendText("     Drag Effects: " + args.Effects.ToString() + Environment.NewLine)
                tbEventEvents.AppendText("     Key States: " + args.KeyStates.ToString() + Environment.NewLine)
                tbEventEvents.AppendText("     Available Data Formats:" + Environment.NewLine)
                For Each format As String In args.Data.GetFormats()
                    tbEventEvents.AppendText("          " + format + Environment.NewLine)
                Next
            End If
            tbEventEvents.ScrollToEnd()
        End Sub

        Structure EventFireStrings
            Public Shared DragEnter As String = " The DragEnter event just occurred." + Environment.NewLine
            Public Shared DragLeave As String = " The DragLeave event just occurred." + Environment.NewLine
            Public Shared DragOver As String = " The DragOver event just occurred." + Environment.NewLine
            Public Shared Drop As String = " The Drop event just occurred." + Environment.NewLine
            Public Shared PreviewDragEnter As String = " The PreviewDragEnter event just occurred." + Environment.NewLine
            Public Shared PreviewDragLeave As String = " The PreviewDragLeave event just occurred." + Environment.NewLine
            Public Shared PreviewDragOver As String = " The PreviewDragOver event just occurred." + Environment.NewLine
            Public Shared PreviewDrop As String = " The PreviewDrop event just occurred." + Environment.NewLine
            Dim myRequiredContent As Canvas() 'Structure requirement in Visual Basic: You must declare at least one nonshared variable or nonshared, noncustom event in a structure. You cannot have only constants, properties, and procedures, even if some of them are nonshared.
        End Structure

    End Class
End Namespace
