/************************************* Module Header **************************************\
* Module Name:	UC_EnableDesignTimeFuncForChildCtrl.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* The EnableDesignTimeFuncOnChildCtrl sample demonstrates how to enable design time 
* functionality on for a child control.  
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
using System.Windows.Forms.Design;

namespace CSWinFormDesigner
{
    // Specify this class use the UC_EnableDesignTimeFuncForChildCtrlDesigner 
    [Designer(typeof(UC_EnableDesignTimeFuncForChildCtrlDesigner))]
    public partial class UC_EnableDesignTimeFuncForChildCtrl : UserControl
    {
        public UC_EnableDesignTimeFuncForChildCtrl()
        {
            InitializeComponent();
        }

        // The code for this user control declares a public property of type Panel with a 
        // DesignerSerializationVisibility attribute set to DesignerSerializationVisibility.Content, 
        // indicating that the properties of the object should be serialized.

        // The public, not hidden properties of the object that are set at design time 
        // will be persisted in the initialization code for the class object. Content 
        // persistence will not work for structs without a custom TypeConverter.  
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        public Panel Panel1
        {
            get { return this.panel1; }
        }
    }

    class UC_EnableDesignTimeFuncForChildCtrlDesigner : ControlDesigner
    {
        public override void Initialize(IComponent component)
        {
            base.Initialize(component);

            UC_EnableDesignTimeFuncForChildCtrl uc = component as UC_EnableDesignTimeFuncForChildCtrl;

            //Call the EnableDesignMode method to enables design time functionality for a child control. 
            this.EnableDesignMode(uc.Panel1, "Panel1");
        }

    }
}
