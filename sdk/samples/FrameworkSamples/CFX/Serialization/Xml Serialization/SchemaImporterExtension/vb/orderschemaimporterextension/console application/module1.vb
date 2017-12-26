Imports System
Imports System.Collections
Imports System.IO
Imports System.Collections.Generic
Imports System.Text
Imports System.Xml.Serialization
Imports System.Xml
Imports System.Xml.Schema
Imports System.Web.Services.Description
Imports System.Web.Services.Discovery
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports Microsoft.VisualBasic


Namespace Microsoft.Samples
    Class Module1

        Public Shared Sub Main()

            Dim wsdlLocation As New Uri(Directory.GetCurrentDirectory() & "\..\..\OrderService.wsdl")

            Console.WriteLine("Beginning schema import.")
            Console.WriteLine()

            Dim client As New DiscoveryClientProtocol()
            client.AllowAutoRedirect = True
            Dim doc As DiscoveryDocument = client.DiscoverAny(wsdlLocation.ToString())
            client.ResolveAll()

            Dim references As New WebReferenceCollection()

            'add 1st web reference document
            Dim wr As New WebReference(client.Documents, New CodeNamespace("Microsoft.Samples"))
            references.Add(wr)

            'add other web reference documents if needed
            Dim codeProvider As Object = New VBCodeProvider()
            Dim Ccu As New CodeCompileUnit()
            Ccu.Namespaces.Add(wr.ProxyCode)
            Try
                ServiceDescriptionImporter.GenerateWebReferences(references, _
                        codeProvider, Ccu, New WebReferenceOptions())
            Catch ex As Exception
                Console.WriteLine("An error has occured on import:" + vbCrLf + ex.ToString())
            End Try
   
            Dim writer As New StreamWriter("OrderProxy.vb", False, New System.Text.UTF8Encoding(True))

            codeProvider.GenerateCodeFromCompileUnit(Ccu, writer, Nothing)

            writer.Close()
            Console.WriteLine()
            Console.WriteLine("Schema import completed.")

            Console.WriteLine("Finished. Press any key to continue...")
            Console.ReadLine()

            System.Diagnostics.Process.Start("notepad", "OrderProxy.vb")

        End Sub 'Main 
    End Class 'Program 
End Namespace 'Microsoft.Samples 