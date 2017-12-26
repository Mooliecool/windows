Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Text
Imports System.IO
Imports System.Xml
Imports System.Windows
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Markup
Imports System.Windows.Data
Imports System.Globalization

'@ <summary>
'@ A Converter class that return true if the EditingMode is Ink 
'@ </summary>
Public Class InkEditingModeConverter
    Implements IValueConverter

    '@ <summary>
    '@ Returns true if the EditingMode is Ink
    '@ </summary>
    Public Function Convert(ByVal o As Object, ByVal type As Type, ByVal parameter As Object, ByVal culture As CultureInfo) As Object Implements IValueConverter.Convert

        Dim editingMode As InkCanvasEditingMode = o

        If (editingMode = InkCanvasEditingMode.Ink) Then
            Return True
        End If
        Return False
    End Function

    '@ <summary>
    '@ Do not convert back.
    '@ </summary>
    Public Function ConvertBack(ByVal o As Object, ByVal type As Type, ByVal parameter As Object, ByVal culture As CultureInfo) As Object Implements IValueConverter.ConvertBack
        Return Nothing
    End Function
End Class

'@ <summary>
'@ A Converter class that return true if the EditingMode is Select 
'@ </summary>
Public Class SelectEditingModeConverter
    Implements IValueConverter

    '@ <summary>
    '@ Returns true if the EditingMode is Ink
    '@ </summary>
    Public Function Convert(ByVal o As Object, ByVal type As Type, ByVal parameter As Object, ByVal culture As CultureInfo) As Object Implements IValueConverter.Convert

        Dim editingMode As InkCanvasEditingMode = o

        If (editingMode = InkCanvasEditingMode.Select) Then
            Return True
        End If
        Return False
    End Function


    '@ <summary>
    '@ Do not convert back.
    '@ </summary>
    Public Function ConvertBack(ByVal o As Object, ByVal type As Type, ByVal parameter As Object, ByVal culture As CultureInfo) As Object Implements IValueConverter.ConvertBack
        Return Nothing
    End Function
End Class

