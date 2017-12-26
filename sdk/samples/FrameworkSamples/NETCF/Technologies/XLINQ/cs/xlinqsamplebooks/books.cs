//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
//System.Linq namespace contains all the Standard Query Operators for LINQ
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
//System.Xml.Linq namespace contains XLINQ related classes
using System.Xml.Linq;
using System.IO;
using System.Reflection;
using System.Diagnostics;
using System.Configuration;
using System.Collections.Specialized;
using System.Globalization;
namespace Microsoft.Samples.XLinq
{

    /// <summary>
    /// Class for the Books Form
    /// </summary>
    public partial class Books : Form
    {
        #region MemberVariables
        /// <summary>
        /// boolean used to track if the KeyWords for every book in the xml file has been processed and written back
        /// </summary>
        private bool KeyWordsBuilt;

        /// <summary>
        /// Maintains the list of search queries for autocomplete
        /// </summary>
        private SearchHistory searchHistory;

        #endregion

        #region Constructor
        /// <summary>
        /// Constructor
        /// </summary>
        public Books()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

        }
        #endregion

        #region HelperMethods

        /// <summary>
        /// Returns the complete path of the XML document.
        /// </summary>
        /// <returns></returns>
        private static string GetXmlFilePath()
        {
            //Assembly.GetExecutingAssembly().GetName().CodeBase return the full path of the current executing assembly
            string dir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().GetName().CodeBase);
            string name = Path.Combine(dir, "XlinqBooks.xml");
            return name;
        }

        /// <summary>
        /// Returns a HTML table with two columns: Title and Price 
        /// </summary>
        /// <param name="books"></param>
        /// <returns></returns>
        private static XElement GetBooksTable(IEnumerable<XElement> books)
        {
            //create table header
            var header = new XElement[]{
                            new XElement("th","Title"), 
                            new XElement("th", "Price")};

            //from every book, get the title and price and create a HTML row
            var rows =
                from
                    b in books
                select
                    new XElement("tr",
                                 new XElement("td",
                                              (string)b.Element("title")),
                                 new XElement("td", string.Format("{0:c}", double.Parse(b.Element("price").Value)))
                                 );


            //return HTML table by create new XElement
            return new XElement("table", new XAttribute("border", 1),
                                header,
                                rows);
        }

        /// <summary>
        /// Enumerates every book element in the XML and adds a new keywords Element to it.
        /// keywords include value of the book's style, title, Authors first and last name, subject
        /// </summary>
        private void BuildKeyWords()
        {
            //get the xml file path
            string name = GetXmlFilePath();

            //Load the xml document
            XDocument doc = XDocument.Load(name);

            //loop through every book element in the document's bookstore element.
            foreach (XElement book in doc.Elements("bookstore").Elements("book"))
            {

                //get the author element
                XElement author = book.Element("author");

                //get style attribute of the book

                string style = book.Attribute("style").Value;

                //get title value of book 
                string title = book.Element("title").Value;

                //get first name value of the book's author 
                string fname = author.Element("first-name").Value;

                //get last name value of the book's author 
                string lname = author.Element("last-name").Value;

                //get subject value of book 
                string subject = book.Element("subject").Value;

                /// keywords include value of the book's style, title, Authors first and last name, subject
                string keywords = style + " " + title + " " + fname + " " + lname + " " + subject;

                //add new keywords element to the book element
                book.Add(new XElement("keywords", keywords));
            }

            //finally save the xml back to tfile.
            doc.Save(name, SaveOptions.None);

            //set the KeyWordsBuilt boolean class member variable to true
            this.KeyWordsBuilt = true;

        }

        /// <summary>
        /// searches for books by keywords element
        /// </summary>
        private void UnifiedSearch()
        {
            //get the xml file path
            string name = GetXmlFilePath();

            //Load the xml document
            XDocument doc = XDocument.Load(name);

            //case insensitive search
            string searchQuery = this.cbUnifiedSearch.Text.ToLower(CultureInfo.CurrentCulture).Trim();

            //add the search entry to the  Search History cache (used to autocomplete)
            if (!string.IsNullOrEmpty ( searchQuery))
                this.searchHistory.AddEntry(searchQuery);

            //XLINQ query to get the list of books that satisfy the keyword search criteria
            var q = from book in doc.Element("bookstore").Elements()
                    where book.Element("keywords").Value.ToLower().Contains(searchQuery)
                    select new XElement("p", GetBookDetails(book));

            //create html, so we can display the results in the web browser control            
            XElement html = new XElement("html",
               from v in q
               select v
               );

            //set the web browser's DocumentText property
            this.wbUnifiedSearchResults.DocumentText = html.ToString();
        }

        /// <summary>
        /// Return HTML table with following details of the Book
        ///     1)Book Title
        ///     2)Book Subject
        ///     3)Book Author (First Name + Last Name)
        ///     4)Book Style
        ///     5)Book Price
        /// </summary>
        /// <param name="b"></param>
        /// <returns></returns>
        private static XElement GetBookDetails(XElement b)
        {
            
            //create the HTML table contents.
            XElement table =
                new XElement("table", new XAttribute("border", 1),
                    new XElement("tr",
                                 new XElement("td", "Title"),
                                 new XElement("td", b.Element("title").Value)
                                 ),
                    new XElement("tr",
                                 new XElement("td", "Subject"),
                                 new XElement("td", b.Element("subject").Value)
                                 ),
                   new XElement("tr",
                                 new XElement("td", "Author"),
                                 new XElement("td", b.Element("author").Element("first-name").Value
                                 + " " + b.Element("author").Element("last-name").Value)
                                 ),
                   new XElement("tr",
                                 new XElement("td", "Style"),
                                 new XElement("td", b.Attribute("style").Value)
                                 ),
                    new XElement("tr",
                                 new XElement("td", "Price"),
                                 new XElement("td", string.Format(CultureInfo.CurrentCulture,"{0:c}", double.Parse(b.Element("price").Value, CultureInfo.CurrentCulture)))
                                 )
                              );


            return table;
        }

        /// <summary>
        /// Returns HTML results based on the appropriate groupBy condition
        /// </summary>
        /// <param name="groupBy"></param>
        /// <returns></returns>
        private static XElement GetHTML(string groupBy)
        {
            //get the xml file path
            string name = GetXmlFilePath();

            //Load the xml document
            XDocument doc = XDocument.Load(name);

            IEnumerable<XElement> q = null;


            //call different XLINQ queries based on the group by condition
            switch (groupBy.ToLower(CultureInfo.CurrentCulture))
            {
                    //if group by condition is Author, group all books by different Author's First name + " " + Last name
                case "author":
                    q = from b in doc.Descendants("book")
                        from a in b.Elements("author")
                        group b by (string)a.Element("first-name") + " " + (string)a.Element("last-name") into authorGroup
                        select new XElement("p", "Author: " + authorGroup.Key,
                                           GetBooksTable(authorGroup));
                    break;
                    //if group by condition is Subject, group all books by different subjects
                case "subject":
                    q = from b in doc.Descendants("book")
                        from s in b.Elements("subject")
                        group b by s.Value into subjectGroup
                        select new XElement("p", "Subject: " + subjectGroup.Key,
                                           GetBooksTable(subjectGroup));
                    break;
                    //if group by condition is Style, group all books by the different styles
                case "style":
                    q = from b in doc.Descendants("book")
                        group b by b.Attribute("style") into styleGroup
                        select new XElement("p", "Style: " + styleGroup.Key,
                                           GetBooksTable(styleGroup));
                    break;
            }

            //return HTML content by wrapping the contents of the query inside HTML section
            XElement html = new XElement("html",
                from v in q
                select v
                );


            return html;

        }

        #endregion

        #region EventHandlers

        /// <summary>
        /// Click event handler for btnSearch 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSearch_Click(object sender, EventArgs e)
        {
            //set the Current Cursor to WaitCursor
            Cursor.Current = Cursors.WaitCursor;

            //get the xml file path
            string name = GetXmlFilePath();

            //case insensitive search
            string searchQuery = this.txtBookName.Text.ToLower(CultureInfo.CurrentCulture).Trim();

            //Load the xml document
            XDocument doc = XDocument.Load(name);

            //XLINQ query to search books based on the title of the book.
            //this query returns IEnumerable<Book>
            var query = from book in doc.Element("bookstore").Elements()
                        where book.Element("title").Value.ToLower().Contains(searchQuery )
                        select new Book
                        {
                            Title = book.Element("title").Value,
                            Author = new Author
                            {
                                FirstName = book.Element("author").Element("first-name").Value,
                                LastName = book.Element("author").Element("last-name").Value,
                                Country = book.Element("author").Element("country") == null ?
                                            "unknown" : book.Element("author").Element("country").Value
                            },
                            Price = Convert.ToDouble(book.Element("price").Value)
                        };

            //get count of search results that query gives us
            int count = query.Count();

            //if count is 0 , then hide the DataGrid
            this.dgResults.Visible = count > 0;

            //display Record Count.
            this.lblBookSearchCount.Visible = true;
            this.lblBookSearchCount.Text = string.Format(CultureInfo.CurrentCulture,"{0} records found",
                count);
            
            

            //call the ToArray method to return an Array of Books.
            //query is IEnumerable<Book> which cannot be bound to a datagrid
            Book[] books = query.ToArray ();

            //set this DataGrid TableStyle's Mapping Name (Array of Books)
            this.dgResults.TableStyles[0].MappingName = "Book[]";

            //Bind to DataGrid
            this.dgResults.DataSource = books;
            this.dgResults.Refresh();

            //set Cursor back to Default
            Cursor.Current = Cursors.Default;

            

        }

        /// <summary>
        /// Click event handler for btnGo
         /// </summary>
         /// <param name="sender"></param>
         /// <param name="e"></param>
        private void btnGo_Click(object sender, EventArgs e)
        {
            if (this.cbGroupBy.SelectedItem == null)
                return;

            //set the Current Cursor to WaitCursor
            Cursor.Current = Cursors.WaitCursor ;

            if (!string.IsNullOrEmpty (this.cbGroupBy.SelectedItem.ToString ()))
            {
                //Get the HTML content
                XElement html = GetHTML(this.cbGroupBy.SelectedItem.ToString ().Trim());

                //set the web browser's DocumentText property
                this.wbGroupByResults.DocumentText = html.ToString();                
            }

            //set Cursor back to Default
            Cursor.Current = Cursors.Default;
        }

        /// <summary>
         /// Click event handler for menu item Exit
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void miExit_Click(object sender, EventArgs e)
        {
            //save the configuration settings(Search History)
            ConfigurationManager.AppSettings = this.searchHistory.SearchHistoryValues;
            ConfigurationManager.Save();

            //close the Application
            this.Close();
        }

        /// <summary>
        /// Load event handler for Form 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Books_Load(object sender, EventArgs e)
        {
            //get the xml file path
            string name = GetXmlFilePath();

            //Load the xml document
            XDocument doc = XDocument.Load(name);

            //check to see if the book elements already have a keywords element inside it
            this.KeyWordsBuilt  = doc.Element("bookstore").Element("book").Element("keywords") != null;

            if (!this.KeyWordsBuilt)
                this.btnAction.Text = "Build Keywords and Search";
            else
                this.btnAction.Text = "Search";

            //get the configuration file settings (Search History)
            NameValueCollection appSettings = ConfigurationManager.AppSettings;

            //set the Search History
            this.searchHistory = new SearchHistory(appSettings);

            
        }

        /// <summary>
        /// Click event handler for btnAction
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAction_Click(object sender, EventArgs e)
        {
            //set the Current Cursor to WaitCursor
            Cursor.Current = Cursors.WaitCursor;

            //if the book elements already has keywords element, then just search
            if (this.KeyWordsBuilt)
            {
                UnifiedSearch();
            }
            else //else build the keywords and write it back to the xml file and then search
            {
                BuildKeyWords();
                UnifiedSearch();
                
                this.btnAction.Text = "Search";
            }

            //set Cursor back to Default
            Cursor.Current = Cursors.Default;
            
        }
        
        /// <summary>
        /// KeyUp event handler for cbUnifiedSearch combobox
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cbUnifiedSearch_KeyUp(object sender, KeyEventArgs e)
        {

            //if the key pressed is any of these , then dont do anything
            if (e.Control || e.Shift  || e.KeyCode == Keys.Back || e.KeyCode == Keys.Delete ||
                e.KeyCode == Keys.Home || e.KeyCode == Keys.ShiftKey )
                return;

            //get the text
            string text = this.cbUnifiedSearch.Text;
            if (text.Length > 0)
            {
                //search to see if the text user is typing at the moment is in the history list
                string match = this.searchHistory.Match(text);

                //if there is a match
                if (!string.IsNullOrEmpty (match ))
                {
                    //set the Text property of the combobox to the match
                    this.cbUnifiedSearch.Text = match;

                    //set selection
                    this.cbUnifiedSearch.SelectionStart = text.Length ;
                    this.cbUnifiedSearch.SelectionLength = match.Length -text.Length ;
                    
                }
            }
        }

        #endregion

    }
   
}