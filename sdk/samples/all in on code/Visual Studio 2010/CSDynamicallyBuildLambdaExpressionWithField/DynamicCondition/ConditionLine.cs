/********************************** Module Header **************************************************\
* Module Name:  ConditionLine.cs
* Project:      CSDynamicallyBuildLambdaExpressionWithField
* Copyright (c) Microsoft Corporation.
*
* The ConditionLine.cs file defines some sub-condition connection operators and some property boxes.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\**************************************************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;

namespace DynamicCondition
{
	internal partial class ConditionLine
	{

		internal ConditionLine()
		{
			InitializeComponent();
		}

        /// <summary>
        /// property DataType
        /// </summary>
		private Type _dataType;
        public Type DataType
		{
			get
			{
				return _dataType;
			}
			set
			{
				_dataType = value;
			}
		}

        /// <summary>
        /// Property DataSource
        /// </summary>
		private PropertyInfo[] _dataSource;
        public PropertyInfo[] DataSource
		{
			get
			{
				return _dataSource;
			}
			set
			{
				_dataSource = value;
				cmbColumn.DataSource = value;
				cmbColumn.DisplayMember = "Name";
			}
		}

        /// <summary>
        /// Condition compare operator
        /// </summary>
		public DynamicCondition.DynamicQuery.Condition.Compare OperatorType
		{
			get
			{
                return ((lb.Text == "AND") ? DynamicCondition.DynamicQuery.Condition.Compare.And :
                    DynamicQuery.Condition.Compare.Or);
			}
			set
			{
                if (value != DynamicQuery.Condition.Compare.And & value != DynamicQuery.Condition.Compare.Or)
				{
					throw new ArgumentException("OperatorType must be \"And\" or \"Or\"");
				}
				lb.Text = value.ToString().ToUpper();
			}
		}

		/// <summary>
        /// Returns a Condition(Of T) which represents the criteria stored in the UserControl
        /// </summary>
        public DynamicCondition.DynamicQuery.Condition<T> GetCondition<T>(T dataSrc)
		{

			var pType = ((PropertyInfo)cmbColumn.SelectedItem).PropertyType;

			// CheckType ensures that T and T? are treated the same
			if (CheckType<bool>(pType))
			{
				 return MakeCond(dataSrc, pType, chkValue.Checked);
			}
		
			else if (CheckType<DateTime>(pType))
			{
				 return MakeCond(dataSrc, pType, dtpValue.Value);
			}
			else if (CheckType<char>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToChar(tbValue.Text));
			}
			else if (CheckType<long>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToInt64(tbValue.Text));
			}
			else if (CheckType<short>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToInt16(tbValue.Text));
			}
           	else if (CheckType<ulong>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToUInt64(tbValue.Text));
			}
			else if (CheckType<ushort>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToUInt16(tbValue.Text));
			}
			else if (CheckType<float>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToSingle(tbValue.Text));
			}
			else if (CheckType<double>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToDouble(tbValue.Text));
			}
			else if (CheckType<decimal>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToDecimal(tbValue.Text));
			}
			else if (CheckType<int>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToInt32(SimulateVal.Val(tbValue.Text)));
			}
			else if (CheckType<uint>(pType))
			{
				 return MakeCond(dataSrc, pType, Convert.ToUInt32(tbValue.Text));
			}

            // This can only ever be String, since we filtered the types that we added to the ComboBox
			else 
			{
				return MakeCond(dataSrc, pType, tbValue.Text);
			}
		}


		public static List<Type> typeList;

        /// <summary>
        /// contract behind the where keyword
        /// </summary>
        public static List<Type> GetSupportedTypes()
		{
			if (typeList == null)
			{
				typeList = new List<Type>();
				typeList.AddRange(new Type[] {typeof(DateTime), typeof(DateTime?), typeof(char), 
                    typeof(char?), typeof(long), typeof(long?), typeof(short), typeof(short?), 
                    typeof(ulong), typeof(ulong?), typeof(ushort), typeof(ushort?), typeof(float),
                    typeof(float?), typeof(double), typeof(double?), typeof(decimal), typeof(decimal?),
                    typeof(bool), typeof(bool?), typeof(int), typeof(int?), typeof(uint), typeof(uint?), 
                    typeof(string)});
			}

			return typeList;
		}

        /// <summary>
        /// Combine condition 
        /// </summary>
  		private void ConditionLine_Load(object sender, EventArgs e)
		{
			cmbOperator.DisplayMember = "Name";
			cmbOperator.ValueMember = "Value";
            var opList = MakeList(new { Name = "Equal", Value = DynamicQuery.Condition.Compare.Equal }, 
                new { Name = "Not Equal", Value = DynamicQuery.Condition.Compare.NotEqual }, 
                new { Name = ">", Value = DynamicQuery.Condition.Compare.GreaterThan }, 
                new { Name = ">=", Value = DynamicQuery.Condition.Compare.GreaterThanOrEqual },
                new { Name = "<", Value = DynamicQuery.Condition.Compare.LessThan }, 
                new { Name = "<=", Value = DynamicQuery.Condition.Compare.LessThanOrEqual },
                new { Name = "Like", Value = DynamicQuery.Condition.Compare.Like });
			cmbOperator.DataSource = opList;
		}

        /// <summary>
        /// select which control to demonstrate when get property from the user picked 
        /// </summary>
        private void cboColumn_SelectedIndexChanged(object sender, EventArgs e)
		{

			// Get the underlying type for the property the user picked
			var propType = ((PropertyInfo)cmbColumn.SelectedItem).PropertyType;

			// Display appropriate control (CheckBox/TextBox/DateTimePicker) for property type
			if (CheckType<bool>(propType))
			{
				 SetVisibility(true, false, false);
			}
			
			else if (CheckType<DateTime>(propType))
			{
				 SetVisibility(false, true, false);
			}
			else
			{
				 SetVisibility(false, false, true);
			}
		}

		/// <summary>
        /// Set which control is visible
        /// </summary>
   		private void SetVisibility(bool chkBox, bool datePicker, bool txtBox)
		{
			chkValue.Visible = chkBox;
			tbValue.Visible = txtBox;
			dtpValue.Visible = datePicker;
		}

		/// <summary>
        /// Toggle between AND/OR
        /// </summary>
        private void lblOperator_Click(object sender, System.EventArgs e)
		{
			lb.Text = ((lb.Text == "AND") ? "OR" : "AND");
		}

        /// <summary>
        /// MakeCond Operator
        /// </summary>
   		private DynamicCondition.DynamicQuery.Condition<T> MakeCond<T, S>(T dataSource, Type propType, S value)
		{
			IEnumerable<T> dataSourceType = null;
            return DynamicCondition.DynamicQuery.Condition.Create<T>(dataSourceType, cmbColumn.Text,
                (DynamicQuery.Condition.Compare)cmbOperator.SelectedValue, value, propType);
		}

        /// <summary>
        /// Returns true if propType is of type T or Nullable(Of T)
        /// </summary>
		private static bool CheckType<T>(Type propType) where T: struct
		{
			return (propType.Equals(typeof(T)) | propType.Equals(typeof(T?)));
		}

        /// <summary>
        /// Turns list of parameters into an IEnumerable(Of T) (where T is an anonymous type in this case)
        /// </summary>
        private static IEnumerable<T> MakeList<T>(params T[] items)
		{
			return items;
		}
	}
}