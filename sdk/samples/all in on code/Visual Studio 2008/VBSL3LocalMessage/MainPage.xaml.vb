'****************************** Module Header ******************************'
' Module Name:                 MainPage.xaml.vb
' Project:                     VBSL3LocalMessage
' Copyright (c) Microsoft Corporation.
' 
' Local message whiteboard's code behind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' ' 9/22/2009 4:10 PM Mog Liang Created
'***************************************************************************'

Imports System.Runtime.Serialization.Json
Imports System.Windows.Messaging
Imports System.IO

Partial Public Class MainPage
    Inherits UserControl

    Private clientid As Integer
    ' Preassign two names for LocalMessageReceiver.
    Private ReadOnly clientnames As String() = New String() {"{36BF6178-53A3-4d0b-8E0D-1E7BD9FBBE62}", "{E62BF131-B5EA-4f1d-B80D-DC083C7F0851}"}
    ' Preassign two colors for distinguishing two applications.
    Private ReadOnly mycolors As Color() = New Color() {Colors.Red, Colors.Green}

    Private _jsonserializer As DataContractJsonSerializer
    Private _localreceiver As LocalMessageReceiver
    Private _localsender As LocalMessageSender

    Public Sub New()
        Me.InitializeComponent()

        ' First try.
        ' Create receiver with one preset name.
        Me._jsonserializer = New DataContractJsonSerializer(Me.inkP.Strokes.GetType)
        Me._localreceiver = New LocalMessageReceiver(Me.clientnames(Me.clientid))
        AddHandler Me._localreceiver.MessageReceived, New EventHandler(Of MessageReceivedEventArgs)(AddressOf Me.localreceiver_MessageReceived)
        Try
            Me._localreceiver.Listen()
        Catch e As Exception
            ' Second try.
            ' Create receiver with another preset name.
            RemoveHandler Me._localreceiver.MessageReceived, New EventHandler(Of MessageReceivedEventArgs)(AddressOf Me.localreceiver_MessageReceived)
            Me.clientid = 1
            Me._localreceiver = New LocalMessageReceiver(Me.clientnames(Me.clientid))
            AddHandler Me._localreceiver.MessageReceived, New EventHandler(Of MessageReceivedEventArgs)(AddressOf Me.localreceiver_MessageReceived)
            Try
                Me._localreceiver.Listen()
            Catch ex As Exception
                ' Already has two apllication instance.
                ' Pop messagebox, disable drawing area.
                MessageBox.Show("Only allow 2 client.")
                Me.disablegrid.Visibility = Visibility.Visible
                Return
            End Try
        End Try

        ' Create sender targeting to another application's receiver
        Me._localsender = New LocalMessageSender(Me.clientnames(((Me.clientid + 1) Mod 2)))
        AddHandler Me._localsender.SendCompleted, New EventHandler(Of SendCompletedEventArgs)(AddressOf Me.localsender_SendCompleted)
        Me.drawbr.Background = New SolidColorBrush(Me.mycolors(Me.clientid))
    End Sub

    Private Sub localreceiver_MessageReceived(ByVal sender As Object, ByVal e As MessageReceivedEventArgs)
        ' Deserialize json string to stroke object.
        Dim stream As New MemoryStream
        Dim streamwriter As New StreamWriter(stream)
        streamwriter.Write(e.Message)
        streamwriter.Flush()
        Dim receivedstroke As Stroke = TryCast(Me._jsonserializer.ReadObject(stream), Stroke)
        stream.Close()

        ' Add received stroke to inkPresenter.
        Me.inkP.Strokes.Add(receivedstroke)
    End Sub

    Private Sub localsender_SendCompleted(ByVal sender As Object, ByVal e As SendCompletedEventArgs)
        ' If send message got error, pop messagebox.
        If (Not e.Error Is Nothing) Then
            MessageBox.Show("Cannot connect to another client")
        End If
    End Sub

    ' Attaching MouseLeftButtonDown/MouseMove/MouseLeftButtonUp event
    ' to implement drawing functionality.
    Private _newStroke As Stroke
    Private Sub InkPresenter_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me.inkP.CaptureMouse()
        Me._newStroke = New Stroke
        Me._newStroke.DrawingAttributes.Color = Me.mycolors(Me.clientid)
        Me._newStroke.StylusPoints.Add(e.StylusDevice.GetStylusPoints(Me.inkP))
        Me.inkP.Strokes.Add(Me._newStroke)
    End Sub

    ' Sending message when finishing one stroke.
    Private Sub InkPresenter_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me.inkP.ReleaseMouseCapture()

        ' Serialize stroke object to string.
        Dim stream As New MemoryStream
        Me._jsonserializer.WriteObject(stream, Me._newStroke)
        stream.Flush()
        stream.Position = 0
        Dim obstring As String = New StreamReader(stream).ReadToEnd
        stream.Close()

        ' Send Serialized stroke.
        If (Not Me._localsender Is Nothing) Then
            Try
                Me._localsender.SendAsync(obstring)
            Catch ex As Exception
                MessageBox.Show(ex.Message)
            End Try
        End If
        Me._newStroke = Nothing
    End Sub

    Private Sub InkPresenter_MouseMove(ByVal sender As Object, ByVal e As MouseEventArgs)
        If (Not Me._newStroke Is Nothing) Then
            Me._newStroke.StylusPoints.Add(e.StylusDevice.GetStylusPoints(Me.inkP))
        End If
    End Sub

End Class

