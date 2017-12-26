'/************************************* Module Header **************************************\
'* Module Name:  FileEnumerator.vb
'* Project:      VBListFilesInDirectory
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBListFilesInDirectory project demonstrates how to implement an IEnumerable(Of String)
'* that utilizes the Win32 File Management functions to enable application to get files and
'* sub-directories in a specified directory one item a time.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 30/8/2009 1:00 PM Jie Wang Created
'\******************************************************************************************/


#Region "Using directives"
Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Runtime.InteropServices
#End Region


Public Class DirectoryEnumerator
    Implements IEnumerable(Of String)

#Region "The Enumerator"
    Public Structure Enumerator
        Implements IEnumerator(Of String)

#Region "Private members"

        Private hFindFile As SafeFindHandle
        Private m_current As String
        Private m_pattern As String
        Private m_mode As Mode

#End Region

#Region "Constructor"

        Friend Sub New(ByVal pattern As String, ByVal mode As Mode)
            Me.m_pattern = pattern
            Me.m_current = Nothing
            Me.hFindFile = Nothing
            Me.m_mode = mode
        End Sub

#End Region

#Region "IEnumerator(Of String) Members"

        Public ReadOnly Property Current() As String Implements IEnumerator(Of String).Current
            Get
                Return Me.m_current
            End Get
        End Property

#End Region

#Region "IDisposable Members"

        Public Sub Dispose() Implements IDisposable.Dispose
            If Me.hFindFile IsNot Nothing Then
                Me.hFindFile.Dispose()
            End If
        End Sub

#End Region

#Region "IEnumerator Members"

        Public ReadOnly Property CurrentObject() As Object Implements IEnumerator.Current
            Get
                Return Me.m_current
            End Get
        End Property

        Public Function MoveNext() As Boolean Implements System.Collections.IEnumerator.MoveNext
            If Me.hFindFile Is Nothing Then
                Return FindFirst()
            Else
                Return FindNext()
            End If
        End Function

        Public Sub Reset() Implements System.Collections.IEnumerator.Reset
            If Me.hFindFile IsNot Nothing Then
                Me.hFindFile.Close()
                Me.hFindFile = Nothing
            End If
        End Sub

#End Region

#Region "Supporting Methods"

        ''' <summary>
        ''' Find the first match.
        ''' </summary>
        ''' <returns></returns>
        Private Function FindFirst() As Boolean
            Dim fd As New WIN32_FIND_DATA

            Me.hFindFile = NativeMethods.FindFirstFile(Me.m_pattern, fd)

            If Me.hFindFile.IsInvalid Then
                ' Got an invalid find handle, get the error code
                Dim code As Integer = Marshal.GetLastWin32Error()

                If code = NativeMethods.ERROR_FILE_NOT_FOUND Then
                    ' file not found, just return false
                    Return False
                End If

                ' other errors, throw exception
                Throw New Win32Exception(code)
            End If

            If Not AttributesMatchMode(fd.dwFileAttributes) Then
                ' if the file does not meet the match mode,
                ' go find the next match.
                Return FindNext()
            End If

            Me.m_current = fd.cFileName
            Return True
        End Function

        Private Function FindNext() As Boolean
            Dim fd As New WIN32_FIND_DATA

            While NativeMethods.FindNextFile(Me.hFindFile, fd)
                If Not AttributesMatchMode(fd.dwFileAttributes) Then
                    ' if the file does not meet the match mode,
                    ' go find the next match.
                    Continue While
                End If

                ' found a match, return.
                Me.m_current = fd.cFileName
                Return True
            End While

            Dim code As Integer = Marshal.GetLastWin32Error()

            If code = NativeMethods.ERROR_NO_MORE_FILES Then
                ' no more files, return false.
                Return False
            End If

            ' other errors, throw exception.
            Throw New Win32Exception(code)
        End Function

        Private Function AttributesMatchMode(ByVal fileAttributes As Integer) As Boolean
            Dim isDir As Boolean = _
                ((fileAttributes And NativeMethods.FILE_ATTRIBUTE_DIRECTORY) = NativeMethods.FILE_ATTRIBUTE_DIRECTORY)

            Return (isDir AndAlso ((Me.m_mode And Mode.Directory) = Mode.Directory)) OrElse _
                    (Not isDir AndAlso (Me.m_mode And Mode.File) = Mode.File)
        End Function

#End Region

    End Structure
#End Region

#Region "FileEnumeratorMode"

    <Flags()> _
    Public Enum Mode
        ''' <summary>
        ''' Enumerate directories.
        ''' </summary>
        Directory = 1
        ''' <summary>
        ''' Enumerate files. 
        ''' </summary>
        File = 2
    End Enum

#End Region

#Region "Private members"

    Private m_pattern As String     ' Search pattern
    Private m_mode As Mode          ' Enum mode

#End Region

#Region "Constructor"

    Public Sub New()
        Me.New("*.*")
    End Sub

    Public Sub New(ByVal pattern As String)
        Me.New(pattern, Mode.Directory Or Mode.File)
    End Sub

    Public Sub New(ByVal pattern As String, ByVal mode As Mode)
        Me.m_pattern = pattern
        Me.m_mode = mode
    End Sub

#End Region

#Region "IEnumerable(Of String) Members"

    Public Function GetEnumerator() As System.Collections.Generic.IEnumerator(Of String) Implements IEnumerable(Of String).GetEnumerator
        Return New Enumerator(Me.m_pattern, Me.m_mode)
    End Function

#End Region

#Region "IEnumerable Memebers"

    Public Function GetEnumeratorObject() As System.Collections.IEnumerator Implements IEnumerable.GetEnumerator
        Return Me.GetEnumerator
    End Function

#End Region

End Class
