/********************************** Module Header **********************************\
* Module Name:  ConditionBuilderDesigner.cs
* Project:      CSDynamicallyBuildLambdaExpressionWithField
* Copyright (c) Microsoft Corporation.
* 
* The ConditionBuilderDesigner.cs file defines some collections in order to 
* demonstrate how to integrating with other control
*
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Windows.Forms.Design;

// Designer used to display Smart Tags for ConditionBuilder
namespace DynamicCondition
{
    internal class ConditionBuilderDesigner : ControlDesigner
    {
        private DesignerActionListCollection actions = new DesignerActionListCollection();

        /// <summary>
        /// override property for integrating PropertyGrid control
        /// </summary>
        public override DesignerActionListCollection ActionLists
        {
            get
            {
                if (actions.Count == 0)
                {
                    actions.Add(new ConditionBuilderActionList(this.Component));
                }
                return actions;
            }
        }

        /// <summary>
        /// Provides the base class for types that define a list of items used to create 
        /// a smart tag panel.
        /// </summary>
        public class ConditionBuilderActionList : DesignerActionList
        {
            private ConditionBuilder cBuilder;
            public ConditionBuilderActionList(IComponent component)
                : base(component)
            {
                cBuilder = (ConditionBuilder)component;
            }

            /// <summary>
            /// Property Lines
            /// </summary>
            public int Lines
            {
                get
                {
                    return cBuilder.Lines;
                }
                set
                {
                    GetPropertyByName("Lines").SetValue(cBuilder, value);
                }
            }

            /// <summary>
            /// Property OperatorType
            /// </summary>
            public ConditionBuilder.Compare OperatorType
            {
                get
                {
                    return cBuilder.OperatorType;
                }
                set
                {
                    GetPropertyByName("OperatorType").SetValue(cBuilder, value);
                }
            }

            /// <summary>
            /// Property Box
            /// </summary>
            private PropertyDescriptor GetPropertyByName(string propName)
            {
                var prop = TypeDescriptor.GetProperties(cBuilder)[propName];
                if (prop == null)
                {
                    throw new ArgumentException("Invalid Property.", propName);
                }
                return prop;
            }

            /// <summary>
            /// Create the elements which will appear in the Smart Tag
            /// </summary>
            public override System.ComponentModel.Design.DesignerActionItemCollection GetSortedActionItems()
            {
                DesignerActionItemCollection items = new DesignerActionItemCollection();
                items.Add(new DesignerActionHeaderItem("Appearance"));
                items.Add(new DesignerActionPropertyItem("Lines", "Number of Lines:", "Appearance",
                    "Sets the number of lines in the ConditionBuilder"));
                items.Add(new DesignerActionPropertyItem("OperatorType", "Default Operator:", "Appearance",
                    "Default operator to use"));
                return items;
            }
        }
    }
}