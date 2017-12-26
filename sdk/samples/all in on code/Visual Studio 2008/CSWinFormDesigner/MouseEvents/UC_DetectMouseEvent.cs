/************************************* Module Header **************************************\
* Module Name:	UC_DetectMouseEvent.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The DetectMouseEvent sample demonstrates how to detect mouse events at design time.
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

namespace CSWinFormDesigner.MouseEvents
{
    [Designer(typeof(UC_DetectMouseEventDesigner))]
    public partial class UC_DetectMouseEvent : UserControl
    {
        public UC_DetectMouseEvent()
        {
            InitializeComponent();
        }
    }

    class UC_DetectMouseEventDesigner : ControlDesigner
    {
        protected override bool GetHitTest(Point point)
        {
            MessageBox.Show("Mouse at: " + point.ToString());

            return base.GetHitTest(point);
        }

        protected override void OnMouseEnter()
        {
            // Do something here when the mouse enters the control at design time;
            base.OnMouseEnter();
        }

        protected override void OnMouseLeave()
        {
            // Do something here when the mouse leaves the control at design time;
            base.OnMouseLeave();
        }
    }
}
