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
    using System.Xml;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.ComponentModel.Design.Serialization;
    using System.Workflow.ComponentModel;
    using System.Workflow.ComponentModel.Compiler;
    using System.Workflow.ComponentModel.Design;
    using System.Workflow.ComponentModel.Serialization;
    using System.Collections;
    using System.Collections.Generic;
    using System.Drawing.Design;
    using System.Windows.Forms;

    #region WorkflowLoader
    internal sealed class WorkflowLoader : WorkflowDesignerLoader
    {
        private string xamlValue = string.Empty;

        internal WorkflowLoader()
        {
        }

        protected override void Initialize()
        {
            base.Initialize();

            IDesignerLoaderHost host = LoaderHost;
            if (host != null)
            {
                host.RemoveService(typeof(IIdentifierCreationService));
                host.AddService(typeof(IIdentifierCreationService), new IdentifierCreationService(host));
                host.AddService(typeof(IMenuCommandService), new MenuCommandService(host));
                TypeProvider typeProvider = new TypeProvider(host);
                typeProvider.AddAssemblyReference(typeof(string).Assembly.Location);
                host.AddService(typeof(ITypeProvider), typeProvider, true);
                host.AddService(typeof(IEventBindingService), new EventBindingService());
            }
        }

        public override void Dispose()
        {
            try
            {
                IDesignerLoaderHost host = LoaderHost;
                if (host != null)
                {
                    host.RemoveService(typeof(IIdentifierCreationService));
                    host.RemoveService(typeof(IMenuCommandService));
                    host.RemoveService(typeof(IToolboxService));
                    host.RemoveService(typeof(ITypeProvider), true);
                    host.RemoveService(typeof(IWorkflowCompilerOptionsService));
                    host.RemoveService(typeof(IEventBindingService));
                }
            }
            finally
            {
                base.Dispose();
            }
        }

        public override void ForceReload()
        {
        }

        private Type workflowTypeValue = null;

        public Type WorkflowType
        {
            get { return this.workflowTypeValue; }
            set { this.workflowTypeValue = value; }
        }


        public string Xaml
        {
            get
            {
                return this.xamlValue;
            }

            set
            {
                this.xamlValue = value;
            }
        }

        public override string FileName
        {
            get
            {
                return string.Empty;
            }
        }

        public override TextReader GetFileReader(string filePath)
        {
            return new StreamReader(new FileStream(filePath, FileMode.OpenOrCreate));
        }

        public override TextWriter GetFileWriter(string filePath)
        {
            // close the handle
            return new StreamWriter(new FileStream(filePath, FileMode.OpenOrCreate));
        }

        protected override void PerformLoad(IDesignerSerializationManager serializationManager)
        {
            IDesignerHost designerHost = (IDesignerHost)GetService(typeof(IDesignerHost));
            Activity rootActivity = null;
            //First see if we have a workflow type
            if (WorkflowType != null)
            {
                rootActivity = (Activity)Activator.CreateInstance(WorkflowType);
            }
            else
            {
                // Create a text reader out of the doc data, and ask
                TextReader reader = new StringReader(this.xamlValue);
                try
                {
                    using (XmlReader xmlReader = XmlReader.Create(reader))
                    {
                        WorkflowMarkupSerializer xamlSerializer = new WorkflowMarkupSerializer();
                        rootActivity = xamlSerializer.Deserialize(xmlReader) as Activity;
                    }
                }
                finally
                {
                    reader.Close();
                }
            }

            if (rootActivity != null && designerHost != null)
            {
                AddObjectGraphToDesignerHost(designerHost, rootActivity);
            }
        }

        public override void Flush()
        {
            PerformFlush(null);
        }

        protected override void PerformFlush(IDesignerSerializationManager manager)
        {
            IDesignerHost host = (IDesignerHost)GetService(typeof(IDesignerHost));
            if (host != null && host.RootComponent != null)
            {
                Activity rootActivity = host.RootComponent as Activity;
                if (rootActivity != null)
                {
                    StringWriter writer = new StringWriter();
                    try
                    {
                        using (XmlWriter xmlWriter = XmlWriter.Create(writer))
                        {
                            WorkflowMarkupSerializer xamlSerializer = new WorkflowMarkupSerializer();
                            xamlSerializer.Serialize(xmlWriter, rootActivity);
                        }
                    }
                    finally
                    {
                        writer.Close();
                    }
                    Xaml = writer.ToString();
                }
            }
        }

        private static void AddObjectGraphToDesignerHost(IDesignerHost designerHost, Activity activity)
        {
            Guid Definitions_Class = new Guid("3FA84B23-B15B-4161-8EB8-37A54EFEEFC7");

            if (designerHost == null)
                throw new ArgumentNullException("designerHost");
            if (activity == null)
                throw new ArgumentNullException("activity");

            string rootSiteName = activity.QualifiedName;
            if (activity.Parent == null)
            {
                string fullClassName = activity.UserData[Definitions_Class] as string;
                if (fullClassName == null)
                    fullClassName = activity.GetType().FullName;
                rootSiteName = (fullClassName.LastIndexOf('.') != -1) ? fullClassName.Substring(fullClassName.LastIndexOf('.') + 1) : fullClassName;
                designerHost.Container.Add(activity, rootSiteName);
            }
            else
            {
                designerHost.Container.Add(activity, activity.QualifiedName);
            }

            if (activity is CompositeActivity)
            {
                foreach (Activity activity2 in GetNestedActivities(activity as CompositeActivity))
                    designerHost.Container.Add(activity2, activity2.QualifiedName);
            }
        }

        private static Activity[] GetNestedActivities(CompositeActivity compositeActivity)
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

        internal static void DestroyObjectGraphFromDesignerHost(IDesignerHost designerHost, Activity activity)
        {
            if (designerHost == null)
                throw new ArgumentNullException("designerHost");
            if (activity == null)
                throw new ArgumentNullException("activity");

            designerHost.DestroyComponent(activity);

            if (activity is CompositeActivity)
            {
                foreach (Activity activity2 in GetNestedActivities(activity as CompositeActivity))
                    designerHost.DestroyComponent(activity2);
            }
        }
    }
    #endregion
}
