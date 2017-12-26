'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.ServiceModel

Namespace Microsoft.Samples.ByteStreamEncoder

    Friend Class Server
        Public Shared Sub Main()
            Dim baseAddress As Uri = New Uri("http://localhost:8000/byteStream")
            Console.WriteLine("Testing image upload and download using ByteStreamHttpBinding")
            Using host As New ServiceHost(New FileService(), baseAddress)
                host.Open()
                Console.WriteLine("Service started at: " & host.ChannelDispatchers(0).Listener.Uri.AbsoluteUri)
                Console.WriteLine("Press <ENTER> to terminate service")
                Console.ReadLine()
            End Using
        End Sub
    End Class
End Namespace
