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
using System.Runtime.InteropServices;
using System.Text;
using System.Xml;
using System.Data.SqlClient;
using System.Data.OleDb;
using System.Windows.Forms;
using Microsoft.Samples.RecipeCatalog;

namespace Microsoft.Samples.RecipeCatalog.ShellStore
{

    public class RecipeCollection : IRecipeCollection
    {
        //The seperator to use for property names
        private const string c_Sep = "/";

        private List <IRecipe> m_RecipeList;
        private Dictionary<String,IRecipe> m_RecipeDictionary;
        private int m_CurrPos;
        private int m_MaxPos;
        private OleDbConnection indexConnection;

        public RecipeCollection()
            : this(null)
        {
        }

        public RecipeCollection(string[] keywordList)
        {
            //Create a new connect to the search index database
            indexConnection = new OleDbConnection("provider=USearch.CollatorDSO.1;EXTENDED PROPERTIES=\"Application=Windows\"");
            indexConnection.Open();

            GetRecipeFiles(keywordList);
        }

        #region Private Methods

        private void GetRecipeFiles(string[] keywordList)
        {
            string query = "";

            if (keywordList != null)
            {
                if (keywordList.Length > 0)
                {
                    StringBuilder queryBuilder = new StringBuilder();

                    queryBuilder.Append("AND \"System" + c_Sep + "Keywords\" = SOME ARRAY [");

                    int currentIndex = 0;

                    foreach (string keyword in keywordList)
                    {
                        if (currentIndex > 0)
                        {
                            queryBuilder.Append(",");
                        }

                        queryBuilder.Append("'" + keyword.Replace("'", "") + "'");

                        currentIndex++;
                    }

                    queryBuilder.Append("]");

                    query = queryBuilder.ToString();
                }
            }

            //Load the file list from the store
            RunSearchQuery(query);

            //Reset the enumeration
            ResetFileList();
        }

        private void RunSearchQuery(string query)
        {
            m_RecipeList = new List<IRecipe>();
            m_RecipeDictionary = new Dictionary<string, IRecipe>();

            OleDbCommand catCMD = indexConnection.CreateCommand();
            catCMD.CommandText = "Select Path,\"System" + c_Sep + "Title\",\"System" + c_Sep + "Rating\",\"System" + c_Sep + "Keywords\" From SystemIndex..SCOPE() Where Path Like '%.recipe' " + query;

            OleDbDataReader myReader = catCMD.ExecuteReader();

            while (myReader.Read())
            {
                //Get the information for each recipe item and add a new recipe object to the list
                string rLongPath = myReader.GetString(0);

                string rPath=rLongPath.Remove(0,5).Replace("/","\\");

                Console.WriteLine(rPath);

                string rTitle = myReader.GetString(1);

                int rRating = 0;

                try
                {
                    rRating = myReader.GetInt32(2);
                }
                catch (InvalidCastException)
                {
                }

                string[] rKeywords = new string[0];

                try
                {
                    rKeywords = (string[])myReader.GetValue(3);
                }
                catch (InvalidCastException)
                {
                }

                RecipeEntry newRecipe = new RecipeEntry(rPath, rTitle, rRating, rKeywords);

                m_RecipeList.Add(newRecipe);
                m_RecipeDictionary.Add(newRecipe.Key, newRecipe);
            }

            myReader.Close();
        }

        private void ResetFileList()
        {
            m_CurrPos = -1;
            m_MaxPos = m_RecipeList.Count - 1;
        }
        #endregion


        #region IRecipeCollection Methods

        public string AddRecipe(XmlReader recipeDoc)
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


            string dirPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments) + @"\My Recipes";
            DirectoryInfo RecipeDirectory = new DirectoryInfo(dirPath);

            // Set up the file path for the new file
            string recipeFilename = Microsoft.Samples.RecipeCatalog.LocalStore.NativeMethods.GetUniqueFilename(RecipeDirectory.FullName, recipeTitle + ".xml");
            FileInfo recipeFile = new FileInfo(recipeFilename);

            // Save the contents of the XML dom to the new file
            dom.Save(recipeFile.FullName);
            ResetFileList();

