/************************************* Module Header **************************************\
* Module Name:	UC_PropertyDisplayOrder.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The PropertyDisplayOrder sample demonstrates how to change the default display order on 
* Properties windows for properties, and how to expand a nested property on the Properties 
* window. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/25/2009 3:00 PM Zhi-Xin Ye Created
* * 
* 
\******************************************************************************************/

#region namespaces
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion

namespace CSWinFormDesigner.PropertyDisplayOrder
{
    public partial class UC_PropertyDisplayOrder : UserControl
    {
        public UC_PropertyDisplayOrder()
        {
            InitializeComponent();
        }

        private SubClass cls = new SubClass();

        [TypeConverter(typeof(PropertyOrderConverter))]
        public SubClass Cls
        {
            get { return this.cls; }
            set { this.cls = value; }
        }
    }

    public class SubClass
    {
        private string prop1;
        private int prop2;
        private float prop3;

        public string Prop1
        {
            get { return prop1; }
            set { prop1 = value; }
        }
        public int Prop2
        {
            get { return prop2; }
            set { prop2 = value; }
        }
        public float Prop3
        {
            get { return prop3; }
            set { prop3 = value; }
        }
    }

    public class PropertyOrderConverter : ExpandableObjectConverter
    {
        /*
         * Change properties display order in Properties Window
         */
        #region
        public override bool GetPropertiesSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
        {
            System.ComponentModel.PropertyDescriptorCollection propertyDescriptorCollection;

            string[] propNames;

            propertyDescriptorCollection =
                TypeDescriptor.GetProperties(typeof(SubClass), attributes);

            propNames = (string[])new System.String[3];

            //Specify the order for the properties
            propNames[0] = @"Prop2";
            propNames[1] = @"Prop3";
            propNames[2] = @"Prop1";

            return propertyDescriptorCollection.Sort(propNames);
        }
        #endregion
    }
}
