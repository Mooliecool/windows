'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System.Net.NetworkInformation
Imports System.Globalization

Public Class PingClientForm
    Dim WithEvents pingClient As New Ping()

    Private Sub pingClient_PingCompleted( _
        ByVal sender As Object, ByVal e As PingCompletedEventArgs) _
        Handles pingClient.PingCompleted
        ' Check to see if an error occurred.  If no error, then display the 
        ' address used and the ping time in milliseconds
        If e.Error Is Nothing Then
            If e.Cancelled Then
                pingDetailsTextBox.Text &= _
                    "Ping cancelled." & Environment.NewLine
            Else
                If e.Reply.Status = IPStatus.Success Then
                    pingDetailsTextBox.Text &= _
                        "  " & e.Reply.Address.ToString() & " " & _
                        e.Reply.RoundtripTime.ToString( _
                        NumberFormatInfo.CurrentInfo) & "ms " & _
                        Environment.NewLine
                Else
                    pingDetailsTextBox.Text &= _
                        "  " & GetStatusString(e.Reply.Status) & _
                        Environment.NewLine
                End If
            End If
        Else
            ' Otherwise display the error
            pingDetailsTextBox.Text &= _
                "  Ping error." & Environment.NewLine
            MessageBox.Show( _
                "An error occurred while sending this ping. " & _
                e.Error.InnerException.Message.ToString())
        End If
        sendPingButton.Enabled = True
    End Sub

    Private Function GetStatusString(ByVal status As IPStatus) As String
        Select Case status
            Case IPStatus.Success
                Return "Success."
            Case IPStatus.DestinationHostUnreachable
                Return "Destination host unreachable."
            Case IPStatus.DestinationNetworkUnreachable
                Return "Destination network unreachable."
            Case IPStatus.DestinationPortUnreachable
                Return "Destination port unreachable."
            Case IPStatus.DestinationProtocolUnreachable
                Return "Destination protocol unreachable."
            Case IPStatus.PacketTooBig
                Return "Packet too big."
            Case IPStatus.TtlExpired
                Return "TTL expired."
            Case IPStatus.ParameterProblem
                Return "Parameter problem."
            Case IPStatus.SourceQuench
                Return "Source quench."
            Case IPStatus.TimedOut
                Return "Timed out."
            Case Else
                Return "Ping failed."
        End Select
    End Function


    Private Sub sendPingButton_Click( _
        ByVal sender As System.Object, ByVal e As System.EventArgs) Handles sendPingButton.Click

        ' Select all the text in the address box
        addressTextBox.SelectAll()

        If addressTextBox.Text.Length <> 0 Then
            ' Disable the Send button.
            sendPingButton.Enabled = False

            pingDetailsTextBox.Text &= _
                "Pinging " & addressTextBox.Text & _
                " . . ." & Environment.NewLine

            ' Send ping request
            pingClient.SendAsync(addressTextBox.Text, Nothing)
        Else
            MessageBox.Show("Please enter an IP address or host name.")
        End If

    End Sub

    Private Sub cancelPingButton_Click( _
        ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cancelPingButton.Click

        ' Cancel any pending pings.
        pingClient.SendAsyncCancel()
    End Sub

End Class
