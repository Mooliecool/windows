'***************************** Module Header ******************************\
'* Module Name:    BinaryHelper.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* This is a class which help to filter the binary data to get the file data. 
'* All these static methods will help to process with the binary data. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************

Friend Class BinaryHelper

    ' Copy partial data from one source binary array.
    Public Shared Function Copy(ByVal source As Byte(),
                                ByVal index As Integer,
                                ByVal length As Integer) As Byte()
        Dim result As Byte() = New Byte(length - 1) {}
        Array.ConstrainedCopy(source, index, result, 0, length)
        Return result
    End Function

    ' Combine two binary arrays into one.
    Public Shared Function Combine(ByVal a As Byte(), ByVal b As Byte()) As Byte()
        If a Is Nothing AndAlso b Is Nothing Then
            Return Nothing
        ElseIf a Is Nothing OrElse b Is Nothing Then
            Return If(a, b)
        End If
        Dim newData As Byte() = New Byte(a.Length + (b.Length - 1)) {}
        Array.ConstrainedCopy(a, 0, newData, 0, a.Length)
        Array.ConstrainedCopy(b, 0, newData, a.Length, b.Length)
        Return newData

    End Function

    ' Check whether two binary arrays 
    ' have the same data in same index.
    Public Overloads Shared Function Equals(ByVal source As Byte(),
                                            ByVal compare As Byte()) As Boolean
        If source.Length <> compare.Length Then
            Return False
        End If
        If SequenceIndexOf(source, compare, 0) <> 0 Then
            Return False
        End If
        Return True
    End Function

    ' Get partial data in the binary array.
    Public Shared Function SubData(ByVal source As Byte(),
                                   ByVal startIndex As Integer) As Byte()
        Dim result As Byte() = New Byte(source.Length - startIndex - 1) {}
        Array.ConstrainedCopy(source, startIndex, result, 0, result.Length)
        Return result
    End Function

    ' Get partial data in the binary array.
    Public Shared Function SubData(ByVal source As Byte(),
                                   ByVal startIndex As Integer,
                                   ByVal length As Integer) As Byte()
        Dim result As Byte() = New Byte(length - 1) {}
        Array.ConstrainedCopy(source, startIndex, result, 0, length)
        Return result
    End Function

    ' Get the index in the source array from which all the data and positions
    ' are same as another array.
    Public Shared Function SequenceIndexOf(ByVal source As Byte(),
                                           ByVal compare As Byte()) As Integer
        Return SequenceIndexOf(source, compare, 0)
    End Function
    Public Shared Function SequenceIndexOf(ByVal source As Byte(),
                                           ByVal compare As Byte(),
                                           ByVal startIndex As Integer) As Integer
        Dim result As Integer = -1
        Dim sourceLen As Integer = source.Length
        Dim compareLen As Integer = compare.Length
        If startIndex < 0 Then
            Return -1
        End If

        For i As Integer = startIndex To sourceLen - compareLen
            If source(i) = compare(0) AndAlso _
                source(i + compareLen - 1) = compare(compareLen - 1) Then
                Dim t As Integer = 0
                For j As Integer = 0 To compare.Length - 1
                    t += 1
                    If compare(j) <> source(i + j) Then
                        Exit For
                    End If
                Next
                If t = compareLen Then
                    result = i
                    Exit For
                End If
            End If
        Next
        Return result
    End Function

End Class
