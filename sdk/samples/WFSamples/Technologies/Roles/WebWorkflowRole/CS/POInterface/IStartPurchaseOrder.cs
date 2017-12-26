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
using System.Security.Principal;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.WebWorkflow 
{
    [Serializable]
    public class InitiatePOEventArgs : ExternalDataEventArgs
    {
        private int     itemId;
        private string  itemName;
        private float   amount;

        public InitiatePOEventArgs()
            : base(Guid.NewGuid())
        {
        }

        public InitiatePOEventArgs(Guid instanceId, int itemId, string itemName, float amount)
            : base(instanceId)
        {
            this.itemId = itemId;
            this.itemName = itemName;
            this.amount = amount;
        }

        public int ItemId
        {
            get
            {
                return this.itemId;
            }
            set
            {
                this.itemId = value;
            }
        }

        public string ItemName
        {
            get
            {
                return this.itemName;
            }
            set
            {
                this.itemName = value;
            }
        }

        public float Amount
        {
            get
            {
                return this.amount;
            }
            set
            {
                this.amount = value;
            }
        }
    }
    
    [ExternalDataExchangeAttribute()]
    public interface IStartPurchaseOrder
    {
        event EventHandler<InitiatePOEventArgs> InitiatePurchaseOrder;
    }
    
    public class StartPurchaseOrder : IStartPurchaseOrder
    {
        public event EventHandler<InitiatePOEventArgs> InitiatePurchaseOrder;

        public void InvokePORequest(Guid instanceId, int itemId, float itemCost, string itemName, IIdentity identity)
        {
            InitiatePOEventArgs args = new InitiatePOEventArgs(instanceId, itemId, itemName, itemCost);
            String securityIdentifier = null;
            WindowsIdentity windowsIdentity = identity as WindowsIdentity;

            if (windowsIdentity != null && windowsIdentity.User != null)
                securityIdentifier = windowsIdentity.User.Translate(typeof(NTAccount)).ToString();
            else if (identity != null)
                securityIdentifier = identity.Name;

            args.Identity = securityIdentifier;
            Console.WriteLine("Purchase Order initiated by: {0}", identity.Name);

            if (InitiatePurchaseOrder != null)
                InitiatePurchaseOrder(null, args);
        }
    }
}
