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


namespace Microsoft.Samples.Workflow.DesignerControl
{
    using System;
    using System.IO;
    using System.Text;
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Collections;
    using System.ComponentModel;
    using System.Collections.Generic;
    using System.ComponentModel.Design;
    using System.ComponentModel.Design.Serialization;
    using System.Workflow.ComponentModel.Compiler;
    using System.Workflow.ComponentModel;
    using System.Workflow.ComponentModel.Design;
    using System.Windows.Forms;
    using System.Drawing;
    using System.Globalization;

    #region IdentifierCreationService
    internal sealed class IdentifierCreationService : IIdentifierCreationService
    {
        private IServiceProvider serviceProvider = null;

        internal IdentifierCreationService(IServiceProvider serviceProvider)
        {
            this.serviceProvider = serviceProvider;
        }

        void IIdentifierCreationService.ValidateIdentifier(Activity activity, string identifier)
        {
            if (identifier == null)
                throw new ArgumentNullException("identifier");
            if (activity == null)
                throw new ArgumentNullException("activity");

            if (activity.Name.ToLower(CultureInfo.InvariantCulture).Equals(identifier.ToLower(CultureInfo.InvariantCulture)))
                return;

            ArrayList identifiers = new ArrayList();
            Activity rootActivity = GetRootActivity(activity);
            identifiers.AddRange(GetIdentifiersInCompositeActivity(rootActivity as CompositeActivity));
            identifiers.Sort();
            if (identifiers.BinarySearch(identifier.ToLower(CultureInfo.InvariantCulture), StringComparer.OrdinalIgnoreCase) >= 0)
                throw new ArgumentException(string.Format("Duplicate Component Identifier {0}", identifier));
        }

        void IIdentifierCreationService.EnsureUniqueIdentifiers(CompositeActivity parentActivity, ICollection childActivities)
        {
            if (parentActivity == null)
                throw new ArgumentNullException("parentActivity");
            if (childActivities == null)
                throw new ArgumentNullException("childActivities");

            ArrayList allActivities = new ArrayList();

            Queue activities = new Queue(childActivities);
            while (activities.Count > 0)
            {
                Activity activity = (Activity)activities.Dequeue();
                if (activity is CompositeActivity)
                {
                    foreach (Activity child in ((CompositeActivity)activity).Activities)
                        activities.Enqueue(child);
                }

                //If we are moving activities, we need not regenerate their identifiers
                if (((IComponent)activity).Site != null)
                    continue;

                allActivities.Add(activity);
            }

            // get the root activity
            CompositeActivity rootActivity = GetRootActivity(parentActivity) as CompositeActivity;
            ArrayList identifiers = new ArrayList(); // all the identifiers in the workflow
            identifiers.AddRange(GetIdentifiersInCompositeActivity(rootActivity));

            foreach (Activity activity in allActivities)
            {
                string finalIdentifier = activity.Name;

                // now loop until we find a identifier that hasn't been used.
                string baseIdentifier = GetBaseIdentifier(activity);
                int index = 0;

                identifiers.Sort();
                while (finalIdentifier == null || finalIdentifier.Length == 0 || identifiers.BinarySearch(finalIdentifier.ToLower(CultureInfo.InvariantCulture), StringComparer.OrdinalIgnoreCase) >= 0)
                {
                    finalIdentifier = string.Format("{0}{1}", baseIdentifier, ++index);
                }

                // add new identifier to collection 
                identifiers.Add(finalIdentifier);
                activity.Name = finalIdentifier;
            }
        }

        private static IList GetIdentifiersInCompositeActivity(CompositeActivity compositeActivity)
        {
            ArrayList identifiers = new ArrayList();
            if (compositeActivity != null)
            {
                identifiers.Add(compositeActivity.Name);
                IList<Activity> allChildren = GetAllNestedActivities(compositeActivity);
                foreach (Activity activity in allChildren)
                    identifiers.Add(activity.Name);
            }
            return ArrayList.ReadOnly(identifiers);
        }

        private static string GetBaseIdentifier(Activity activity)
        {
            string baseIdentifier = activity.GetType().Name;
            StringBuilder b = new StringBuilder(baseIdentifier.Length);
            for (int i = 0; i < baseIdentifier.Length; i++)
            {
                if (Char.IsUpper(baseIdentifier[i]) && (i == 0 || i == baseIdentifier.Length - 1 || Char.IsUpper(baseIdentifier[i + 1])))
                {
                    b.Append(Char.ToLower(baseIdentifier[i], CultureInfo.InvariantCulture));
                }
                else
                {
                    b.Append(baseIdentifier.Substring(i));
                    break;
                }
            }
            return b.ToString();
        }

