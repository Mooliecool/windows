//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities.Presentation;
using System.Activities.Presentation.Converters;
using System.Activities.Presentation.Model;
using System.Activities.Presentation.PropertyEditing;
using System.Windows;

namespace Microsoft.Samples.Activities.PowerShell
{

    class ArgumentDictionaryEditor : DialogPropertyValueEditor
    {
        public ArgumentDictionaryEditor()
        {
            this.InlineEditorTemplate = PropertyEditorResources.GetResources()["DynamicArgumentDictionaryInlineTemplate"] as DataTemplate;
        }

        public override void ShowDialog(PropertyValue propertyValue, IInputElement commandSource)
        {           
            ModelPropertyEntryToOwnerActivityConverter ownerActivityConverter = new ModelPropertyEntryToOwnerActivityConverter();
            ModelItem activityItem = 
                ownerActivityConverter.Convert(propertyValue.ParentProperty, typeof(ModelItem), false, null) as ModelItem;
            EditingContext context = activityItem.GetEditingContext();

            ModelItem parentModelItem =
                ownerActivityConverter.Convert(propertyValue.ParentProperty, typeof(ModelItem), true, null) as ModelItem;            
            ModelItemDictionary arguments = parentModelItem.Properties[propertyValue.ParentProperty.PropertyName].Dictionary;

            DynamicArgumentDesignerOptions options = new DynamicArgumentDesignerOptions
            {
                Title = propertyValue.ParentProperty.DisplayName
            };

            using (ModelEditingScope change = arguments.BeginEdit("PowerShellParameterEditing"))
            {
                if (DynamicArgumentDialog.ShowDialog(activityItem, arguments, context, activityItem.View, options))
                {
                    change.Complete();
                }
                else
                {
                    change.Revert();
                }
            }          
        } 
    }
}
