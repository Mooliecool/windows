/************************************* Module Header **************************************\
* Module Name:	UC_CumstomPropertyTab.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The CustomPropertyTab sample demonstrates how to add custom PropertyTab on to the 
* Properties Windows 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/25/2009 3:00 PM Zhi-Xin Ye Created
* * 
* 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.PropertyGridInternal;

namespace CSWinFormDesigner.CustomPropertyTab
{
    [PropertyTab(typeof(CustomTab), PropertyTabScope.Component)]
    public partial class UC_CumstomPropertyTab : UserControl
    {
        public UC_CumstomPropertyTab()
        {
            InitializeComponent();
        }

        private string testProp;

        // display on the custom tab
        [Browsable(false)]
        [CustomTabDisplayAttribute(true)]
        public string TestProp
        {
            get { return this.testProp; }
            set { this.testProp = value; }
        }

    }

    public class CustomTab : PropertiesTab
    {
        public override bool CanExtend(object extendee)
        {
            // return true if we extend this control
            return extendee is UC_CumstomPropertyTab;
        }

        public override PropertyDescriptorCollection GetProperties(
            ITypeDescriptorContext context, 
            object component, 
            Attribute[] attrs)
        {
            // Only returns the properties that are marked as Browserable(false) and CustomTabDisplay(true)
            return TypeDescriptor.GetProperties(
                component, 
                new Attribute[] 
                { 
                    new BrowsableAttribute(false), 
                    new CustomTabDisplayAttribute(true) 
                });
        }

        public override string TabName
        {
            get { return "Custom Tab"; }
        }

    }

    [AttributeUsage(AttributeTargets.Property)]
    public class CustomTabDisplayAttribute : Attribute
    {
        private bool display;

        public CustomTabDisplayAttribute(bool display)
        {
            this.display = display;
        }

        public bool Display
        {
            get { return this.display; }
            set { this.display = value; }
        }
    }
}
