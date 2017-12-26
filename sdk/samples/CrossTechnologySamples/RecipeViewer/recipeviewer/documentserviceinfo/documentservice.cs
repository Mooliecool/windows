// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections.Generic;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.IO;
using System.Text;
using System.Xml;

namespace Microsoft.Samples.RecipeCatalog
{
    public enum StoreMode
    {
        Private,
        Shell,
        Shared
    }

    public class DocumentServiceState
    {
        // mode = "shared" : service is running in a shared server environment 
        // mode = "private": service is running in a single-user stand-alone environment 
        // mode = "shell"  : service is retrieving data from the shell indexer (Windows Vista only)

        private static StoreMode mode = StoreMode.Private;

        static DocumentServiceState()
        {
            // Get mode from app.config; force to "local" mode if config is invalid
            string modeSetting = (ConfigurationManager.AppSettings["storeMode"]).ToLower();

            switch (modeSetting)
            {
                case "shared":
                    mode = StoreMode.Shared;
                    break;
                case "private":
                    //NOTE: When enabled, the following code "sniffs" for Vista, and if found, 
                    //would use the ShellRecipeStore, which in turns uses Vista's shell property handler
                    //system to fetch recipe data.
                    //if (Environment.OSVersion.Version.Major >= 6)
                    //{
                    //    //Windows Vista
                    //    mode = StoreMode.Shell;
                    //}
                    //else
                    //{
                    //    //Windows XP or lower
                    //    mode = StoreMode.Private;
                    //}
                    mode = StoreMode.Private;
                    break;
                case "shell":
                    mode = StoreMode.Shell;
                    break;
                default:
                    throw new ApplicationException("invalid 'storeMode' value in serivce configuration: " + modeSetting);
            }
        }

        public static StoreMode Mode
        {
            get 
            {
                return mode;
            }
        }
    }
	public class DocumentService : IDocumentService
	{
        // Reference to the local/shared store
		private IRecipeCollection store;

		public DocumentService()
		{
			if (DocumentServiceState.Mode == StoreMode.Private)
			{
				try
				{
					store = new LocalStore.RecipeCollection();
				}
				catch (Exception ex)
				{
					throw new ApplicationException("\"Private\" store initialization failed", ex);
				}
			}

            if (DocumentServiceState.Mode == StoreMode.Shell)
            {
                try
                {
                    store = new ShellStore.RecipeCollection();
                }
                catch (Exception ex)
                {
                    throw new ApplicationException("\"Shell\" store initialization failed", ex);
                }
            }

            if (DocumentServiceState.Mode == StoreMode.Shared)
			{
                throw new NotImplementedException("\"Shared\" store is not implemented");
			}
		}

        public DocHeader[] GetDocHeaders(string searchKey)
		{
			int i = 0;
			DocHeader[] docs=null;

            // Refresh the local/shared store collection
            // TODO - use key to find matching docs first via _recipeCollection.Search(key);
            // Currently, we're returning everything in the store
            store.Reset();
            if (store.Length > 0)
            {
                // If we have documents, iterate over the collection and build a DocHeader array
                docs = new DocHeader[store.Length];
                foreach (IRecipe recipe in store)
                {
                    // If this doc has attachments, iterate the collection of attachments and build a AttachmentHeader array
                    AttachmentHeader[] attachments = null;
                    if (recipe.Attachments.Length > 0)
                    {
                        int j = 0;
                        attachments = new AttachmentHeader[recipe.Attachments.Length];
                        foreach (IRecipeAttachment attachment in recipe.Attachments)
                        {
                            attachments[j++] = new AttachmentHeader(attachment.Name,
                                                                    attachment.Key,
                                                                    attachment.Type);

                        }
                    }

                    // Fill each element of the DocHeader array with a new DocHeader
                    docs[i++] = new DocHeader(recipe.Title,
                                              recipe.Keywords,
                                              recipe.Rating,
                                              recipe.Key,
                                              attachments);
                }
            }
            else
            {
                NoDocumentsFoundFaultException fault = new NoDocumentsFoundFaultException(searchKey, "No recipes found");
                throw new FaultException<NoDocumentsFoundFaultException>(fault,
                                                       new FaultReason("No recipes found"));
            }

			return docs;
		}

        public Message GetDoc(Message msgRequest)
        {
            Message msgResponse = null;
            string key = null;

            // Pull the document key from request Message
            try
            {
                XmlReader body = msgRequest.GetReaderAtBodyContents();
                body.Read();
                key = body.ReadString().Trim();
                msgRequest.Close();
            }
            catch (Exception ex)
            {
                DocumentNotFoundFaultException fault = new DocumentNotFoundFaultException(key, ex.Message);
                throw new FaultException<DocumentNotFoundFaultException>(fault,
                                                  new FaultReason("Document contains invalid XML"));
            }

            // Pull the document from the store using the document key
            IRecipe recipe = store[key];
            if (recipe != null)
            {
                // Create the response message
                try
                {
                    XmlNodeReader reader = new XmlNodeReader(recipe.RecipeXml);
                    msgResponse = Message.CreateMessage(MessageVersion.Default, "http://Microsoft.Samples.RecipeCatalog/IDocumentService/GetDocResponse",
                                    reader);
                }
                catch (Exception ex)
                {
                    DocumentNotFoundFaultException fault = new DocumentNotFoundFaultException(key, ex.Message);
                    throw new FaultException<DocumentNotFoundFaultException>(fault,
                                                      new FaultReason("Error retrieving the document"));
                }
            }
            else
            {
                DocumentNotFoundFaultException fault = new DocumentNotFoundFaultException(key, "Document does not exist in store");
                throw new FaultException<DocumentNotFoundFaultException>(fault,
                                                  new FaultReason("Document does not exist in store"));
            }

            return msgResponse;
        }

        public Stream GetAttachment(string docKey, string attachmentKey)
        {
            // Pull the requested attachment, from the requested document in the store
            Stream stream = null;
            try
            {
                stream = store[docKey].Attachments[attachmentKey].AttachmentStream;
            }
            catch(Exception ex)
            {
                AttachmentNotFoundFaultException fault = new AttachmentNotFoundFaultException(docKey, attachmentKey, ex.Message);
                throw new FaultException<AttachmentNotFoundFaultException>(fault,
                                                         new FaultReason("Problem accessing attachment"));
            }

            if (stream == null)
            {
                AttachmentNotFoundFaultException fault = new AttachmentNotFoundFaultException(docKey, attachmentKey, "Store returned null stream");
                throw new FaultException<AttachmentNotFoundFaultException>(fault,
                                                         new FaultReason("Empty attachment"));
            }

            return stream;
        }

        public Message AddDoc(Message msgRequest)
        {
            string key = "";
            try
            {
                // Pull the document from request Message 
                XmlReader body = msgRequest.GetReaderAtBodyContents();

                // Add document to store
                key = store.AddRecipe(body);
                msgRequest.Close();

                // Create the response message
                Message msgResponse = Message.CreateMessage(MessageVersion.Default,"http://Microsoft.Samples.RecipeCatalog/IDocumentService/AddDocResponse",
                                key);

                return msgResponse;
            }
            catch (Exception ex)
            {

                UnableToAddDocumentFaultException fault = new UnableToAddDocumentFaultException(key, ex.Message);
                throw new FaultException<UnableToAddDocumentFaultException>(fault,
                                                         new FaultReason("Error adding document"));

            }

        }

        
	}
}
