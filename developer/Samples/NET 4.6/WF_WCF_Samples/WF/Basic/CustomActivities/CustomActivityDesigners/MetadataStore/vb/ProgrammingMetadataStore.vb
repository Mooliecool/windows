'-------------------------------------------------------------------
' Copyright (c) Microsoft Corporation. All rights reserved
'-------------------------------------------------------------------


Imports System
Imports System.Activities.Presentation.Metadata
Imports System.ComponentModel

Namespace Microsoft.Samples.MetadataProgramming

    Module ProgrammingMetadataStore

        Sub Main()
            ' First, we will print out all attributes that are on string by default
            Dim _attributeCollection As AttributeCollection = TypeDescriptor.GetAttributes(GetType(String))
            Console.WriteLine("--------- default attributes")
            OutputAttributes(_attributeCollection)

            ' Use the AttributeTableBuilder to add a new Attribute to string
            Dim builder = New AttributeTableBuilder()
            builder.AddCustomAttributes(GetType(String), New DesignerCategoryAttribute("Custom category"))
            MetadataStore.AddAttributeTable(builder.CreateTable())

            Console.WriteLine("--------- including custom attributes")
            _attributeCollection = TypeDescriptor.GetAttributes(GetType(String))
            OutputAttributes(_attributeCollection)
            Console.WriteLine("--------- registering callback")

            builder = New AttributeTableBuilder()
            ' Use the AttributeCallback to delay registering metadata until it is requested.
            builder.AddCallback(GetType(String), 
				Sub(acb)
					Console.WriteLine("*** In AttributeCallback, adding a new attribute")
					acb.AddCustomAttributes(New DesignTimeVisibleAttribute(False))
				End Sub  )
            MetadataStore.AddAttributeTable(builder.CreateTable())

            Console.WriteLine("--------- including custom attributes added via callback")
            _attributeCollection = TypeDescriptor.GetAttributes(GetType(String))
            OutputAttributes(_attributeCollection)
            Console.WriteLine("Press Enter to Exit")
            Console.ReadLine()


        End Sub

        
        Sub OutputAttributes(ByVal attributeCollection As AttributeCollection)
            For Each attribute In attributeCollection
                Console.WriteLine("Attribute: {0}", attribute.ToString())
            Next attribute

        End Sub

    End Module

End Namespace
