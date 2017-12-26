/********************************** Module Header **********************************\
* Module Name:  ConditionBuilder.cs
* Project:      CSDynamicallyBuildLambdaExpressionWithField
* Copyright (c) Microsoft Corporation.
*
* The ConditionBuilder.cs file defines a UserControl for first condition
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
using System.Collections.Generic;
using System.Windows.Forms;
using System.Linq;
using System.ComponentModel;

namespace DynamicCondition
{
    /// <summary>
    /// Designer metadata
    /// </summary>
    [Designer(typeof(ConditionBuilderDesigner))]
    public partial class ConditionBuilder
    {

        public ConditionBuilder()
        {
            InitializeComponent();
        }


        #region Properties
        private const string cName = "ConditionLine";

        /// <summary>
        /// enum for definition of condition type
        /// </summary>
        public enum Compare : int
        {
            And = DynamicCondition.DynamicQuery.Condition.Compare.And,
            Or = DynamicCondition.DynamicQuery.Condition.Compare.Or
        }

        private int _lines = 1;
        private Type _type;
        private Compare _operatorType = Compare.And;

        /// <summary>
        /// The number of ConditionLine controls to display
        /// </summary>
        public int Lines
        {
            get
            {
                return _lines;
            }
            set
            {
                if (value < 1)
                {
                    throw new ArgumentException("Lines cannot be less than 1");
                }

                if (value > _lines)
                {
                    // Create the new ConditionLines
                    for (var i = _lines; i < value; i++)
                    {
                        ConditionLine cLine = new ConditionLine
                        {
                            Name = cName + (i + 1),
                            Left = ConditionLine1.Left,
                            Width = ConditionLine1.Width,
                            Top = ConditionLine1.Top + i * (ConditionLine1.Height + 1),
                            OperatorType = (DynamicQuery.Condition.Compare)_operatorType,
                            Anchor = AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right
                        };

                        this.Controls.Add(cLine);
                    }

                }
                else if (value < _lines)
                {

                    // Remove the extra ConditionLines
                    for (var i = value; i <= _lines; i++)
                    {
                        this.Controls.RemoveByKey(cName + (i + 1));
                    }

                }
                _lines = value;
            }
        }

        /// <summary>
        /// Default operator (And/Or) to be used for each ConditionLine
        /// </summary>
        public Compare OperatorType
        {
            get
            {
                return _operatorType;
            }
            set
            {
                _operatorType = value;
                for (var i = 1; i <= _lines; i++)
                {
                    GetConditionLine(cName + i).OperatorType = (DynamicCondition.DynamicQuery.Condition.Compare)value;
                }
            }
        }
        #endregion

        #region Public Methods
        /// <summary>
        /// Populates the dropdowns with the columns contained in dataSource.  dataSource can
        /// be either an IEnumerable(Of T) (for local), or an IQueryable(Of T) (for remote)
        /// </summary>
        public void SetDataSource<T>(IEnumerable<T> dataSource)
        {
            _type = typeof(T);

            // Filter out all properties that are not intrinsic types
            // For example, a Customers object may have an Orders property of type 
            // EntityRef(Of Order), but it's not meaningful to display this property in the list.
            // Note that the underlying Condition API does support nested property access, but
            // it's just the ConditionBuilder control doesn't give the user a mechanism to
            // specify it.
            var props = from p in _type.GetProperties()
                        where DynamicCondition.ConditionLine.GetSupportedTypes().Contains(p.PropertyType)
                        select p;

            // Load the columns into each ConditionLine
            for (var i = 1; i <= _lines; i++)
            {
                GetConditionLine(cName + i).DataSource = (System.Reflection.PropertyInfo[])(props.ToArray().Clone());
            }
        }



        /// <summary>
        /// Use this method to get a condition object which represents all the data the user
        /// has entered into the ConditionBuilder.
        /// </summary>
        public DynamicCondition.DynamicQuery.Condition<T> GetCondition<T>()
        {

            // This is just used to infer the type, so there's no need to instantiate it
            T dataSrc = default(T);
            var finalCond = GetConditionLine(cName + "1").GetCondition<T>(dataSrc);

            // Extract the condition from each ConditionLine and then combine it with finalCond
            for (var i = 2; i <= _lines; i++)
            {
                var cLine = GetConditionLine(cName + i);
                finalCond = DynamicCondition.DynamicQuery.Condition.Combine<T>(finalCond, cLine.OperatorType,
                    cLine.GetCondition<T>(dataSrc));
            }

            return finalCond;
        }

        #endregion

        #region Private Methods
        /// <summary>
        /// Takes in "ConditionLine2" and returns the actual instance of the control
        /// </summary>
        private ConditionLine GetConditionLine(string name)
        {
            return (ConditionLine)(this.Controls[name]);
        }

        /// <summary>
        /// Execution when Loading ConditionBuilder 
        /// </summary>
        private void ConditionBuilder_Load(object sender, EventArgs e)
        {
            this.ConditionLine1.lb.Visible = false;
        }
        #endregion

    }
}