using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Text;
using System.Xml;
using System.Windows.Forms;

namespace Microsoft.Samples.RecipeCatalog
{
    public class DocumentServiceHelpers
    {
      
        static public void ProcessMessageFault(XmlReader body)
        {
            FaultCode faultCode = null;
            FaultReason faultReason = null;
            string faultTypePrefix = "";
            string faultTypeName = "";
            string sn = "http://www.w3.org/2003/05/soap-envelope";

            // We should be on the <Fault> element; get fault <Code> info
            if (body.ReadToFollowing("Code", sn) && body.ReadToDescendant("Value", sn))
            {
                faultCode = new FaultCode(body.ReadElementContentAsString());

                // Get fault <Reason> info
                if (body.ReadToFollowing("Reason", sn) && body.ReadToDescendant("Text", sn))
                {
                    faultReason = new FaultReason(body.ReadElementContentAsString());

                    // Get fault <Detail> info
                    if (body.ReadToFollowing("Detail", sn) && body.ReadToDescendant("anyType"))
                    {
                        // Grab the fault type info from the xsi:type attribute
                        body.MoveToAttribute("type", "http://www.w3.org/2001/XMLSchema-instance");
                        string[] faultType = body.Value.Split(new Char[] { ':' });
                        if (faultType.Length == 1)
                        {
                            faultTypeName = faultType[0];
                        }
                        else
                        {
                            faultTypePrefix = faultType[0];
                            faultTypeName = faultType[1];
                        }

                        // Move back up to the <anyType> element & process the fault accordingly                       
                        body.MoveToElement();

                        switch (faultTypeName)
                        {
                            // Deal with our known faults
                            case "DocumentNotFoundFault":
                            {
                                // Rethrow the DocNotFoundFault fault
                                DocumentNotFoundFaultException fault = new DocumentNotFoundFaultException();
                                GetDocumentFaultDetail(body, faultTypePrefix, fault);
                                throw new FaultException<DocumentNotFoundFaultException>(fault,
                                                    faultReason,
                                                    faultCode);       
                            }
                            case "UnableToAddDocumentFault":
                            {
                                UnableToAddDocumentFaultException fault = new UnableToAddDocumentFaultException();
                                GetDocumentFaultDetail(body, faultTypePrefix, fault);
                                throw new FaultException<UnableToAddDocumentFaultException>(fault,
                                                    faultReason,
                                                    faultCode);
                            }
                            // Deal with string faults (Indigo throws FaultException<string>)
                            case "string":
                            {
                                // Rethrow the string fault
                                throw new FaultException<string>(body.ReadElementContentAsString(),
                                                            faultReason,
                                                            faultCode);
                            }

                            // Deal with all other faults
                            default:
                            {
                                throw new FaultException("Unknown fault");
                            }
                        }
                    }
                }
            }
            // Bad fault message
            throw new Exception("Unable to read Fault message");
        }

        static public void HandleException(Exception e)
        {
            // Get the exception type name; if it's a generic (ie: FaultException<DocumentNotFoundFault>) 
            // use the template parameter type name (ie: DocumentNotFoundFault)
            Type type = e.GetType();
            string typeName = type.Name;
            if (type.IsGenericType)
            {
                typeName = type.GetGenericArguments()[0].Name;
            }

            switch (typeName)
            {
                case "DocumentNotFoundFault":
                    {
                        FaultException<DocumentNotFoundFaultException> f = (FaultException<DocumentNotFoundFaultException>)e;

                        string msg = string.Format("Store: {0}\nDoc Key: {1}\nMessage: {2}",
                        f.Detail.StoreMode,
                        f.Detail.DocKey,
                        f.Detail.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                case "UnableToAddDocumentFault":
                    {
                        FaultException<UnableToAddDocumentFaultException> f = (FaultException<UnableToAddDocumentFaultException>)e;

                        string msg = string.Format("Store: {0}\nDoc Key: {1}\nMessage: {2}",
                        f.Detail.StoreMode,
                        f.Detail.DocKey,
                        f.Detail.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                case "NoDocumentsFoundFault":
                    {
                        FaultException<NoDocumentsFoundFaultException> f2 = e as FaultException<NoDocumentsFoundFaultException>;
                        string msg = string.Format("Store: {0}\nSearch Key: {1}\nMessage: {2}",
                        f2.Detail.StoreMode,
                        f2.Detail.SearchKey,
                        f2.Detail.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                case "AttachmentNotFoundFault":
                    {
                        FaultException<AttachmentNotFoundFaultException> f2 = e as FaultException<AttachmentNotFoundFaultException>;
                        string msg = string.Format("Store: {0}\nDoc Key: {1}\nAttachment Key: {2}\nMessage: {3}",
                        f2.Detail.StoreMode,
                        f2.Detail.DocKey,
                        f2.Detail.AttachmentKey,
                        f2.Detail.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                case "String":
                    {
                        FaultException<string> f2 = e as FaultException<string>;
                        string msg = string.Format("Fault Code: {0}\nFault Reason: {1}\nFault Detail: {2}\nFault Message: {3}",
                        f2.Code.Name,
                        f2.Reason.GetMatchingTranslation().Text,
                        f2.Detail,
                        f2.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                case "FaultException":
                    {
                        FaultException f2 = e as FaultException;
                        string msg = string.Format("Fault Code: {0}\nFault Reason: {1}\nFault Detail: {2}\nFault Message: {3}",
                        f2.Code.Name,
                        f2.Reason.GetMatchingTranslation().Text,
                        f2.Data.ToString(),
                        f2.Message);
                        MessageBox.Show(msg, "Error");
                        break;
                    }
                default :
                    {
                        if (e.InnerException != null)
                            MessageBox.Show(e.InnerException.Message, e.Message);
                        else
                            MessageBox.Show(e.Message, "Error");
                        break;
                    }
            }
        }


        static private void GetDocumentFaultDetail(XmlReader body,
                                                   string faultTypePrefix,
                                                   DocumentFaultException fault)
        {
            // Appears that the elements are serialized in the reverse order that they
            // are specificed in DocumentFault ?

            // Move to the <DocKey> element 
            if (body.ReadToDescendant(faultTypePrefix + ":DocKey"))
            {
                // Read the <DocKey>, <Message> and <StoreMode> elements
                fault.DocKey = body.ReadElementContentAsString();
                fault.Message = body.ReadElementContentAsString();
                fault.StoreMode = body.ReadElementContentAsString();
            }
            else
            {
                throw new Exception("Unable to find DocKey/Message/StoreMode elements for DocumentFault");
            }


        }
    }
}
