/************************************* Module Header **************************************\
* Module Name:	UC_AdornmentPainting.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The AdornmentPainting sample demonstrates how to paint additional adornments on top of 
* the control.  
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
using System.Drawing.Drawing2D;

namespace CSWinFormDesigner.Adornment
{
    [Designer(typeof(UC_AdornmentPaintingDesigner))]
    public partial class UC_AdornmentPainting : UserControl
    {
        public UC_AdornmentPainting()
        {
            InitializeComponent();
        }
    }

    class UC_AdornmentPaintingDesigner : ControlDesigner
    {
        protected override void OnPaintAdornments(PaintEventArgs pe)
        {
            UC_AdornmentPainting uc = (UC_AdornmentPainting)Component;
            
            Rectangle clientRectangle = uc.ClientRectangle;

            //TODO: Code here to paint adornments on the top the control 

            using (Pen pen = new Pen(Color.Red))
            {
                pen.DashStyle = DashStyle.Dot;
                clientRectangle.Width -= 1;
                clientRectangle.Height -= 1;
                pe.Graphics.DrawRectangle(pen, clientRectangle);
            }

            base.OnPaintAdornments(pe);
        }

    }
}
