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
    // Interaction logic for WizardPage3.xaml
    

    public partial class WizardPage3 : WizardPageFunction
    {
        public override void Initialize(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }

            this.InitializeComponent();

            //Set the DataContext of the page
            this.MainGrid.DataContext = currentObject;

            //build the actions list (divide by item groups)
            this.ProjectActions.Items.Clear();

            foreach (TreeViewItem bAction in currentObject.CreateActionList())
            {
                this.ProjectActions.Items.Add(bAction);
            }
        }

        public override void Done(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }

            //Done
            base.Done(currentObject);
        }
    }
}