            return recipeFile.FullName;
        }

        public IRecipeCollection Find(string[] keyword)
        {
            return (new RecipeCollection(keyword));
        }

        public IRecipe this[string key]
        {
            get
            {
                return m_RecipeDictionary[key];
            }
        }

        #endregion

        #region IRecipeCollection Properties

        // Number of recipes in collection
        int IRecipeCollection.Length
        {
            get
            {
                return m_RecipeList.Count;
            }
        }
        #endregion


        #region IEnumerable Methods

        IEnumerator IEnumerable.GetEnumerator()
        {
            return (IEnumerator<IRecipe>)this;
        }

        #endregion

        #region IEnumerator Methods

        void IEnumerator.Reset()
        {
            ResetFileList();
        }

        bool IEnumerator.MoveNext()
        {
            bool ret = false;

            if (m_CurrPos < m_MaxPos)
            {
                m_CurrPos++;
                ret = true;
            }

            return ret;
        }

        IRecipe IEnumerator<IRecipe>.Current
        {
            get
            {
                IRecipe entry = null;
                if (m_CurrPos != -1)
                {
                    entry = m_RecipeList[m_CurrPos];
                }

                return entry;
            }
        }

        object IEnumerator.Current
        {
            get
            {
                IRecipe entry = null;
                if (m_CurrPos != -1)
                {
                    entry = m_RecipeList[m_CurrPos];
                }

                return entry;
            }
        }

        #endregion

        #region IDisposable Methods

        void IDisposable.Dispose()
        {
            indexConnection.Close();
        }

        #endregion

    }

    public class RecipeEntry : IRecipe
    {
        private string m_Title = "";
        private int m_Rating;
        private string[] m_Keywords;
        private XmlDocument m_RecipeXml;
        private string m_Key = "";
        private string m_Path = "";
        private RecipeAttachmentCollection m_Attachments;

        public RecipeEntry(string path, string title, int rating, string[] keywords)
        {
            m_Title = title;
            m_Rating = rating;
            m_Keywords = keywords;
            m_Path = path;

            //Determine the key
            int slashPostion = m_Path.LastIndexOf("\\");
            m_Key = m_Path.Substring(slashPostion + 1);
        }

        #region Properties
        public string Title
        {
            get { return m_Title; }
        }

        public int Rating
        {
            get { return m_Rating; }
        }

        public string[] Keywords
        {
            get { return m_Keywords; }
        }

        public XmlDocument RecipeXml
        {
            get
            {
                if (m_RecipeXml == null)
                {
                    m_RecipeXml = new XmlDocument();
                    m_RecipeXml.Load(m_Path);
                }

                return m_RecipeXml;
            }
        }

        public string Key
        {
            get { return m_Key; }
        }

        public IRecipeAttachmentCollection Attachments
        {
            get
            {
                if (m_Attachments == null)
                {
                    m_Attachments = new RecipeAttachmentCollection();

                }

                return (m_Attachments);
            }
        }
        #endregion
    }

    public class RecipeAttachment : IRecipeAttachment
    {
        private RecipeAttachmentType m_Type;
        private string m_Name;
        private string m_Key;
        private string m_Path;

        public RecipeAttachment(RecipeAttachmentType type,
                                string name,
                                string key,
                                string path)
        {
            m_Name = name;
            m_Type = type;
            m_Key = key;
            m_Path = path;
        }

        public RecipeAttachmentType Type
        {
            get
            {
                return m_Type;
            }
        }

        public string Name
        {
            get
            {
                return m_Name;
            }
        }

        public string Key
        {
            get
            {
                return m_Key;
            }
        }

        public Stream AttachmentStream
        {
            get
            {
                //  Validate current state
                if (m_Path == null || m_Path.Length == 0)
                {
                    throw new InvalidOperationException();
                }

                //  Open the stream. If the file does not exist, the constructor
                //  will throw the appropriate exception
                FileStream theStream = new FileStream(m_Path, FileMode.Open, FileAccess.Read);

                return theStream;
            }
        }
    }

    public class RecipeAttachmentCollection : List<IRecipeAttachment>, IRecipeAttachmentCollection
    {
        public IRecipeAttachment this[string key]
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

        #region IRecipeAttachmentCollection Properties

        // Number of attachments in attachment collection
        public int Length
        {
            get
            {
                return this.Count;
            }
        }
        #endregion
    }
}