// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Microsoft.Samples.RecipeCatalog;

namespace Microsoft.Samples.RecipeCatalog.LocalStore
{
    public class RecipeAttachment : IRecipeAttachment
    {
        #region Fields

        private RecipeAttachmentType _Type;
        private String _Name;
        private String _Key;
        private String _Source;

        #endregion Fields

        public RecipeAttachment(RecipeAttachmentType type,
                                String name,
                                String key,
                                String source)
        {
            _Name = name;
            _Type = type;
            _Key = key;
            _Source = source;
        }

        RecipeAttachmentType IRecipeAttachment.Type
        {
            get
            {
                return _Type;
            }
        }

        String IRecipeAttachment.Name
        {
            get
            {
                return _Name;
            }
        }

        String IRecipeAttachment.Key
        {
            get
            {
                return _Key;
            }
        }

        Stream IRecipeAttachment.AttachmentStream
        {
            get
            {
                //  Validate current state

                if (_Source == null || _Source.Length == 0)
                {
                    throw new InvalidOperationException();
                }

                //  We assume that the _Source is a path relative
                //  to the recipe directory

                string path = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)
                            + @"\My Recipes\"
                            + _Source;

                //  Open the stream. If the file does not exist, the constructor
                //  will throw the appropriate exception

                FileStream theStream = new FileStream(path, FileMode.Open, FileAccess.Read);

                return theStream;
            }
        }
    }

    public class RecipeAttachmentCollection : List<IRecipeAttachment>, IRecipeAttachmentCollection
    {
        IRecipeAttachment IRecipeAttachmentCollection.this[String key]
        {
            get
            {
                foreach (IRecipeAttachment item in this)
                {
                    if (item.Key == key)
                    {
                        return item;
                    }
                }

                return null;
            }
        }

        #region IRecipeAttachmentCollection properties

        // Number of attachments in attachment collection
        int IRecipeAttachmentCollection.Length
        {
            get
            {
                return this.Count;
            }
        }
        #endregion IRecipeAttachmentCollection properties
    }

    /// <summary>
    /// RecipeListEntry
    /// Encapsulates an entry in the Recipe cache
    /// </summary>
    public class RecipeListEntry : IRecipe
    {
        #region fields

        private String recipeName;
        private int recipeRating;
        private String filePath;
        private List<String> keywords;
        private RecipeAttachmentCollection attachments;

        #endregion fields

        public RecipeListEntry(FileInfo recipeFile)
        {
            keywords = new List<String>();
            attachments = new RecipeAttachmentCollection();
            filePath = recipeFile.FullName;
            XmlReaderSettings settings = new XmlReaderSettings();

            using (XmlReader recipeXml = XmlReader.Create(recipeFile.FullName, settings))
            {
                try
                {
                    //  Retrieve the recipe title
                    recipeXml.ReadStartElement("Recipe");
                    if (recipeXml.ReadToFollowing("Title"))
                    {
                        recipeXml.MoveToContent();
                        recipeName = recipeXml.ReadElementContentAsString();
                    }

                    //  Retrieve the keywords
                    if (recipeXml.ReadToNextSibling("RecipeKeywords"))
                    {
                        bool success = recipeXml.ReadToDescendant("Keyword");
                        while (success)
                        {
                            recipeXml.MoveToContent();
                            keywords.Add(recipeXml.ReadElementContentAsString());
                            success = recipeXml.ReadToNextSibling("Keyword");
                        }
                    }

                    //  Retrieve the rating
                    if (recipeXml.ReadToFollowing("Rating"))
                    {
                            recipeXml.MoveToContent();
                            recipeRating = (int)recipeXml.ReadElementContentAs(typeof(int), null);
                    }

                    //  Retrieve the Attachments
                    if (recipeXml.ReadToFollowing("Attachments"))
                    {
                        String name = null;
                        String source = null;
                        RecipeAttachmentType type = RecipeAttachmentType.Unknown;

                        bool success = recipeXml.Read();
                        while (success && recipeXml.NodeType != XmlNodeType.EndElement)
                        {
                            if (recipeXml.NodeType == XmlNodeType.Element)
                            {
                                if (recipeXml.Name == "Picture")
                                {
                                    type = RecipeAttachmentType.Photo;
                                }
                                else if (recipeXml.Name == "Video")
                                {
                                    type = RecipeAttachmentType.Video;
                                }
                                else
                                {
                                    type = RecipeAttachmentType.Unknown;
                                }

                                if (type != RecipeAttachmentType.Unknown)
                                {
                                    bool gotAttr = recipeXml.MoveToNextAttribute();
                                    while (gotAttr)
                                    {
                                        if (recipeXml.Name == "Name")
                                        {
                                            name = recipeXml.Value.ToString();
                                        }
                                        else if (recipeXml.Name == "Source")
                                        {
                                            source = recipeXml.Value.ToString();
                                        }

                                        gotAttr = recipeXml.MoveToNextAttribute();
                                    }

                                    RecipeAttachment attachment = new RecipeAttachment(type,
                                                                                       name,
                                                                                       source,
                                                                                       source);
                                    attachments.Add(attachment);
                                }
                            }

                            success = recipeXml.Read();
                        }
                   }
                }
                catch (XmlException)
                {
                    // TODO: what should we do with XML exceptions
                }
            }
        }

        #region IRecipe methods

        String IRecipe.Title
        {
            get
            {
                return recipeName;
            }
        }

        int IRecipe.Rating
        {
            get
            {
                return recipeRating;
            }
        }

        String[] IRecipe.Keywords
        {
            get
            {
                return keywords.ToArray();
            }
        }

        XmlDocument IRecipe.RecipeXml
        {
            get
            {
                XmlDocument recipe = new XmlDocument();
                recipe.Load(filePath);

                return recipe;
            }
        }

        string IRecipe.Key
        {
            get
            {
                int pos = filePath.LastIndexOf("\\");
                return filePath.Substring(pos + 1);
            }
        }

        IRecipeAttachmentCollection IRecipe.Attachments
        {
            get
            {
                return attachments;
            }
        }

        #endregion IRecipe methods
    }

    public class RecipeCollection : IRecipeCollection
    {
        #region fields

        private DirectoryInfo recipeDirectory;
        private FileInfo[] recipeFileList;
        private int currentPosition;
        private int maximumPosition;

        #endregion fields

        public RecipeCollection()
        {
            string dirPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments) + @"\My Recipes";
            recipeDirectory = new DirectoryInfo(dirPath);

            //  Ensure that the directory exists
            recipeDirectory.Create();
            ResetFileList();
        }

        #region IEnumerable methods

        IEnumerator IEnumerable.GetEnumerator()
        {
            return (IEnumerator<IRecipe>)this;
        }

        #endregion IEnumerable methods

        #region IEnumerator methods

        void IEnumerator.Reset()
        {
            ResetFileList();
        }

        bool IEnumerator.MoveNext()
        {
            bool ret = false;

            if (currentPosition < maximumPosition)
            {
                currentPosition++;
                ret = true;
            }

            return ret;
        }

        IRecipe IEnumerator<IRecipe>.Current
        {
            get
            {
                RecipeListEntry entry = null;
                if (currentPosition != -1)
                {
                    entry = new RecipeListEntry(recipeFileList[currentPosition]);
                }

                return (IRecipe)entry;
            }
        }

        object IEnumerator.Current
        {
            get
            {
                RecipeListEntry entry = null;
                if (currentPosition != -1)
                {
                    entry = new RecipeListEntry(recipeFileList[currentPosition]);
                }

                return entry;
            }
        }

        #endregion IEnumerator methods

        #region IDisposable methods

        void IDisposable.Dispose()
        {       
            //  No unmanaged resources.
        }

        #endregion IDisposable methods

        #region IRecipeCollection properties

        // Number of recipes in collection
        int IRecipeCollection.Length
        {
            get
            {
                return recipeFileList.Length;
            }
        }
        #endregion IRecipeCollection properties

        #region IRecipeCollection methods

        string IRecipeCollection.AddRecipe(XmlReader recipeDoc)
        {
            //  Load the XML from the reader into a DOM
            XmlDocument dom = new XmlDocument();
            dom.Load(recipeDoc);

            // Create an XML declaration node and insert it as the root node 
            XmlDeclaration xmldecl = dom.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            dom.InsertBefore(xmldecl, dom.DocumentElement);

            // Retrieve the recipe title
            XmlNode node = dom.SelectSingleNode("//Title");
            string recipeTitle = node.InnerText;

            // Set up the file path for the new file
            string recipeFilename = NativeMethods.GetUniqueFilename(recipeDirectory.FullName, recipeTitle + ".xml");
            FileInfo recipeFile = new FileInfo(recipeFilename);

            // Save the contents of the XML dom to the new file
            dom.Save(recipeFile.FullName);
            ResetFileList();

            return recipeFile.FullName;
        }

        IRecipeCollection IRecipeCollection.Find(string[] keyword)
        {
            throw new NotImplementedException();
        }

        IRecipe IRecipeCollection.this[String key]
        {
            get
            {
                //  Simple implementation: Linear search of file list by name

                foreach (FileInfo fInfo in recipeFileList)
                {
                    if (fInfo.Name == key)
                    {
                        return new RecipeListEntry(fInfo);
                    }
                }

                return null;
            }
        }

        #endregion IRecipeCollection methods

        #region private methods

        private void ResetFileList()
        {
            recipeFileList = recipeDirectory.GetFiles("*.xml");
            currentPosition = -1;
            maximumPosition = recipeFileList.Length - 1;
        }
        #endregion private methods
    }
}