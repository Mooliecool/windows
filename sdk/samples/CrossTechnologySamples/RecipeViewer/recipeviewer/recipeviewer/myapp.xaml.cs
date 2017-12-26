// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Windows;
using System.Windows.Navigation;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.Data;
using System.Xml;
using System.IO;
using System.Collections;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ComponentModel;
using System.Collections.Generic;
using Microsoft.Samples.RecipeCatalog;

namespace Microsoft.Samples.RecipeCatalog.RecipeViewer
{
    // Interaction logic for the Recipe Viewer App
    public partial class RecipeViewerApp : Application
    {
        private DocumentServiceProxy proxy = null;
        private Recipes recipeList;
        private UInt32 currentIndex;

        //  Connect to the Indigo service and retrieve the list of recipes
        public void OnAppStartup(object sender, StartupEventArgs e)
        {
            //  Set up the Indigo service to the local store.
            proxy = new DocumentServiceProxy();

            //  Initialize the recipe list
            DocHeader[] headers;

            try
            {
                headers = proxy.GetDocHeaders("");

                if ((headers != null) && (headers.Length > 0))
                {
                    recipeList = new Recipes(headers);
                    currentIndex = 0;

                    //  Add the application specific properties
                    Properties.Add("RecipeList", recipeList);
                    Properties.Add("CurrentRecipe", currentIndex);
                }
                else
                {
                    throw new ApplicationException("No documents found");
                }
            }
            catch (FaultException<NoDocumentsFoundFaultException> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (FaultException<string> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (Exception ex)
            {
                DocumentServiceHelpers.HandleException(ex);
            }
        }

        //  Clean up
        public void OnAppExit(object sender, ExitEventArgs e)
        {
            //  Shut down the Indigo proxy
            if (proxy != null)
            {
                proxy.Close();
            }
        }

        //  Retrieve the recipe identified by "key"
        //  from the Indigo service

        public XmlDocument GetRecipe(String key)
        {
            XmlDocument recipeDoc = null;
            Message query = null;
            Message response = null;
            String action = "http://Microsoft.Samples.RecipeCatalog/IDocumentService/GetDoc";

            try
            {
                // Create the request message using the doc key; a document will be returned in the reply Message
                query = Message.CreateMessage(MessageVersion.Default, action, key);
                response = proxy.GetDoc(query);

                // Process the response message
                XmlReader rdr = response.GetReaderAtBodyContents();
                if (!rdr.EOF)
                {
                    // The service returned a document
                    if (!response.IsFault)
                    {
                        recipeDoc = new XmlDocument();
                        recipeDoc.Load(rdr);
                    }
                    // The service returned a fault
                    else
                    {
                        DocumentServiceHelpers.ProcessMessageFault(rdr);
                    }
                }
                // Response message came back empty
                else
                {
                    throw new Exception("Response message is empty");
                }
            }
            catch (FaultException<DocumentNotFoundFaultException> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (FaultException<string> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (FaultException f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (Exception e)
            {
                DocumentServiceHelpers.HandleException(e);
            }
            finally
            {
                if (query != null)
                    query.Close();
                if (response != null)
                    response.Close();
            }

            return recipeDoc;
        }

        //  Retrieve the stream containing the specified attachment
        //  from the Indigo service.
        public Stream GetRecipeAttachment(String docKey, String attachmentKey)
        {
            Stream stream = null;

            try
            {
                stream = proxy.GetAttachment(docKey, attachmentKey);
                if (stream == null)
                {
                    throw new Exception("Attachment not found");
                }
#if true //TODO: SeptCTP "bug" - unable to use WCF stream with BitmapImage.StreamSource
                else
                {
                    List<byte> bytesList = new List<byte>();
                    int i = stream.ReadByte();
                    while (i != -1)
                    {
                        bytesList.Add((byte)i);
                        i = stream.ReadByte();
                    }
                    stream = new MemoryStream(bytesList.ToArray());
                }
#endif
            }
            catch (FaultException<AttachmentNotFoundFaultException> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (FaultException<string> f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (FaultException f)
            {
                DocumentServiceHelpers.HandleException(f);
            }
            catch (Exception e)
            {
                DocumentServiceHelpers.HandleException(e);
            }

            return stream;
        }
    }

    //  Encapsulates the list of recipies retrieved from the Indigo Service.
    //  Deriving from ArrayListDataCollection simplifies binding to the
    //  ItemsControl in the WPF page.
    public class Recipes : System.Collections.ObjectModel.ObservableCollection<RecipeListItem>
    {
        public Recipes(DocHeader[] recipes)
            : base()
        {
            foreach (DocHeader entry in recipes)
            {
                RecipeListItem item = new RecipeListItem(entry);
                Add(item);
            }
        }

        public RecipeListItem Find(String key)
        {
            foreach (RecipeListItem item in this)
            {
                if (item.Key == key)
                {
                    return item;
                }
            }

            return null;
        }
    }

    //  Encapsulates a recipe
    public class RecipeListItem
    {
        private DocHeader item;
        private ArrayList attachmentCache;

        public RecipeListItem(DocHeader item)
        {
            this.item = item;
            attachmentCache = new ArrayList();

            //  Build the attachment cache
            if (item.Attachments != null)
            {
                foreach (AttachmentHeader header in item.Attachments)
                {
                    switch (header.MediaType)
                    {
                        case RecipeAttachmentType.Photo:
                            BitmapAttachment bitmap = new BitmapAttachment(item.Key, header);
                            attachmentCache.Add(bitmap);
                            break;

                        case RecipeAttachmentType.Video:
                            VideoAttachment video = new VideoAttachment(item.Key, header);
                            attachmentCache.Add(video);
                            break;

                        default:
                            break;
                    }
                }
            }
        }

        public String RecipeTitle
        {
            get
            {
                return item.Title;
            }
        }

        public int Rating
        {
            get
            {
                return item.Rating;
            }
        }

        public String Key
        {
            get
            {
                return item.Key;
            }
        }

        public ArrayList Attachments
        {
            get
            {
                return attachmentCache;
            }
        }
    }

    //  Base class for attachments.
    public abstract class RecipeAttachment
    {
        protected bool contentCached;
        private AttachmentHeader header;
        private String documentKey;

        public RecipeAttachment(String docKey, AttachmentHeader header)
        {
            this.header = header;
            documentKey = docKey;
            contentCached = false;
        }

        public String Name
        {
            get
            {
                return header.Name;
            }
        }

        public abstract Object Content
        {
            get;
        }

        protected Stream GetStream()
        {
            RecipeViewerApp app = (RecipeViewerApp)Application.Current;

            return app.GetRecipeAttachment(documentKey, header.Key);
        }
    }

    //  Class for attachments that are bitmaps
    //  Caches the bitmap once it has been retrieved.
    public class BitmapAttachment : RecipeAttachment
    {
        private BitmapImage bitmap;

        public BitmapAttachment(String docKey, AttachmentHeader header)
            : base(docKey, header)
        {
            bitmap = null;
        }

        public override Object Content
        {
            get
            {
                if (!contentCached)
                {
                    Stream stream = GetStream();
                    if (stream != null)
                    {
                        bitmap = new BitmapImage();
                        bitmap.BeginInit();
                        bitmap.StreamSource = stream;
                        bitmap.EndInit();
                    }
                }

                contentCached = true;
                return bitmap;
            }
        }
    }


    //  Class for attachments that are Videos
    public class VideoAttachment : RecipeAttachment
    {

        public VideoAttachment(String docKey, AttachmentHeader header)
            : base(docKey, header)
        {
        }

        public override Object Content
        {
            get
            {
                throw new NotImplementedException();
            }
        }
    }

    // Converts decimal representations of recipe quantities
    // to fractional representations. (e.g. .75 = 3/4)
    // The smallest fraction we use is 1/8. If the value is not an even
    // 1/8th we round.
    public class QuantityConverter : System.Windows.Data.IValueConverter
    {
        object IValueConverter.Convert(object obj,
                                       Type type,
                                       object param,
                                       System.Globalization.CultureInfo culture)
        {
            //  Separate the integer part and the fractionpart into two variables
            double value = Convert.ToDouble(obj);
            int intPart = Convert.ToInt32(Math.Floor(value));
            double fractionPart = Math.IEEERemainder(value, 1.0);
            String ret;

            if (fractionPart == 0)
            {
                ret = intPart.ToString();
            }
            else
            {
                double denominator;
                double numerator;

                if (Math.Sign(fractionPart) == -1)
                {
                    fractionPart += 1.0;
                }

                //  Test for 1/3rds first
                if (fractionPart > 0.29 && fractionPart < 0.35)
                {
                    denominator = 3;
                    numerator = Math.Round(fractionPart * denominator);
                }
                else
                {
                    //  Convert to 1/8ths
                    denominator = 8;
                    numerator = Math.Round(fractionPart * denominator);

                    //  Normalize
                    while (Math.IEEERemainder(numerator, 2.0) == 0.0)
                    {
                        denominator /= 2;
                        numerator /= 2;
                    }
                }

                //  create the return string
                if (intPart > 0)
                {
                    ret = intPart.ToString() + " " + numerator.ToString() + "/" + denominator.ToString();
                }
                else
                {
                    ret = numerator.ToString() + "/" + denominator.ToString();
                }
            }
            return ret;
        }

        object IValueConverter.ConvertBack(object obj,
                                           Type type,
                                           object param,
                                           System.Globalization.CultureInfo culture)
        {
            return System.Windows.DependencyProperty.UnsetValue;
        }
    }
}