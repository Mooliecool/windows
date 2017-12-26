/************************************* Module Header **************************************\
* Module Name:	UC_CustomEditor.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The CustomEditor sample demonstrates how to use a custom editor for a specific property 
* at design time. 
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

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Design;
using System.Windows.Forms.Design;

namespace CSWinFormDesigner.CustomEditors
{
    public partial class UC_CustomEditor : UserControl
    {
        public UC_CustomEditor()
        {
            InitializeComponent();
        }

        private SubClass cls = new SubClass();

        [TypeConverter(typeof(ExpandableObjectConverter))]
        [Editor(typeof(MyEditor),typeof(UITypeEditor))]
        public SubClass Cls
        {
            get { return this.cls; }
            set { this.cls = value; }
        }
    }

    public class SubClass
    {
        private string name;
        private DateTime date = DateTime.Now;

        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }
        public DateTime Date
        {
            get { return this.date; }
            set { this.date = value; }
        }
    }

    #region UITypeEditor

    // This class demonstrates the use of a custom UITypeEditor. 
    // It allows the UC_CustomEditor control's Cls property
    // to be changed at design time using a customized UI element
    // that is invoked by the Properties window. The UI is provided
    // by the EditorForm class.
    public class MyEditor : UITypeEditor
    {

        private IWindowsFormsEditorService editorService = null;

        public override UITypeEditorEditStyle GetEditStyle(
        System.ComponentModel.ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(
            ITypeDescriptorContext context,
            IServiceProvider provider,
            object value)
        {
            if (provider != null)
            {
                editorService =
                    provider.GetService(typeof(IWindowsFormsEditorService))
                    as IWindowsFormsEditorService;
            }

            if (editorService != null)
            {
                EditorForm editorForm = new EditorForm();
                editorForm.SubCls = (SubClass)value;

                if (editorService.ShowDialog(editorForm) == DialogResult.OK)
                {
                    value = editorForm.SubCls;
                }
            }
            return value;
        }

        public override bool GetPaintValueSupported(
            ITypeDescriptorContext context)
        {
            return false;
            //return true;
        }


        public override void PaintValue(PaintValueEventArgs e)
        {
            //TODO:
        }
    }


    #endregion
}
