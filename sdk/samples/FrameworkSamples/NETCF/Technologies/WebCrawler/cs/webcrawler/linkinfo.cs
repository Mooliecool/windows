//---------------------------------------------------------------------
// File: LinkInfo.cs
//
// This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
// Copyright (C) Microsoft Corporation.  All rights reserved.
// 
// This source code is intended only as a supplement to Microsoft
// Development Tools and/or on-line documentation.  See these other
// materials for detailed information regarding Microsoft code samples.
// 
// THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Net;

namespace Microsoft.Samples.NetCF
{
    /// <summary>
    /// Class describing a link
    /// </summary>
    public class LinkInfo
    {
        private string linkPathValue;
        /// <summary>
        /// The link's address (ex: http://www.microsoft.com)
        /// </summary>
        public string LinkPath
        {
            get
            { 
                return this.linkPathValue; 
            }
        }

        private HttpStatusCode statusCodeValue;
        /// <summary>
        /// HttpStatusCode received when we attempted to
        /// connect to the link
        /// NOTE: Other possible values
        ///  0 == no connection attempted
        /// -1 == generic failure
        /// -2 == link does not point to html data
        /// </summary>
        public HttpStatusCode StatusCode
        {
            get
            {
                return this.statusCodeValue;
            }

            set
            {
                this.statusCodeValue = value;
            }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">
        /// The link's address
        /// </param>
        /// <param name="status">
        /// Status code from connection attempt
        /// </param>
        public LinkInfo(string path,
                        HttpStatusCode status)
        {
            this.linkPathValue = path;
            this.statusCodeValue = status;
        }
    }
}
