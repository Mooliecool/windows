Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.Specialized
Imports System.Text
Imports System.Xml.Serialization
Imports System.Xml.Serialization.Advanced
Imports System.Xml.Schema
Imports System.Xml
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports Microsoft.Samples.Orders


    Public Class OrderSchemaImporterExtension
    Inherits SchemaImporterExtension

    Shared Sub New()
    End Sub 'New


    Public Overloads Overrides Function ImportSchemaType(ByVal type As XmlSchemaType, _
            ByVal context As XmlSchemaObject, ByVal schemas As XmlSchemas, _
            ByVal importer As XmlSchemaImporter, ByVal compileUnit As CodeCompileUnit, _
            ByVal codeNamespace As CodeNamespace, ByVal options As CodeGenerationOptions, _
            ByVal codeGenerator As CodeDomProvider) As String

        Dim defaultColor As ConsoleColor = Console.BackgroundColor
        Console.BackgroundColor = ConsoleColor.Blue
        Console.WriteLine((vbTab & "OrderExtension called for type: " & type.ToString()))
        Console.BackgroundColor = defaultColor

        Return Nothing
    End Function 'ImportSchemaType



    Public Overloads Overrides Function ImportSchemaType(ByVal name As String, _
            ByVal ns As String, ByVal context As XmlSchemaObject, _
            ByVal schemas As XmlSchemas, ByVal importer As XmlSchemaImporter, _
            ByVal compileUnit As CodeCompileUnit, ByVal codeNamespace As CodeNamespace, _
            ByVal options As CodeGenerationOptions, ByVal codeGenerator As CodeDomProvider) As String
        Dim defaultColor As ConsoleColor = Console.BackgroundColor
        Console.BackgroundColor = ConsoleColor.Blue
        Console.WriteLine((ControlChars.Tab & "OrderExtension called for name = " & name & " and namespace = " & ns))
        Console.BackgroundColor = defaultColor

        If name.Equals("Order") And ns.Equals("http://orders/") Then
            Console.BackgroundColor = ConsoleColor.Blue
            Console.WriteLine()
            Console.WriteLine(vbTab & vbTab & "OrderExtension matched for Order!")
            Console.WriteLine()
            Console.BackgroundColor = defaultColor

            compileUnit.ReferencedAssemblies.Add("Order.dll")
            codeNamespace.Imports.Add(New CodeNamespaceImport("Microsoft.Samples.Orders"))
            Return "Order"
        End If


        Return Nothing
    End Function 'ImportSchemaType

End Class 'OrderSchemaImporterExtension
