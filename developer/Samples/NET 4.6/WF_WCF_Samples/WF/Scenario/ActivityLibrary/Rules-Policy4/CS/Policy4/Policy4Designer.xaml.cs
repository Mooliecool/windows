//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Windows;
using System.Windows.Forms;
using System.Workflow.Activities.Rules;
using System.Workflow.Activities.Rules.Design;

namespace Microsoft.Samples.Activities.Rules
{
    // Interaction logic for PolicyDesigner.xaml
    public partial class Policy4Designer
    {
        public Policy4Designer()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {            
            // verifiy that TargetObject property has been configured
            object input = ModelItem.Properties["Input"].ComputedValue;
            if (input == null)
            {
                System.Windows.MessageBox.Show("Input argument needs to be configured before adding the rules");
                return;
            }

            // verify that target object is correctly configured
            InArgument arg = input as InArgument;
            if (arg == null)
            {
                System.Windows.MessageBox.Show("Invalid value for Input argument");
                return;
            }

            // open the ruleset editor
            Type inputType = arg.ArgumentType;
            RuleSet ruleSet = ModelItem.Properties["RuleSet"].ComputedValue as RuleSet;
            if (ruleSet == null)
                ruleSet = new RuleSet();

            // popup the dialog for editing the rules            
            RuleSetDialog ruleSetDialog = new RuleSetDialog(inputType, null, ruleSet);
            DialogResult result = ruleSetDialog.ShowDialog();

            // update the model item
            if (result == DialogResult.OK) //check if they cancelled
            {                
                ModelItem.Properties["RuleSet"].SetValue(ruleSetDialog.RuleSet);
            }   
        }
    }
}
