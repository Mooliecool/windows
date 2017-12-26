/****************************** Module Header ******************************\
* Project Name:   CSAzureWebRoleBackendProcessing
* Module Name:    Common
* File Name:      WordDataContext.cs
* Copyright (c) Microsoft Corporation
*
* This class represents a System.Data.Services.Client.DataServiceContext object 
* for use with the Windows Azure Table service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System.Linq;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;

namespace CSAzureWebRoleBackendProcessing.Common
{
    public class WordDataContext : TableServiceContext
    {
        public WordDataContext(string baseAddress, StorageCredentials credentials)
            : base(baseAddress, credentials)
        { }

        public IQueryable<WordEntry> WordEntry
        {
            get
            {
                return this.CreateQuery<WordEntry>("WordEntry");
            }
        }
    }
}