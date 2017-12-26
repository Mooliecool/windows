'***************************** Module Header ******************************\
'* Module Name:    UploadStatus.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* We use this class to store upload progress status.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'* History:
'* 6/22/2010 3:20 PM Jerry Weng Created
'\****************************************************************************


Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Web
Imports System.Web.Caching
Imports System.Threading
Imports System.Runtime.Serialization
Imports System.Web.Script.Serialization
Imports System.IO


#Region ""
' we define a EventHandler delegate to hold the events int the UploadStatus.
Public Delegate Sub UploadStatusEventHandler(ByVal sender As Object, ByVal e As UploadStatusEventArgs)

Public Class UploadStatusEventArgs
    Inherits EventArgs
    <ScriptIgnore()> _
    Public Property context() As HttpContext
        Get
            Return m_context
        End Get
        Protected Set(ByVal value As HttpContext)
            m_context = value
        End Set
    End Property
    Private m_context As HttpContext
    Public Sub New(ByVal ctx As HttpContext)
        context = ctx
    End Sub
End Class
#End Region

<Serializable()> _
Public Class UploadStatus

    Private Enum DataUnit
        [Byte] = 1
        KB = 1024
        MB = 1048576
        GB = 1073741824
    End Enum
    Private Enum TimeUnit
        Seconds = 1
        Minutes = 60
        Hours = 3600
        Day = 86400
    End Enum

    'return the unit of the Uploaded data.
    Private ReadOnly Property LoadedUnit() As DataUnit
        Get
            Return GetDataUnit(LoadedLength)
        End Get
    End Property
    'return the unit of the whole content data.
    Private ReadOnly Property ContentUnit() As DataUnit
        Get
            Return GetDataUnit(ContentLength)
        End Get
    End Property

    'the context of the request
    Private Context As HttpContext





    ''' <summary>
    ''' If the user abort the upload, then it will return true.
    ''' </summary>
    Public Property Aborted() As Boolean
        Get
            Return _aborted
        End Get
        Private Set(ByVal value As Boolean)
            _aborted = value
        End Set
    End Property
    Private _aborted As Boolean

    ''' <summary>
    ''' get the content length of the file
    ''' </summary>
    Public Property ContentLength() As Long
        Get
            Return _contentLength
        End Get
        Private Set(ByVal value As Long)
            _contentLength = value
        End Set
    End Property
    Private _contentLength As Long
    ''' <summary>
    ''' get the content length of the file which is formatted with unit
    ''' </summary>
    Public ReadOnly Property ContentLengthString() As String
        Get
            Dim rslWithUnit As Decimal = CDec(ContentLength) / CInt(ContentUnit)
            Return rslWithUnit.ToString("0.00") & " " & ContentUnit.ToString()
        End Get
    End Property
    ''' <summary>
    ''' get the uploaded content length of the file
    ''' </summary>
    Public Property LoadedLength() As Long
        Get
            Return _loadedLength
        End Get
        Private Set(ByVal value As Long)
            _loadedLength = value
        End Set
    End Property
    Private _loadedLength As Long
    ''' <summary>
    ''' get the uploaded content length of the file 
    ''' which is formatted with unit
    ''' </summary>
    Public ReadOnly Property LoadedLengthString() As String
        Get
            Dim rslWithUnit As Decimal = CDec(LoadedLength) / CInt(LoadedUnit)
            Return rslWithUnit.ToString("0.00") & " " & LoadedUnit.ToString()
        End Get
    End Property
    ''' <summary>
    ''' get the time when the uploading started.
    ''' </summary>
    Public Property StartTime() As DateTime
        Get
            Return _startTime
        End Get
        Private Set(ByVal value As DateTime)
            _startTime = value
        End Set
    End Property
    Private _startTime As DateTime
    ''' <summary>
    ''' get the time when the uploading finished or aborted.
    ''' </summary>
    Public Property EndTime() As DateTime
        Get
            Return _endTime
        End Get
        Private Set(ByVal value As DateTime)
            _endTime = value
        End Set
    End Property
    Private _endTime As DateTime
    ''' <summary>
    ''' get whether the uploading finished.
    ''' </summary>
    Public Property IsFinished() As Boolean
        Get
            Return _isFinished
        End Get
        Private Set(ByVal value As Boolean)
            _isFinished = value
        End Set
    End Property
    Private _isFinished As Boolean
    ''' <summary>
    ''' get the persentage of the uploaded content
    ''' </summary>
    Public ReadOnly Property LoadedPersentage() As Integer
        Get
            Dim percent As Integer = Convert.ToInt32(Math.Ceiling(CDec(LoadedLength) / CDec(ContentLength) * 100))
            Return percent
        End Get
    End Property
    ''' <summary>
    ''' get the spent time of the uploading
    ''' the unit is seconds
    ''' </summary>
    Public ReadOnly Property SpendTimeSeconds() As Double
        Get
            Dim calcTime As DateTime = DateTime.Now
            If IsFinished OrElse Aborted Then
                calcTime = EndTime
            End If
            Dim spendtime As Double = Math.Ceiling(calcTime.Subtract(StartTime).TotalSeconds)
            If spendtime = 0 AndAlso IsFinished Then
                spendtime = 1
            End If
            Return spendtime
        End Get
    End Property
    ''' <summary>
    ''' get the spent time of the uploading which is formatted with unit
    ''' </summary>
    Public ReadOnly Property SpendTimeString() As String
        Get
            Dim spent As Double = SpendTimeSeconds
            Dim unit As TimeUnit = GetTimeUnit(spent)
            Dim unitTime As Double = spent / CInt(GetTimeUnit(spent))
            Return unitTime.ToString("0.0") & " " & unit.ToString()
        End Get
    End Property

    ''' <summary>
    ''' get the upload speed
    ''' the unit is bytes/second
    ''' </summary>
    Public ReadOnly Property UploadSpeed() As Double
        Get
            Dim spendtime As Double = SpendTimeSeconds
            Dim speed As Double = CDbl(LoadedLength) / spendtime
            Return speed
        End Get
    End Property
    ''' <summary>
    ''' get the upload speed which is formatted with unit.
    ''' </summary>
    Public ReadOnly Property UploadSpeedString() As String
        Get
            Dim spendtime As Double = SpendTimeSeconds
            Dim unit As DataUnit = GetDataUnit(CLng(Math.Truncate(Math.Ceiling(CDbl(LoadedLength) / spendtime))))
            Dim speed As Double = UploadSpeed / CInt(unit)
            Return speed.ToString("0.0") & " " & unit.ToString() & "/seconds"
        End Get
    End Property
    ''' <summary>
    ''' get the left time
    ''' the unit is seconds
    ''' </summary>
    Public ReadOnly Property LeftTimeSeconds() As Double
        Get
            Dim remain As Double = Math.Floor((ContentLength - LoadedLength) / UploadSpeed)
            Return remain
        End Get
    End Property
    ''' <summary>
    ''' get the left time which is formatted with unit
    ''' </summary>
    Public ReadOnly Property LeftTimeString() As String
        Get
            Dim remain As Double = LeftTimeSeconds
            Dim unit As TimeUnit = GetTimeUnit(remain)
            Dim newRemain As Double = remain / CInt(unit)
            Return newRemain.ToString("0.0") & " " & unit.ToString()
        End Get
    End Property




    Public Event OnDataChanged As UploadStatusEventHandler
    Public Event OnFinish As UploadStatusEventHandler



    Public Sub New()
    End Sub
    Public Sub New(ByVal ctx As HttpContext, ByVal length As Long)
        Aborted = False
        IsFinished = False
        StartTime = DateTime.Now
        Context = ctx
        ContentLength = length
    End Sub



    Private Function GetTimeUnit(ByVal seconds As Double) As TimeUnit
        If seconds > CInt(TimeUnit.Day) Then
            Return TimeUnit.Day
        End If
        If seconds > CInt(TimeUnit.Hours) Then
            Return TimeUnit.Hours
        End If
        If seconds > CInt(TimeUnit.Minutes) Then
            Return TimeUnit.Minutes
        End If
        Return TimeUnit.Seconds

    End Function
    Private Function GetDataUnit(ByVal length As Long) As DataUnit
        If length > Math.Pow(2.0, 30) Then
            Return DataUnit.GB
        End If
        If length > Math.Pow(2.0, 20) Then
            Return DataUnit.MB
        End If
        If length > Math.Pow(2.0, 10) Then
            Return DataUnit.KB
        End If
        Return DataUnit.[Byte]
    End Function

    Private Sub changeFinish()
        RaiseEvent OnFinish(Me, New UploadStatusEventArgs(Context))
    End Sub
    Private Sub changeData()
        If Aborted Then
            Return
        End If
        RaiseEvent OnDataChanged(Me, New UploadStatusEventArgs(Context))
        If LoadedLength = ContentLength Then
            EndTime = DateTime.Now
            IsFinished = True
            changeFinish()
        End If
    End Sub
    Public Sub Abort()
        Aborted = True
        EndTime = DateTime.Now
    End Sub
    Public Sub UpdateLoadedLength(ByVal length As Long)
        If Not IsFinished AndAlso Not Aborted Then
            LoadedLength += length
            changeData()
        End If
    End Sub




End Class

