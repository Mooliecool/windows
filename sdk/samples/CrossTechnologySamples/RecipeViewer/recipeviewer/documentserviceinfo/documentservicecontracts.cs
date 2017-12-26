// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections.Generic;
using System.IO;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Text;
using System.Xml;

namespace Microsoft.Samples.RecipeCatalog
{
    /// <summary>
    /// IDocumentService - contract for adding/updating/retrieving documents and attachments
    /// </summary>
	[ServiceContract(Name="IDocumentService", Namespace = "http://Microsoft.Samples.RecipeCatalog")]
	public interface IDocumentService
	{
		[OperationContract]
        [FaultContract(typeof(NoDocumentsFoundFaultException))]
		DocHeader[] GetDocHeaders(string searchKey);

        [OperationContract]
        [FaultContract(typeof(DocumentNotFoundFaultException))]
        Message GetDoc(Message docKey);

        [OperationContract]
        [FaultContract(typeof(AttachmentNotFoundFaultException))]
        Stream GetAttachment(string docKey, string attachmentKey);

        [OperationContract]
        [FaultContract(typeof(UnableToAddDocumentFaultException))]
        Message AddDoc(Message doc);
	}
}