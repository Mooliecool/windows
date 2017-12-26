/************************************* Module Header **************************************\
* Module Name:  MyCustomTypeDescriptor.cs
* Project:      CSWinFormBindToNestedProperties
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to bind a DataGridView column to a nested property 
* in the data source.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/


using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace BindingNestedProperties
{
    public class MyCustomTypeDescriptor : CustomTypeDescriptor
    {
        public MyCustomTypeDescriptor(ICustomTypeDescriptor parent)

            : base(parent)
        {

        }

        public override PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            // get original PropertyDescriptorCollection from the type
            PropertyDescriptorCollection originalPds = base.GetProperties(attributes);
            List<PropertyDescriptor> subPds = new List<PropertyDescriptor>();

            for (int i = 0; i < originalPds.Count; i++)
            {
                // get the chid properties of each original PropertyDescriptor
                PropertyDescriptorCollection tempPds = originalPds[i].GetChildProperties();

                // if the child properties' count is greater than 0, create a new 
                // PropertyDescriptor for
                // each of the child property and add it to the subPds
                if (tempPds.Count > 0)
                {
                    for (int j = 0; j < tempPds.Count; j++)
                    {
                        subPds.Add(new SubPropertyDescriptor(originalPds[i], tempPds[j], 
                            originalPds[i].Name + "_" + tempPds[j].Name));
                    }
                }
            }

            PropertyDescriptor[] array = new PropertyDescriptor[originalPds.Count + subPds.Count];

            // copy the original PropertyDescriptorCollection to the array
            originalPds.CopyTo(array,0);

            // copy all the PropertyDescriptor representing the child properties to the array
            subPds.CopyTo(array, originalPds.Count);

            // create a new PropertyDescriptorCollection based on the array
            PropertyDescriptorCollection newPds = new PropertyDescriptorCollection(array);

            // return the new PropertyDescriptorCollection containing both the PropertyDescriptor 
            // for original properties and 
            // child properties
            return newPds;
        }

        public override PropertyDescriptorCollection GetProperties()
        {
            // get original PropertyDescriptorCollection from the type
            PropertyDescriptorCollection originalPds = base.GetProperties();
            List<PropertyDescriptor> subPds = new List<PropertyDescriptor>();

            for (int i = 0; i < originalPds.Count; i++)
            {
                // get the chid properties of each original PropertyDescriptor
                PropertyDescriptorCollection tempPds = originalPds[i].GetChildProperties();

                // if the child properties' count is greater than 0, create a new PropertyDescriptor for
                // each of the child property and add it to the subPds
                if (tempPds.Count > 0)
                {
                    for (int j = 0; j < tempPds.Count; j++)
                    {
                        subPds.Add(new SubPropertyDescriptor(originalPds[i], tempPds[j], 
                            originalPds[i].Name + "_" + tempPds[j].Name));
                    }
                }
            }

            PropertyDescriptor[] array = new PropertyDescriptor[originalPds.Count + subPds.Count];

            // copy the original PropertyDescriptorCollection to the array
            originalPds.CopyTo(array, 0);

            // copy all the PropertyDescriptor representing the child properties to the array
            subPds.CopyTo(array, originalPds.Count);

            // create a new PropertyDescriptorCollection based on the array
            PropertyDescriptorCollection newPds = new PropertyDescriptorCollection(array);

            // return the new PropertyDescriptorCollection containing both the PropertyDescriptor 
            // for original properties and 
            // child properties
            return newPds;

        }

    }

    public class MyTypeDescriptionProvider : TypeDescriptionProvider
    {
        private ICustomTypeDescriptor td;

        public MyTypeDescriptionProvider()
            : this(TypeDescriptor.GetProvider(typeof(Person)))
        {

        }

        public MyTypeDescriptionProvider(TypeDescriptionProvider parent)
            : base(parent)
        {

        }

        public override ICustomTypeDescriptor GetTypeDescriptor(Type objectType, object instance)
        {
            if (td == null)
            {
                td = base.GetTypeDescriptor(objectType, instance);
                td = new MyCustomTypeDescriptor(td);
            }
            return td;
        }

    }



}
