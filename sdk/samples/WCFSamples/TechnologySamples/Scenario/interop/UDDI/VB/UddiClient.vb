' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.

Imports System
Imports System.IO
Imports System.Text
Imports System.ServiceModel
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    Public Class UddiClient

        Private inquireProxy As InquireClient
        Private publishProxy As PublishClient
        Private token As authToken

        Public Sub New()

            inquireProxy = New InquireClient("Inquire")
            publishProxy = New PublishClient("Publish")

        End Sub

        Public Sub Login(ByVal username As String, ByVal password As String)

            Dim getToken As New get_authToken()
            getToken.generic = "2.0"
            getToken.userID = username
            getToken.cred = password

            Dim request As New get_authTokenRequest()
            request.get_authToken = getToken
            token = publishProxy.get_authToken(request).authToken

        End Sub

        Public Function GetBusinessByName(ByVal businessName As String) As businessInfo()

            Dim find As New find_business()
            find.name = New name(0) {New name()}
            find.name(0).Value = businessName
            find.generic = "2.0"
            find.maxRows = 20

            Dim request As New find_businessRequest()
            request.find_business = find
            Dim bList As businessList = inquireProxy.find_business(request).businessList
            Return bList.businessInfos

        End Function

        Public Function PublishBusiness(ByVal businessName As String, ByVal description As String) As String

            Dim newBusiness As New save_business()
            newBusiness.authInfo = token.authInfo
            newBusiness.generic = "2.0"
            newBusiness.businessEntity = New businessEntity(0) {New businessEntity()}

            Dim myBusinessEntity As businessEntity = newBusiness.businessEntity(0)
            myBusinessEntity.name = New name(0) {New name()}
            myBusinessEntity.name(0).Value = businessName
            myBusinessEntity.businessKey = ""
            myBusinessEntity.description = New description(0) {New description()}
            myBusinessEntity.description(0).Value = description

            Dim request As New save_businessRequest()
            request.save_business = newBusiness
            Dim myBusinessDetail As businessDetail = publishProxy.save_business(request).businessDetail

            Return myBusinessDetail.businessEntity(0).businessKey

        End Function

        Public Function GetMyBusinesses() As businessInfo()

            Dim getInfo As New get_registeredInfo()

            getInfo.generic = "2.0"
            getInfo.authInfo = token.authInfo

            Dim request As New get_registeredInfoRequest()
            request.get_registeredInfo = getInfo
            Dim info As registeredInfo = publishProxy.get_registeredInfo(request).registeredInfo
            If info Is Nothing Then
                Return Nothing
            End If

            Return info.businessInfos

        End Function

        Public Function GetMyBusinessByName(ByVal businessName As String) As businessInfo

            Dim getInfo As New get_registeredInfo()

            getInfo.generic = "2.0"
            getInfo.authInfo = token.authInfo

            Dim request As New get_registeredInfoRequest()
            request.get_registeredInfo = getInfo
            Dim info As registeredInfo = publishProxy.get_registeredInfo(request).registeredInfo

            If info Is Nothing Then
                Return Nothing
            End If

            For Each bInfo As businessInfo In info.businessInfos

                If bInfo.name(0).Value = businessName Then

                    Return bInfo

                End If

            Next

            Return Nothing

        End Function

        Public Sub DeleteMyBusinesses()

            Dim infos() As businessInfo = Me.GetMyBusinesses()

            Dim business As New delete_business()
            business.businessKey = New String(infos.Length - 1) {}
            For i As Integer = 0 To infos.Length - 1

                business.businessKey(i) = infos(i).businessKey

            Next
            business.generic = "2.0"
            business.authInfo = token.authInfo

            Dim request As New delete_businessRequest()
            request.delete_business = business
            publishProxy.delete_business(request)

        End Sub

        Public Sub DeleteMyBusiness(ByVal businessKey As String)

            Dim business As New delete_business()
            business.businessKey = New String(0) {businessKey}
            business.generic = "2.0"
            business.authInfo = token.authInfo

            Dim request As New delete_businessRequest()
            request.delete_business = business
            publishProxy.delete_business(request)

        End Sub

        Public Sub PrintBusinessInfo(ByVal bInfo As businessInfo)

            If bInfo.name IsNot Nothing Then

                For Each n As name In bInfo.name
                    Console.WriteLine("Name:        " & n.Value)
                Next

            End If

            If bInfo.description IsNot Nothing Then

                For Each desc As description In bInfo.description
                    Console.WriteLine("Description: " & desc.Value)
                Next

            End If
            Console.WriteLine("Key:         " & bInfo.businessKey)

            If bInfo.serviceInfos IsNot Nothing Then
                PrintServiceInfos(bInfo.serviceInfos)
            End If

            Console.WriteLine()

        End Sub

        Public Sub PrintServiceInfos(ByVal serviceInfos As serviceInfo())

            Console.WriteLine("Total Services Offered = " & serviceInfos.Length.ToString())
            If serviceInfos.Length > 0 Then

                For Each sInfo As serviceInfo In serviceInfos

                    Console.WriteLine(vbTab & "Service Key: " & sInfo.serviceKey)

                    If sInfo.name IsNot Nothing Then

                        For Each n As name In sInfo.name
                            Console.WriteLine(vbTab & "Name: " & n.Value)
                        Next

                    End If

                    Console.WriteLine()

                Next

            End If

        End Sub

        Public Sub Close()

            DirectCast(inquireProxy, ICommunicationObject).Close()
            DirectCast(publishProxy, ICommunicationObject).Close()

        End Sub

    End Class

End Namespace
