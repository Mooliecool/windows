//-------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved
//-------------------------------------------------------------------


using System;
using System.Activities.Presentation.Metadata;
using System.ComponentModel;

namespace Microsoft.Samples.ProgrammingMetadataStore
{
    class ProgrammingMetadataStore
    {
        static void Main(string[] args)
        {
            // First, we will print out all attributes that are on string by default
            AttributeCollection attributeCollection = TypeDescriptor.GetAttributes(typeof(string));
            Console.WriteLine("--------- default attributes");
            OutputAttributes(attributeCollection);

            // Use the AttributeTableBuilder to add a new Attribute to string
            AttributeTableBuilder builder = new AttributeTableBuilder();
            builder.AddCustomAttributes(typeof(string), new DesignerCategoryAttribute("Custom category"));
            MetadataStore.AddAttributeTable(builder.CreateTable());

            Console.WriteLine("--------- including custom attributes");
            attributeCollection = TypeDescriptor.GetAttributes(typeof(string));
            OutputAttributes(attributeCollection);
            Console.WriteLine("--------- registering callback");

            // Use the AttributeCallback to delay registering metadata until it is requested.
            builder = new AttributeTableBuilder();
            builder.AddCallback(typeof(string),
                new AttributeCallback(acb =>
                    {
                        Console.WriteLine("*** In AttributeCallback, adding a new attribute");
                        acb.AddCustomAttributes(new DesignTimeVisibleAttribute(false));
                    }
                )
            );
            MetadataStore.AddAttributeTable(builder.CreateTable());

            Console.WriteLine("--------- including custom attributes added via callback");
            attributeCollection = TypeDescriptor.GetAttributes(typeof(string));
            OutputAttributes(attributeCollection);
            Console.WriteLine("Press Enter to Exit");
            Console.ReadLine();
        }

        private static void OutputAttributes(AttributeCollection attributeCollection)
        {
            foreach (Attribute attribute in attributeCollection)
            {
                Console.WriteLine("Attribute: {0}", attribute.ToString());
            }
        }
    }
}
