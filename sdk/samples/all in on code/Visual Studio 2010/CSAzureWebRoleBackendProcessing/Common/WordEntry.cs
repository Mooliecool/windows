/****************************** Module Header ******************************\
* Project Name:   CSAzureWebRoleBackendProcessing
* Module Name:    Common
* File Name:      WordEntry.cs
* Copyright (c) Microsoft Corporation
*
* This class represents an entity in Table storage.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using Microsoft.WindowsAzure.StorageClient;

namespace CSAzureWebRoleBackendProcessing.Common
{
    public class WordEntry : TableServiceEntity
    {
        public WordEntry()
        {
            PartitionKey = "";
            RowKey = string.Format("{0:10}_{1}", DateTime.MaxValue.Ticks - DateTime.Now.Ticks, Guid.NewGuid());
        }

        public string Content { get; set; }
        public bool IsProcessed { get; set; }
    }
}