/************************************* Module Header **************************************\
* Module Name:	UC_SmartTagSupport.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The SmartTagSupport sample demonstrates how to add smart tags for a control.
 * 
* Smart tags are menu-like user interface (UI) elements that supply commonly used design-time 
* options. 
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
using System.ComponentModel.Design;

namespace CSWinFormDesigner.SmartTags
{
    [Designer(typeof(UC_SmartTagSupportDesigner))]
    public partial class UC_SmartTagSupport : UserControl
    {
        public UC_SmartTagSupport()
        {
            InitializeComponent();
        }
    }

    [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")] 
    public class UC_SmartTagSupportDesigner :
             System.Windows.Forms.Design.ControlDesigner
    {
        private DesignerActionListCollection actionLists;

        // Use pull model to populate smart tag menu.
        public override DesignerActionListCollection ActionLists
        {
            get
            {
                if (null == actionLists)
                {
                    actionLists = new DesignerActionListCollection();
                    actionLists.Add(
                        new UC_SmartTagSupportActionList(this.Component));
                }
                return actionLists;
            }
        }
    }


    // DesignerActionList-derived class defines smart tag entries and
    // resultant actions.
    public class UC_SmartTagSupportActionList :
              System.ComponentModel.Design.DesignerActionList
    {
        private UC_SmartTagSupport control;

        private DesignerActionUIService designerActionUISvc = null;

        //The constructor associates the control 
        //with the smart tag list.
        public UC_SmartTagSupportActionList(IComponent component)
            : base(component)
        {
            this.control = component as UC_SmartTagSupport;

            // Cache a reference to DesignerActionUIService, so the
            // DesigneractionList can be refreshed.
            this.designerActionUISvc =
                GetService(typeof(DesignerActionUIService))
                as DesignerActionUIService;
        }

        // Helper method to retrieve control properties. Use of 
        // GetProperties enables undo and menu updates to work properly.
        private PropertyDescriptor GetPropertyByName(String propName)
        {
            PropertyDescriptor prop;
            prop = TypeDescriptor.GetProperties(control)[propName];
            if (null == prop)
                throw new ArgumentException(
                     "Matching ColorLabel property not found!",
                      propName);
            else
                return prop;
        }

        // Properties that are targets of DesignerActionPropertyItem entries.
        public Color BackColor
        {
            get
            {
                return control.BackColor;
            }
            set
            {
                GetPropertyByName("BackColor").SetValue(control, value);
            }
        }

        public Color ForeColor
        {
            get
            {
                return control.ForeColor;
            }
            set
            {
                GetPropertyByName("ForeColor").SetValue(control, value);
            }
        }

        // Implementation of this abstract method creates smart tag  
        // items, associates their targets, and collects into list.
        public override DesignerActionItemCollection GetSortedActionItems()
        {
            DesignerActionItemCollection items = new DesignerActionItemCollection();

            //Define static section header entries.
            items.Add(new DesignerActionHeaderItem("Appearance"));
            items.Add(new DesignerActionHeaderItem("Information"));

            items.Add(new DesignerActionPropertyItem("BackColor",
                                 "Back Color", "Appearance",
                                 "Selects the background color."));

            items.Add(new DesignerActionPropertyItem("ForeColor",
                                 "Fore Color", "Appearance",
                                 "Selects the foreground color."));

            //Create entries for static Information section.
            StringBuilder location = new StringBuilder("Location: ");
            location.Append(control.Location);
            StringBuilder size = new StringBuilder("Size: ");
            size.Append(control.Size);
            items.Add(new DesignerActionTextItem(location.ToString(),
                             "Information"));
            items.Add(new DesignerActionTextItem(size.ToString(),
                             "Information"));

            return items;
        }
    }

}
