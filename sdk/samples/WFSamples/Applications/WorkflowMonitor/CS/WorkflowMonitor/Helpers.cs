//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Workflow.ComponentModel;
using System.Windows.Forms;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    internal static class Helpers
    {
        internal static Activity[] GetNestedActivities(CompositeActivity compositeActivity)
        {
            if (compositeActivity == null)
                throw new ArgumentNullException("compositeActivity");

            IList<Activity> childActivities = null;
            ArrayList nestedActivities = new ArrayList();
            Queue compositeActivities = new Queue();
            compositeActivities.Enqueue(compositeActivity);
            while (compositeActivities.Count > 0)
            {
                CompositeActivity compositeActivity2 = (CompositeActivity)compositeActivities.Dequeue();
                childActivities = compositeActivity2.Activities;

                foreach (Activity activity in childActivities)
                {
                    nestedActivities.Add(activity);
                    if (activity is CompositeActivity)
                        compositeActivities.Enqueue(activity);
                }
            }
            return (Activity[])nestedActivities.ToArray(typeof(Activity));
        }

        internal static void AddObjectGraphToDesignerHost(IDesignerHost designerHost, Activity activity)
        {
            if (designerHost == null)
                throw new ArgumentNullException("designerHost");
            if (activity == null)
                throw new ArgumentNullException("activity");

            string rootSiteName = activity.QualifiedName;
            designerHost.Container.Add(activity, activity.QualifiedName);

            if (activity is CompositeActivity)
            {
                foreach (Activity activity2 in GetNestedActivities(activity as CompositeActivity))
                    designerHost.Container.Add(activity2, activity2.QualifiedName);
            }
        }
    }

    //ListView sorter that sorts alphanumeric or numeric
    internal class ListViewItemComparer : IComparer
    {
        private int column;
        private bool isAlphaSort;
        internal ListViewItemComparer()
        {
            this.column = 0;
        }

        internal ListViewItemComparer(int column, bool isAlphaSort)
        {
            this.isAlphaSort = isAlphaSort;
            this.column = column;
        }

        public int Compare(object x, object y)
        {
            if (isAlphaSort)
                return String.Compare(((ListViewItem)x).SubItems[column].Text, ((ListViewItem)y).SubItems[column].Text);
            else
            {
                int result = 0;
                try
                {
                    result = (Convert.ToInt32(((ListViewItem)x).SubItems[column].Text)).CompareTo((Convert.ToInt32(((ListViewItem)y).SubItems[column].Text)));
                }
                catch
                {
                    result = 0;
                }
                return result;
            }
        }
    }
}
