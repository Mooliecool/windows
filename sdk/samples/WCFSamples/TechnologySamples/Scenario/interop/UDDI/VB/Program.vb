' Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.


Imports System
Imports System.IO
Imports System.Configuration
Imports System.ServiceModel
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    Class Program

        Shared username As String
        Shared password As String

        Public Shared Sub Main(ByVal args As String())

            GetCredentials()

            Dim sampleBusinessName As String = "My WCF Sample Business"
            Dim sampleBusinessDescription As String = "My Sample Business published using WCF"

            Dim client As New UddiClient()

            Try

                Console.WriteLine("Logging in as user " & username & "...")
                client.Login(username, password)

                Console.WriteLine("Publish " & sampleBusinessName & "...")
                'Check if "My Business" was already published by me and delete it...
                Dim bInfo As businessInfo = client.GetMyBusinessByName(sampleBusinessName)
                If bInfo IsNot Nothing Then
                    client.DeleteMyBusiness(bInfo.businessKey)
                End If
                'Publish My Business
                Dim businessKey As String = ""

                businessKey = client.PublishBusiness(sampleBusinessName, sampleBusinessDescription)

                Dim bInfos() As businessInfo = Nothing
                Console.WriteLine("Query for all businesses published by me....")
                bInfos = client.GetMyBusinesses()
                If bInfos IsNot Nothing Then

                    Console.WriteLine("The following businesses are published by me:")
                    For Each info As businessInfo In bInfos

                        client.PrintBusinessInfo(info)

                    Next

                End If

                Console.WriteLine("Query businesses by name '" & sampleBusinessName & "' in the registry...")
                bInfos = client.GetBusinessByName(sampleBusinessName)
                If bInfos IsNot Nothing Then

                    Console.WriteLine("The following businesses are published under the name '" & sampleBusinessName & "':" & vbNewLine)
                    For Each info As businessInfo In bInfos

                        client.PrintBusinessInfo(info)

                    Next

                End If

                If businessKey <> String.Empty Then

                    Console.WriteLine("Deleting business """ & sampleBusinessName & """ created earlier")
                    client.DeleteMyBusiness(businessKey)

                End If

            Catch fault As FaultException

                Dim faultReason As String = fault.Reason.Translations(0).Text
                Console.WriteLine("Received Fault: " & faultReason)
                If faultReason.Contains("Publisher limit") = True Then

                    Console.WriteLine("You have a limited number of businesses you can publish." & vbNewLine & "Use GetMyBusinesses() to query your published businesses " & vbNewLine & " and DeleteMyBusiness(businessKey) to delete them.")

                End If
                Console.WriteLine()

            End Try

            client.Close()


            Console.WriteLine("Press <ENTER> to exit.")
            Console.ReadLine()

        End Sub

        Private Shared Sub GetCredentials()

            username = ConfigurationManager.AppSettings("username")
            password = ""

            Console.WriteLine("Please enter username and password to log in to UDDI registry")
            Console.Write("Username[" & username & "]:")
            Dim input As String = Console.ReadLine()
            If input <> String.Empty Then

                username = input

            End If
            Console.Write("Password:")
            Dim key As ConsoleKeyInfo = Console.ReadKey(True)
            While key.Key <> ConsoleKey.Enter

                Console.Write("*")
                password &= key.KeyChar
                key = Console.ReadKey(True)

            End While
            Console.WriteLine()

        End Sub

    End Class

End Namespace