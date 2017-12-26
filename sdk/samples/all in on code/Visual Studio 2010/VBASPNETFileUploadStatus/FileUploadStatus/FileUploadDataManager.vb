'***************************** Module Header ******************************\
'* Module Name:    UploadFileCollection.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* This is a class which used to filter the file data in the request entity and 
'* store them into the UploadFileCollection. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************

Imports System.Text
Imports System.Text.RegularExpressions

Friend Class FileUploadDataManager
    Private isFinished As Boolean = True
    Private draft As Byte() = Nothing
    Private isFile As Boolean = False

    Private ReadOnly _backSpace As Byte() = Nothing
    Private ReadOnly _doubleBackSpace As Byte() = Nothing
    Private ReadOnly _boundary As Byte() = Nothing
    Private ReadOnly _endTag As Byte() = Nothing

    Public Sub New(ByVal boundary__1 As String)
        _boundary = ASCIIEncoding.ASCII.GetBytes("--" + boundary__1)
        _backSpace = ASCIIEncoding.ASCII.GetBytes(vbCr & vbLf)
        _doubleBackSpace = ASCIIEncoding.ASCII.GetBytes(vbCr & vbLf & vbCr & vbLf)
        _endTag = ASCIIEncoding.ASCII.GetBytes("--" + boundary__1 + "--" & vbCr & vbLf)
        FilterResult = New UploadFileCollection()
        isFinished = True
        draft = Nothing
        isFile = False

    End Sub

    Public Property FilterResult() As UploadFileCollection
        Get
            Return _filterResult
        End Get
        Private Set(ByVal value As UploadFileCollection)
            _filterResult = value
        End Set
    End Property
    Private _filterResult As UploadFileCollection

    Public Sub AppendData(ByVal data As Byte())
        FilterFileDataFromBodyEntity(data)
        If data Is Nothing Then
            Return
        End If
    End Sub

    Private Sub AppendToLastEntity(ByVal data As Byte())
        If Not isFinished AndAlso isFile Then
            Dim lastFile As UploadFile = FilterResult(FilterResult.Count - 1)
            lastFile.AppendData(data)
        End If
    End Sub

    ' Filter the request data to get the file binary data.
    ' Here is an algorithm logic to filter the data. There
    ' will be a lot of words to say before we talk about 
    ' the algorithm(Maybe a small book). So I just open the
    ' source here but omit the explanation in details.
    Private Sub FilterFileDataFromBodyEntity(ByVal data As Byte())
        If data Is Nothing Then
            Return
        End If

        If draft IsNot Nothing Then
            Dim temp As Byte() =
                BinaryHelper.Combine(draft,
                                     BinaryHelper.Copy(data, 0, _boundary.Length))
            Dim entity_st As Integer =
                BinaryHelper.SequenceIndexOf(temp,
                                             _boundary,
                                             0)
            Dim entity_ed As Integer =
                BinaryHelper.SequenceIndexOf(temp,
                                             _boundary,
                                             entity_st + _boundary.Length + 2)

            If isFile AndAlso Not isFinished Then
                If entity_st = 0 Then
                    Dim header_st As Integer = entity_st + _boundary.Length + 2
                    Dim header_ed As Integer =
                        BinaryHelper.SequenceIndexOf(temp,
                                                     _doubleBackSpace, header_st)

                    Dim body_st As Integer = header_ed + 4
                    If entity_ed = -1 Then
                        AppendToLastEntity(BinaryHelper.SubData(draft, body_st))
                        draft = Nothing
                    Else
                        AppendToLastEntity(BinaryHelper.SubData(draft,
                                                                body_st,
                                                                entity_ed - body_st - 2))
                        isFinished = True
                        isFile = False
                        draft = BinaryHelper.SubData(draft, entity_ed)
                    End If
                Else
                    AppendToLastEntity(draft)
                    draft = Nothing
                End If
            End If

            ' When need append new data, combine the two
            ' binary array into one.
            data = BinaryHelper.Combine(draft, data)
            draft = Nothing
        End If
        While True
            ' find the boundary
            Dim entity_st As Integer = BinaryHelper.SequenceIndexOf(data, _boundary, 0)

            ' if the current loaded data contain the boundary
            If entity_st > -1 Then
                If isFile AndAlso Not isFinished Then
                    AppendToLastEntity(BinaryHelper.SubData(data, 0, entity_st - 2))
                    data = BinaryHelper.SubData(data, entity_st)
                    isFile = False
                    isFinished = True
                    Continue While
                End If

                Dim entity_ed As Integer = BinaryHelper.SequenceIndexOf(data,
                                                                        _boundary,
                                                                        entity_st + _boundary.Length + 2)
                Dim header_st As Integer = entity_st + _boundary.Length + 2
                Dim header_ed As Integer = BinaryHelper.SequenceIndexOf(data,
                                                                        _doubleBackSpace,
                                                                        header_st)
                Dim body_st As Integer = header_ed + 4

                If body_st < 4 Then
                    ' If the header in the entity is not complete, then
                    ' set the draft as the data, and dump out the function
                    ' to ask for more data.
                    draft = data
                    Return
                Else
                    ' If the header in the entity is complete 
                    If Not isFile AndAlso isFinished Then

                        ' Encoding the data in the header of the entity
                        Dim headerInEntity As String = ASCIIEncoding.UTF8.GetString(
                            BinaryHelper.SubData(data, header_st, header_ed - header_st))

                        ' If it is a file entity, the header contain the keyword:"filename".
                        If headerInEntity.IndexOf("filename") > -1 Then
                            ' Use Regular Expression to get the meta key values from 
                            ' the header of the entity.
                            Dim detailsReg As New Regex("Content-Disposition: form-data; name=""([^""]*)"";" +
                                                        " filename=""([^""]*)""Content-Type: ([^""]*)")
                            Dim regMatch As Match =
                                detailsReg.Match(headerInEntity.Replace(vbCr & vbLf, ""))

                            Dim controlName As String = regMatch.Groups(1).Value
                            Dim clientPath As String = regMatch.Groups(2).Value
                            Dim contentType As String = regMatch.Groups(3).Value
                            If String.IsNullOrEmpty(clientPath) Then
                                isFile = False
                            Else
                                isFile = True
                                ' Create a new instance for the file entity
                                Dim up As New UploadFile(clientPath, contentType)
                                FilterResult.Add(up)
                                isFinished = False
                            End If
                        Else
                            isFile = False
                        End If

                    End If
                End If
                If entity_ed > -1 Then
                    ' If we can find another boundary after the first boundary,
                    ' that means the entity block is ended there.
                    ' Only if it is a file entity we need to get the data
                    ' in the body of the entity
                    If isFile Then
                        AppendToLastEntity(BinaryHelper.SubData(data,
                                                                body_st,
                                                                entity_ed - body_st - 2))
                        isFinished = True
                        isFile = False
                    End If
                    ' Remove the current processed entity data
                    ' and loop for the next one.
                    data = BinaryHelper.SubData(data, entity_ed)
                    If BinaryHelper.Equals(data, _endTag) Then
                        data = Nothing
                        draft = Nothing
                        Return
                    End If
                    Continue While
                Else
                    ' If we can't find the end mark, we have to 
                    ' move the data to the draft and 
                    ' ask for new data to append.
                    draft = data
                    Return
                End If
            Else
                ' If we can't find any mark of the boundary,
                ' we have to move the data to the draft and 
                ' ask for new data to append.
                draft = data
                Return

            End If
        End While


    End Sub
End Class


