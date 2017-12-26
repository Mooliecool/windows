' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Security.Permissions
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.Dispatcher
Imports Microsoft.VisualBasic

<Assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution:=True)> 
Namespace Microsoft.ServiceModel.Samples

    Class Service

        Public Shared Sub Main(ByVal args As String())

            ' Create ServiceHost. 
            Dim sh As New ServiceHost(GetType(SecurityTokenService))
            sh.Open()

            Try

                For Each cd As ChannelDispatcher In sh.ChannelDispatchers

                    For Each ed As EndpointDispatcher In cd.Endpoints

                        Console.WriteLine("STS listening at {0}", ed.EndpointAddress.Uri)

                    Next

                Next

                Console.WriteLine(vbNewLine & "Press enter to exit" & vbNewLine)
                Console.ReadLine()

            Finally

                sh.Close()

            End Try

        End Sub

    End Class

End Namespace