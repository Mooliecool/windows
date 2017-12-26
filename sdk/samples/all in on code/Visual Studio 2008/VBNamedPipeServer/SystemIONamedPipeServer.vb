'******************************* Module Header *********************************'
'* Module Name:  SystemIONamedPipeServer.vb
'* Project:      VBNamedPipeServer
'* Copyright (c) Microsoft Corporation.
'* 
'* The System.IO.Pipes namespace contains types that provide a means for 
'* interprocess communication through anonymous and/or named pipes. 
'* http://msdn.microsoft.com/en-us/library/system.io.pipes.aspx
'* These classes make the programming of named pipe in .NET much easier and safer 
'* than P/Invoking native APIs directly. However, the System.IO.Pipes namespace
'* is not available before .NET Framework 3.5. So, if you are programming against 
'* .NET Framework 2.0, you have to P/Invoke native APIs to use named pipe.
'* 
'* The sample code in SystemIONamedPipeServer.Run() uses the 
'* Systen.IO.Pipes.NamedPipeServerStream class to create a pipe that is named 
'* "\\.\pipe\SamplePipe" to perform message-based communication. The pipe supports 
'* duplex connections, so both client and server can read from and write 
'* to the pipe. The security attributes of the pipe are customized to allow 
'* Authenticated Users read and write access to a pipe, and to allow the 
'* Administrators group full access to the pipe. When the pipe is connected by a 
'* client, the server attempts to read the client's message from the pipe, and 
'* write a response.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\******************************************************************************'

#Region "Imports directives"

Imports System.Text
Imports System.IO
Imports System.IO.Pipes
Imports System.Security.AccessControl

#End Region


Class SystemIONamedPipeServer

    ''' <summary>
    ''' Use the pipe classes in the System.IO.Pipes namespace to create the named
    ''' pipe. This solution is recommended.
    ''' </summary>
    Public Shared Sub Run()
        Dim pipeServer As NamedPipeServerStream = Nothing

        Try
            ' Prepare the security attributes (the pipeSecurity parameter in the 
            ' constructor of NamedPipeServerStream) for the pipe. This is 
            ' optional. If pipeSecurity of NamedPipeServerStream is null, the
            ' named pipe gets a default security descriptor.and the handle cannot 
            ' be inherited. The ACLs in the default security descriptor of a pipe
            ' grant full control to the LocalSystem account, (elevated) 
            ' administrators, and the creator owner. They also give only read 
            ' access to members of the Everyone group and the anonymous account. 
            ' However, if you want to customize the security permission of the 
            ' pipe, (e.g. to allow Authenticated Users to read from and write to 
            ' the pipe), you need to create a PipeSecurity object.
            Dim pipeSecurity As PipeSecurity = Nothing
            pipeSecurity = CreateSystemIOPipeSecurity()

            ' Create the named pipe.
            pipeServer = New NamedPipeServerStream( _
                PipeName, _
                PipeDirection.InOut, _
                NamedPipeServerStream.MaxAllowedServerInstances, _
                PipeTransmissionMode.Message, _
                PipeOptions.None, _
                BufferSize, _
                BufferSize, _
                pipeSecurity, _
                HandleInheritability.None)

            Console.WriteLine("The named pipe ({0}) is created.", FullPipeName)

            ' Wait for the client to connect.
            Console.WriteLine("Waiting for the client's connection...")
            pipeServer.WaitForConnection()
            Console.WriteLine("Client is connected.")

            ' 
            ' Receive a request from client.
            ' 
            ' Note: The named pipe was created to support message-based 
            ' communication. This allows a reading process to read varying-length
            ' messages precisely as sent by the writing process. In this mode you 
            ' should not use StreamWriter to write the pipe, or use StreamReader 
            ' to read the pipe. You can read more about the difference from the 
            ' article: http://go.microsoft.com/?linkid=9721786.
            ' 

            Dim message As String
            Do
                Dim bRequest(BufferSize - 1) As Byte
                Dim cbRequest As Integer = bRequest.Length, cbRead As Integer

                cbRead = pipeServer.Read(bRequest, 0, cbRequest)

                ' Unicode-encode the received byte array and trim all the '\0' 
                ' characters at the end.
                message = Encoding.Unicode.GetString(bRequest).TrimEnd( _
                    ControlChars.NullChar)
                Console.WriteLine("Receive {0} bytes from client: ""{1}""", _
                    cbRead, message)
            Loop While Not pipeServer.IsMessageComplete

            ' 
            ' Send a response from server to client.
            ' 

            message = ResponseMessage
            Dim bResponse As Byte() = Encoding.Unicode.GetBytes(message)
            Dim cbResponse As Integer = bResponse.Length

            pipeServer.Write(bResponse, 0, cbResponse)

            Console.WriteLine("Send {0} bytes to client: ""{1}""", _
                cbResponse, message.TrimEnd(ControlChars.NullChar))

            ' Flush the pipe to allow the client to read the pipe's contents 
            ' before disconnecting. Then disconnect the client's connection.
            pipeServer.WaitForPipeDrain()
            pipeServer.Disconnect()

        Catch ex As Exception
            Console.WriteLine("The server throws the error: {0}", ex.Message)
        Finally
            If (pipeServer IsNot Nothing) Then
                pipeServer.Close()
                pipeServer = Nothing
            End If
        End Try
    End Sub


    ''' <summary>
    ''' The CreateSystemIOPipeSecurity function creates a new PipeSecurity 
    ''' object to allow Authenticated Users read and write access to a pipe, 
    ''' and to allow the Administrators group full access to the pipe.
    ''' </summary>
    ''' <returns>
    ''' A PipeSecurity object that allows Authenticated Users read and write 
    ''' access to a pipe, and allows the Administrators group full access to 
    ''' the pipe.
    ''' </returns>
    Private Shared Function CreateSystemIOPipeSecurity() As PipeSecurity
        Dim pipeSecurity As New PipeSecurity()

        ' Allow Everyone read and write access to the pipe.
        pipeSecurity.SetAccessRule(New PipeAccessRule("Authenticated Users", _
            PipeAccessRights.ReadWrite, AccessControlType.Allow))

        ' Allow the Administrators group full access to the pipe.
        pipeSecurity.SetAccessRule(New PipeAccessRule("Administrators", _
            PipeAccessRights.FullControl, AccessControlType.Allow))

        Return pipeSecurity
    End Function

End Class