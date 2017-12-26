// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;

namespace Microsoft.Samples.RecipeCatalog
{
    /////////////////////////////////////////////////////////////////////
    // Data contract for document metadata
	//
    [DataContract]
	public class DocHeader
	{
		private string title;
		private string[] searchKeywords;
		private int rating;
		private string key; 
        private AttachmentHeader[] attachments;

		public DocHeader(string title, string[] searchKeywords, int rating, string key, AttachmentHeader[] attachments)
		{
			this.title = title;
			this.searchKeywords = searchKeywords;
			this.rating = rating;
			this.key = key;
            this.attachments = attachments;
		}

		[DataMember]
		public string Title
		{
			get
			{
				return title;
			}
			set
			{
				title = value;
			}
		}

		[DataMember]
		public string[] SearchKeywords
		{
			get
			{
                return searchKeywords;
			}
			set
			{
				searchKeywords = value;
			}
		}

		[DataMember]
		public int Rating
		{
			get
			{
				return rating;
			}
			set
			{
                rating = value;
			}
		}

		[DataMember]
		public string Key
		{
			get
			{
				return key;
			}
			set
			{
				key = value;
			}
		}

        [DataMember]
        public AttachmentHeader[] Attachments
        {
            get
            {
                return attachments;
            }
            set
            {
                attachments = value;
            }
        }
	}

    /////////////////////////////////////////////////////////////////////
    // Data contract for document
    // NOTE: The Indigo "Message" type is used to pass the document as an
    // XML document; as such, there is no formal data contract for the document type
    //

    /////////////////////////////////////////////////////////////////////
    // Data contract for attachment metadata
    //
    [DataContract]
    public class AttachmentHeader
    {
        private string name;
        private string key; 
        private RecipeAttachmentType mediaType;

        public AttachmentHeader(string name, string key, RecipeAttachmentType mediaType)
        {
            this.name = name;
            this.key = key; 
            this.mediaType = mediaType;
        }

        [DataMember]
        public string Name
        {
            get
            {
                return name;
            }
            set
            {
                name = value;
            }
        }

        [DataMember]
        public string Key
        {
            get
            {
                return key;
            }
            set
            {
                key = value;
            }
        }

        [DataMember]
        public RecipeAttachmentType MediaType
        {
            get
            {
                return mediaType;
            }
            set
            {
                mediaType = value;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////
    // Data contract for attachment
    // NOTE: The CLR "Stream" type is used to pass the attachment; as such, 
    // there is no formal data contract for the attachment type
    //


    /////////////////////////////////////////////////////////////////////
    // Data contracts for Faults thrown by DocumentService
    //
    [DataContract]
    public class DocumentFaultException
    {
        private string storeMode;
        protected string message;
        protected string documentKey;

        public DocumentFaultException() : this("General document fault", "unspecified document")
        {
        }

        public DocumentFaultException(string msg, string key)
        {
            storeMode = DocumentServiceState.Mode.ToString();
            message = msg;
            documentKey = key;
        }

        [DataMember]
        public string StoreMode
        {
            get
            {
                return storeMode;
            }
            set
            {
                storeMode = value;
            }
        }

        [DataMember]
        public string Message
        {
            get
            {
                return message;
            }
            set
            {
                message = value;
            }
        }
        [DataMember]
        public string DocKey
        {
            get
            {
                return documentKey;
            }
            set
            {
                documentKey = value;
            }
        }
    }

    [DataContract]
    public class NoDocumentsFoundFaultException : DocumentFaultException
    {
        private string searchKey;

        public NoDocumentsFoundFaultException(string searchKey, string message)
            : base(message, "n/a")
        {
            this.searchKey = searchKey;
        }

        [DataMember]
        public string SearchKey
        {
            get
            {
                return searchKey;
            }
            set
            {
                searchKey = value;
            }
        }
    }

    [DataContract]
    public class DocumentNotFoundFaultException : DocumentFaultException
    {
        public DocumentNotFoundFaultException()
            : base()
        {
        }
        public DocumentNotFoundFaultException(string docKey, string message)
            : base(message, docKey)
        {
        }

    }

    [DataContract]
    public class UnableToAddDocumentFaultException : DocumentFaultException
    {
        public UnableToAddDocumentFaultException()
            : base()
        {
        }

        public UnableToAddDocumentFaultException(string docKey, string message)
            : base(message, docKey)
        {
        }

    }

    [DataContract]
    public class AttachmentNotFoundFaultException : DocumentFaultException
    {
        private string attachmentKey;

        public AttachmentNotFoundFaultException(string docKey, string attachmentKey, string message) 
            : base(message, docKey)
        {
            this.attachmentKey = attachmentKey;
        }

        [DataMember]
        public string AttachmentKey
        {
            get
            {
                return attachmentKey;
            }
            set
            {
                attachmentKey = value;
            }
        }
    }
}
