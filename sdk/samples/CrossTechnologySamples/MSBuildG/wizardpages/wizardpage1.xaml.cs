// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for WizardPage1.xaml
    

    public partial class WizardPage1 : WizardPageFunction
    {
        public override void Initialize(BuildProject currentObject)
        {
            this.InitializeComponent();
        }

        public override void Done(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }
            //Check for an empty field

            if (this.ProjectName.Text.Trim().Length == 0)
            {
                ShowErrorBubble(new WizardErrorMessage(ProjectName, "Project Name Required", "You must enter a project name to continue"));
                return;
            }

            //Set data in the project
            currentObject.ProjectName = this.ProjectName.Text;
            currentObject.ProjectNotes = this.ProjectNotes.Text;
            
            //Done
            base.Done(currentObject);
        }

    }
}