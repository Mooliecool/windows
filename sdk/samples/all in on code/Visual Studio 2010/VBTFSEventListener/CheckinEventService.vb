'*************************** Module Header ******************************'
' Module Name:  CheckinEventService.vb
' Project:	    VBTFSEventListener
' Copyright (c) Microsoft Corporation.
' 
' This class implement the IEventService interface, and it is used to subscribe 
' a TFS Check-in Event. If a user checked in a changeset which met the filters
' of the subscription, TFS will call the Notify method of this WCF service with 
' the parameters. The Notify method display the message if the Changeset has
' policy failures. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO
Imports System.Text
Imports System.Xml.Serialization
Imports Microsoft.TeamFoundation.VersionControl.Common


Public Class CheckinEventService
    Implements IEventService

    ''' <summary>
    ''' This method will send out an email if the Changeset has policy failures. 
    ''' </summary>
    ''' <param name="eventXml">
    ''' This parameter is a Xml format string which is serialized from a CheckinEvent
    ''' object.
    ''' </param>
    ''' <param name="tfsIdentityXml">
    ''' The TFS url, like 
    ''' "<TeamFoundationServer url="http://server:8080/tfs/VSTS/Services/v3.0/LocationService.asmx" />"
    ''' </param>
    Public Sub Notify(ByVal eventXml As String, ByVal tfsIdentityXml As String) _
        Implements IEventService.Notify

        ' Initialize a CheckinEvent XmlSerializer.
        Dim serializer As New XmlSerializer(GetType(CheckinEvent))

        ' Deserialize the eventXml to a CheckinEvent object.
        Dim cievent As CheckinEvent = Nothing
        Using reader = New StringReader(eventXml)
            cievent = TryCast(serializer.Deserialize(reader), CheckinEvent)
        End Using


        If cievent Is Nothing Then
            Return
        End If

        ' Check whether the Changeset has policy failures. 
        If cievent.PolicyFailures.Count > 0 Then
            Dim message As New StringBuilder()

            message.AppendFormat("ChangeSet{0} Check In Policy Policy Failed. " _
                                 & vbLf, cievent.ContentTitle)

            message.AppendFormat("Committer : {0}" & vbLf, cievent.Committer)
            message.AppendFormat("Override Comment : {0}" & vbLf, cievent.PolicyOverrideComment)
            message.Append("Check in policy failures:" & vbLf)
            For Each p As NameValuePair In cievent.PolicyFailures
                message.AppendFormat(vbTab & "{0} : {1}", p.Name, p.Value)

            Next p
            Console.WriteLine(message.ToString())
        End If
    End Sub
End Class