        private static Activity GetRootActivity(Activity activity)
        {
            if (activity == null)
                throw new ArgumentException("activity");

            while (activity.Parent != null)
                activity = activity.Parent;

            return activity;
        }

        private static Activity[] GetAllNestedActivities(CompositeActivity compositeActivity)
        {
            if (compositeActivity == null)
                throw new ArgumentNullException("compositeActivity");

            ArrayList nestedActivities = new ArrayList();
            Queue compositeActivities = new Queue();
            compositeActivities.Enqueue(compositeActivity);
            while (compositeActivities.Count > 0)
            {
                CompositeActivity compositeActivity2 = (CompositeActivity)compositeActivities.Dequeue();

                foreach (Activity activity in compositeActivity2.Activities)
                {
                    nestedActivities.Add(activity);
                    if (activity is CompositeActivity)
                        compositeActivities.Enqueue(activity);
                }

                foreach (Activity activity in compositeActivity2.EnabledActivities)
                {
                    if (!nestedActivities.Contains(activity))
                    {
                        nestedActivities.Add(activity);
                        if (activity is CompositeActivity)
                            compositeActivities.Enqueue(activity);
                    }
                }
            }
            return (Activity[])nestedActivities.ToArray(typeof(Activity));
        }
    }
    #endregion

    #region WorkflowCompilerOptionsService
    internal class WorkflowCompilerOptionsService : IWorkflowCompilerOptionsService
    {
        public WorkflowCompilerOptionsService()
        {
        }

        #region IWorkflowCompilerOptionsService Members
        string IWorkflowCompilerOptionsService.RootNamespace
        {
            get
            {
                return String.Empty;
            }
        }

        string IWorkflowCompilerOptionsService.Language
        {
            get
            {
                return "CSharp";
            }
        }

        public bool CheckTypes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        #endregion
    }
    #endregion

    #region Class EventBindingService
    internal class EventBindingService : IEventBindingService
    {
        public EventBindingService()
        {
        }

        public string CreateUniqueMethodName(IComponent component, EventDescriptor e)
        {
            return e.DisplayName;
        }

        public ICollection GetCompatibleMethods(EventDescriptor e)
        {
            return new ArrayList();
        }

        public EventDescriptor GetEvent(PropertyDescriptor property)
        {
            return (property is EventPropertyDescriptor) ? ((EventPropertyDescriptor)property).EventDescriptor : null;
        }

        public PropertyDescriptorCollection GetEventProperties(EventDescriptorCollection events)
        {
            return new PropertyDescriptorCollection(new PropertyDescriptor[] { }, true);
        }

        public PropertyDescriptor GetEventProperty(EventDescriptor e)
        {
            return new EventPropertyDescriptor(e);
        }

        public bool ShowCode()
        {
            return false;
        }

        public bool ShowCode(int lineNumber)
        {
            return false;
        }

        public bool ShowCode(IComponent component, EventDescriptor e)
        {
            return false;
        }

        private class EventPropertyDescriptor : PropertyDescriptor
        {
            private EventDescriptor eventDescriptorValue;

            public EventDescriptor EventDescriptor
            {
                get
                {
                    return this.eventDescriptorValue;
                }
            }

            public EventPropertyDescriptor(EventDescriptor eventDescriptor)
                : base(eventDescriptor, null)
            {
                this.eventDescriptorValue = eventDescriptor;
            }

            public override Type ComponentType
            {
                get
                {
                    return this.eventDescriptorValue.ComponentType;
                }
            }
            public override Type PropertyType
            {
                get
                {
                    return this.eventDescriptorValue.EventType;
                }
            }

            public override bool IsReadOnly
            {
                get
                {
                    return true;
                }
            }

            public override bool CanResetValue(object component)
            {
                return false;
            }

            public override object GetValue(object component)
            {
                return null;
            }

            public override void ResetValue(object component)
            {
            }

            public override void SetValue(object component, object value)
            {
            }

            public override bool ShouldSerializeValue(object component)
            {
                return false;
            }
        }
    }
    #endregion    
}
