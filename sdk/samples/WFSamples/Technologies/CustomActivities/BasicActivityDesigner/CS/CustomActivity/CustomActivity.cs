//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Workflow.Activities;
using System.Workflow.ComponentModel.Design;

namespace Microsoft.Samples.Workflow.BasicActivityDesigner
{
    [Designer(typeof(CustomActivityDesigner), typeof(IDesigner))]
    [ToolboxItem(typeof(ActivityToolboxItem))]
    public partial class CustomActivity: SequenceActivity
    {
    }

    [ActivityDesignerThemeAttribute(typeof(CustomActivityDesignerTheme))]
    public class CustomActivityDesigner : ActivityDesigner
    {
        protected override ReadOnlyCollection<DesignerAction> DesignerActions
        {
            get
            {
                List<DesignerAction> DesignerActionList = new List<DesignerAction>();

                //This is for the    configuration error on an activity
                DesignerActionList.Add(new DesignerAction(this, 1, "Insert Text Here!"));
                return DesignerActionList.AsReadOnly();
            }
        }
        protected override void OnExecuteDesignerAction(DesignerAction designerAction)
        {
            if (designerAction.ActionId == 1)
                MessageBox.Show(designerAction.Text);
        }

        // callback for a context Menu item when a user right clicks on the
        // activity
        private void CustomContextMenuEvent(object sender, EventArgs e)
        {
            MessageBox.Show("This is the action from my Context Menu");
        }

        protected override ActivityDesignerVerbCollection Verbs
        {
            get
            {
                ActivityDesignerVerbCollection newVerbs = new ActivityDesignerVerbCollection();
                newVerbs.AddRange(base.Verbs);
                newVerbs.Add(new ActivityDesignerVerb(this, DesignerVerbGroup.General, "Custom Context Menu", new EventHandler(CustomContextMenuEvent)));
                return newVerbs;
            }
        }
    }

    internal sealed class CustomActivityDesignerTheme : ActivityDesignerTheme
    {
        public CustomActivityDesignerTheme(WorkflowTheme theme)
            : base(theme)
        {
            this.BorderColor = Color.Red;
            this.BorderStyle = DashStyle.Dot;
            this.BackColorStart = Color.LightYellow;
            this.BackColorEnd = Color.Yellow;
            this.BackgroundStyle = LinearGradientMode.Horizontal;
        }
    }
}
