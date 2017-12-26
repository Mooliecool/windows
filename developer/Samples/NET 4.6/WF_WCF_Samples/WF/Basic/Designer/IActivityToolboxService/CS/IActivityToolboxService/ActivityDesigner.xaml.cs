//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Activities.Presentation;
using System.ComponentModel;

namespace Microsoft.Samples.IActivityToolboxService
{
    // Interaction logic for ActivityDesigner.xaml
    public partial class ActivityDesigner
    {
        System.Activities.Presentation.IActivityToolboxService toolboxService = null;
        const string itemQualifiedName = "System.Activities.Statements.Assign, System.Activities, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35";
        const string itemCategoryName = "New WF Category";

        public ActivityDesigner()
        {
            InitializeComponent();   
        }

        //The event is triggered every time you any proeprty changes on the activity including its selection
        void ModelItem_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            //If the selection on the activity changes, Toolbox contents change as well.
            if (e.PropertyName == "IsPrimarySelection")
            {               
                dynamic mi = this.ModelItem;
                bool isPrimarySelection = mi.IsPrimarySelection;

                if (isPrimarySelection == true)
                {
                    AddItem();
                }
                else
                {
                    RemoveItem();
                }   
            }
        }

        //Adds the item to the specific category
        private void AddItem()
        {
            bool categoryExists = false;
            bool itemExists = false;

            //Check if the category exists
            foreach (var item in toolboxService.EnumCategories())
            {
                if (((string)item).Contains(itemCategoryName))
                {
                    categoryExists = true;
                    break;
                }
            }

            if (categoryExists == false)
            {
                toolboxService.AddCategory(itemCategoryName);
            }

            //Check if the item exists
            foreach (var item in toolboxService.EnumItems(itemCategoryName))
            {
                if (((string)item).Contains("Assign"))
                {
                    itemExists = true;
                    break;
                }
            }

            if (itemExists == false)
            {
                toolboxService.AddItem(itemQualifiedName, itemCategoryName);
            }
        }

        //Removes the item from the specific category
    	private void RemoveItem()
        {
            foreach (var item in toolboxService.EnumItems(itemCategoryName))
            {
                if (((string)item).Contains("Assign"))
                {
                    toolboxService.RemoveItem(itemQualifiedName, itemCategoryName);
                }
            }
        }

        //The event is triggered the first time the activity is drag-dropped on the designer surface. Changing the contents of the Toolbox accordingly
        protected override void OnModelItemChanged(object newItem)
        {
            this.ModelItem.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(ModelItem_PropertyChanged);
            base.OnModelItemChanged(newItem);

            if (toolboxService == null)
            {
                toolboxService = (System.Activities.Presentation.IActivityToolboxService)this.Context.Services.GetService(typeof(System.Activities.Presentation.IActivityToolboxService));
            }

            toolboxService.AddCategory("New WF Category");
            toolboxService.AddItem(itemQualifiedName, itemCategoryName);
       }

    }
}
