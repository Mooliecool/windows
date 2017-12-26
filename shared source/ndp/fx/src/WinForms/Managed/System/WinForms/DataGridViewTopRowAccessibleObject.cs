//------------------------------------------------------------------------------
// <copyright file="DataGridViewTopRowAccessibleObject.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>            
//------------------------------------------------------------------------------

using System.Security.Permissions;
using System.Drawing;
using System.Diagnostics;

namespace System.Windows.Forms
{
    public partial class DataGridView
    {
        /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridView.DataGridViewTopRowAccessibleObject"]/*' />
        [
            System.Runtime.InteropServices.ComVisible(true)
        ]
        protected class DataGridViewTopRowAccessibleObject : AccessibleObject
        {
            DataGridView owner;

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.DataGridViewTopRowAccessibleObject1"]/*' />
            public DataGridViewTopRowAccessibleObject() : base()
            {
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.DataGridViewTopRowAccessibleObject2"]/*' />
            public DataGridViewTopRowAccessibleObject(DataGridView owner) : base()
            {
                this.owner = owner;
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Bounds"]/*' />
            public override Rectangle Bounds
            {
                get
                {
                    if (this.owner == null)
                    {
                        throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerNotSet));
                    }
                    if (this.owner.ColumnHeadersVisible)
                    {
                        Rectangle rect = Rectangle.Union(this.owner.layout.ColumnHeaders, this.owner.layout.TopLeftHeader);
                        return this.owner.RectangleToScreen(rect);
                    }
                    else
                    {
                        return Rectangle.Empty;
                    }
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Name"]/*' />
            public override string Name
            {
                get
                {
                    return SR.GetString(SR.DataGridView_AccTopRow);
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Owner"]/*' />
            public DataGridView Owner
            {
                get
                {
                    return this.owner;
                }
                set
                {
                    if (this.owner != null)
                    {
                        throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerAlreadySet));
                    }
                    this.owner = value;
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Parent"]/*' />
            public override AccessibleObject Parent
            {
                [SecurityPermission(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
                get
                {
                    if (this.owner == null)
                    {
                        throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerNotSet));
                    }
                    return this.owner.AccessibilityObject;
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Role"]/*' />
            public override AccessibleRole Role
            {
                get
                {
                    return AccessibleRole.Row;
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Value"]/*' />
            public override string Value
            {
                [SecurityPermission(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
                get
                {
                    return this.Name;
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.GetChild"]/*' />
            public override AccessibleObject GetChild(int index)
            {
                if (this.owner == null)
                {
                    throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerNotSet));
                }
                
                if (index < 0)
                {
                    throw new ArgumentOutOfRangeException("index");
                }

                if (index == 0 && this.owner.RowHeadersVisible)
                {
                    return this.owner.TopLeftHeaderCell.AccessibilityObject;
                }

                if (this.owner.RowHeadersVisible)
                {
                    // decrement the index because the first child is the top left header cell
                    index --;
                }

                Debug.Assert(index >= 0);

                if (index < this.owner.Columns.GetColumnCount(DataGridViewElementStates.Visible))
                {
                    int actualColumnIndex = this.owner.Columns.ActualDisplayIndexToColumnIndex(index, DataGridViewElementStates.Visible);
                    return this.owner.Columns[actualColumnIndex].HeaderCell.AccessibilityObject;
                }
                else
                {
                    return null;
                }
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.GetChildCount"]/*' />
            public override int GetChildCount()
            {
                if (this.owner == null)
                {
                    throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerNotSet));
                }
                int result = this.owner.Columns.GetColumnCount(DataGridViewElementStates.Visible);
                if (this.owner.RowHeadersVisible)
                {
                    // + 1 is the top left header cell accessibility object
                    result ++;
                }

                return result;
            }

            /// <include file='doc\DataGridViewTopRowAccessibleObject.uex' path='docs/doc[@for="DataGridViewTopRowAccessibleObject.Navigate"]/*' />
            [SecurityPermission(SecurityAction.Demand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            public override AccessibleObject Navigate(AccessibleNavigation navigationDirection)
            {
                if (this.owner == null)
                {
                    throw new InvalidOperationException(SR.GetString(SR.DataGridViewTopRowAccessibleObject_OwnerNotSet));
                }
                switch (navigationDirection)
                {
                    case AccessibleNavigation.Down:
                    case AccessibleNavigation.Next:
                        if (this.owner.AccessibilityObject.GetChildCount() > 1)
                        {
                            return this.owner.AccessibilityObject.GetChild(1);
                        }
                        else
                        {
                            return null;
                        }
                    case AccessibleNavigation.FirstChild:
                        return this.GetChild(0);
                    case AccessibleNavigation.LastChild:
                        return this.GetChild(this.GetChildCount() - 1);
                    default:
                        return null;
                }
            }
        }
    }
}
