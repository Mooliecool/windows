/****************************** Module Header ******************************\
* Module Name:  MySequenceActivity.cs
* Project:		CSWF4CustomSequenceActivity
* Copyright (c) Microsoft Corporation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\***************************************************************************/
using System.ComponentModel;
using System.Activities;
using System.Collections.ObjectModel;
namespace CSWF4CustomSequenceActivity
{
    [Designer(typeof(MySequenceDesigner))]
    public class MySequenceActivity : NativeActivity
    {
        public Collection<Activity> Branches { get; set; }
        public Collection<Variable> Variables { get; set; }
        public MySequenceActivity()
        {
            Branches = new Collection<Activity>();
            Variables = new Collection<Variable>();
        }
        int activityCounter = 0;
        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            metadata.SetChildrenCollection(Branches);
            metadata.SetVariablesCollection(Variables);
        }

        protected override void Execute(NativeActivityContext context)
        {
            ScheduleActivities(context);
        }

        void ScheduleActivities(NativeActivityContext context)
        {
            if (activityCounter < Branches.Count)
                context.ScheduleActivity(this.Branches[activityCounter++],
                    OnActivityCompleted);
        }

        void OnActivityCompleted(NativeActivityContext context,
            ActivityInstance completedInstance)
        {
            ScheduleActivities(context);
        }
    }
}
