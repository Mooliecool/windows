/****************************** Module Header *********************************************************\ 
 * Module Name: OpenOtherWorkItemControl
 * Project:     CSTFSCustomWorkItemControl
 * Copyright (c) Microsoft Corporation. 
 *
 * The OpenOtherWorkItemControl demonstrates how to create and deploy a Custom WorkItem Control which
 * open another work item in Visual Studio.
 * 
 * This source is subject to the Microsoft Public License. 
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved. 
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 \********************************************************************************************************/



#region Using Directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Microsoft.TeamFoundation.WorkItemTracking.Controls;
using System.ComponentModel;
using Microsoft.TeamFoundation.WorkItemTracking.Client;
using System.Collections.Specialized;
using Microsoft.VisualStudio.TeamFoundation;
using Microsoft.TeamFoundation.Client;
using Microsoft.VisualStudio.TeamFoundation.WorkItemTracking;
#endregion

namespace CSTFSCustomWorkItemControl
{
    public class OpenOtherWorkItemControl : UserControl, IWorkItemControl
    {
        #region InitializeUIComponent

        public OpenOtherWorkItemControl()
        {
            InitializeComponent();
        }

        private TextBox tb1;
        private Button btn1;

        private void InitializeComponent()
        {
            this.tb1 = new System.Windows.Forms.TextBox();
            this.btn1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // tb1
            // 
            this.tb1.Location = new System.Drawing.Point(4, 2);
            this.tb1.Name = "tb1";
            this.tb1.Size = new System.Drawing.Size(100, 20);
            this.tb1.TabIndex = 0;
            // 
            // btn1
            // 
            this.btn1.Location = new System.Drawing.Point(111, 0);
            this.btn1.Name = "btn1";
            this.btn1.Size = new System.Drawing.Size(66, 23);
            this.btn1.TabIndex = 1;
            this.btn1.Text = "Open";
            this.btn1.UseVisualStyleBackColor = true;
            this.btn1.Click += new System.EventHandler(this.btn1_Click);
            // 
            // OpenOtherWorkItemControl
            // 
            this.Controls.Add(this.btn1);
            this.Controls.Add(this.tb1);
            this.Name = "OpenOtherWorkItemControl";
            this.Size = new System.Drawing.Size(180, 25);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        #endregion

        #region Fields and properties

        private EventHandlerList m_events;
        private EventHandlerList Events
        {
            get
            {
                if (m_events == null)
                {
                    m_events = new EventHandlerList();
                }
                return m_events;
            }
        }

        private static object EventBeforeUpdateDatasource = new object();
        private static object EventAfterUpdateDatasource = new object();

        private StringDictionary m_properties;
        private bool m_readOnly;
        private IServiceProvider m_serviceProvider;
        private WorkItem m_workItem;
        private string m_fieldName;

       /// <summary>
        ///   The top-level object in the Visual Studio automation object model. 
       /// </summary>
        EnvDTE80.DTE2 dte2;
        EnvDTE80.DTE2 DTE2
        {
            get
            {
                if (dte2 == null)
                {
                    //Get Visual Studio automation object model from m_serviceProvider
                    var vsModel = m_serviceProvider.GetService(typeof(EnvDTE.DTE));
                    dte2 = (EnvDTE80.DTE2)vsModel;
                }
                return dte2;
            }
        }
        #endregion

        #region IWorkItemControl Members

        /// <summary>
        /// Raise this events before updating WorkItem object with values. When value is changed 
        /// by a control, work item form asks all controls (except current control) to refresh their display 
        /// values (by calling InvalidateDatasource) in case if affects other controls 
        /// </summary>
        public event EventHandler BeforeUpdateDatasource
        {
            add { Events.AddHandler(EventBeforeUpdateDatasource, value); }
            remove { Events.RemoveHandler(EventBeforeUpdateDatasource, value); }
        }
        /// <summary>
        /// Raise this event after updating WorkItem object with values. When value is changed 
        /// by a control, work item form asks all controls (except current control) to refresh their display 
        /// values (by calling InvalidateDatasource) in case if affects other controls 
        /// </summary>
        public event EventHandler AfterUpdateDatasource
        {
            add { Events.AddHandler(EventAfterUpdateDatasource, value); }
            remove { Events.RemoveHandler(EventAfterUpdateDatasource, value); }
        }
        
        /// <summary>
        /// Control is asked to clear its contents
        /// </summary>
        void IWorkItemControl.Clear()
        {
            tb1.Text = string.Empty;
        }

        /// <summary>
        /// Control is requested to flush any data to workitem object. 
        /// </summary>
        void IWorkItemControl.FlushToDatasource()
        {           
        }
        
        ///<summary>
        /// Asks control to invalidate the contents and redraw.
        /// </summary>
        void IWorkItemControl.InvalidateDatasource()
        {
        }

        ///<summary>
        /// All attributes specified in work item type definition file for this control, including custom attributes
        /// </summary>
        StringDictionary IWorkItemControl.Properties
        {
            get
            {
                return m_properties;
            }
            set
            {
                m_properties = value;
            }
        }

        /// <summary>
        /// Whether the control is readonly.
        /// </summary>
        bool IWorkItemControl.ReadOnly
        {
            get
            {
                return m_readOnly;
            }
            set
            {
                m_readOnly = value;
            }
        }

        /// <summary>
        /// Gives pointer to IServiceProvider if you have to use VS Services. 
        /// </summary>
        void IWorkItemControl.SetSite(IServiceProvider serviceProvider)
        {
            m_serviceProvider = serviceProvider;
        }

        /// <summary>
        /// WorkItemDatasource refers to current work item object. 
        /// </summary>
        object IWorkItemControl.WorkItemDatasource
        {
            get
            {
                return m_workItem;
            }
            set
            {
                m_workItem = (WorkItem)value;
        
                //WorkItem.FieldChanged event is very useful, and it will be fired when the value
                //  of other field changed
                //m_workItem.FieldChanged+=new WorkItemFieldChangeEventHandler(m_workItem_FieldChanged);
            }
        }

        /// <summary>
        /// The field name which the control is associated with in work item type definition.
        ///</summary>
        string IWorkItemControl.WorkItemFieldName
        {
            get
            {
                return m_fieldName;
            }
            set
            {
                m_fieldName = value;
            }
        }
        #endregion

        #region Private funtions

        /// <summary>
        /// Returen the instance of TeamFoundationServerExt, from which we can get current 
        /// TFS ActiveProjectContext
        /// </summary>
        private TeamFoundationServerExt GetTFSExt()
        {
            TeamFoundationServerExt tfsExt = DTE2.GetObject("Microsoft.VisualStudio.TeamFoundation.TeamFoundationServerExt") as TeamFoundationServerExt;
            if ((tfsExt.ActiveProjectContext.DomainUri == null) || (tfsExt.ActiveProjectContext.ProjectUri == null))
            {
                MessageBox.Show("Error");
                return null;
            }
            return tfsExt;
        }
        #endregion
        
        #region UI Event
        private void btn1_Click(object sender, EventArgs e)
        {
            //check whether the value in tb1 is integer
            int workitemID = 0;
            bool isInteger = int.TryParse(tb1.Text, out workitemID);
            if (!isInteger || workitemID <= 0)
            {
                MessageBox.Show("Work Item ID must be an integer and lager than 0");
                return;
            }



            TeamFoundationServerExt tfsExt = this.GetTFSExt();
            //chech whether tfsExt exists
            if (tfsExt != null)
            {
                DocumentService witDocumentService = (DocumentService)DTE2.DTE.GetObject(
                    "Microsoft.VisualStudio.TeamFoundation.WorkItemTracking.DocumentService");
                //create a TFS instance with the url
                TeamFoundationServer activeTFS = new TeamFoundationServer(
                    tfsExt.ActiveProjectContext.DomainUri);
                IWorkItemDocument widoc = null;
                widoc = witDocumentService.GetWorkItem(activeTFS, workitemID, this);
                try
                {
                    //Open the work item in Visual Studio
                    witDocumentService.ShowWorkItem(widoc);
                }
                finally
                {
                    widoc.Release(this);
                }
            }
        }
        #endregion
    }
}
