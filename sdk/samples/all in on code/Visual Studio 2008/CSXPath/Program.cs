/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSXPath
* Copyright (c) Microsoft Corporation.
* 
* This sample project shows how to use XPathDocument class to load the XML 
* file and manipulate. It includes two main parts, XPathNavigator usage and 
* XPath Expression usage. The first part shows how to use XPathNavigator to 
* navigate through the whole document, read its content. The second part 
* shows how to use XPath expression to filter information and select it out.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.XPath;
#endregion


namespace CSXPath
{
    class Program
    {
        static void Main(string[] args)
        {
            #region Initialize XPathDocument and XPathNavigator

            XPathNavigator xPathNavigator;
            XPathDocument xPathDoc;

            //Navigate through the whole document
            //Create a new instance of XPathDocument from a XML file
            xPathDoc = new XPathDocument("books.xml");

            //Call CreateNavigator method to create a navigator instance
            //And we will use this navigator object to navigate through whole document
            xPathNavigator = xPathDoc.CreateNavigator();
            
            #endregion

            #region Navigate through the document

            //Move to the root element
            xPathNavigator.MoveToRoot();
            //Catalog element is the first children of the root
            //Move to catalog element
            xPathNavigator.MoveToFirstChild();
            //We can know a XML node's type from the NodeType property
            //XPathNodeType has Attribute, Element, Namespace and so on
            if (xPathNavigator.NodeType == XPathNodeType.Element)
            {
                //We can know if a Node has child nodes by checking its
                //HasChildren property. If it returns true, that node has
                //child nodes
                if (xPathNavigator.HasChildren == true)
                {
                    //Move to the first child which is our first book nodes
                    xPathNavigator.MoveToFirstChild();
                    do
                    {
                        //We can know if a node has any attribute by checking
                        //the HasAttributes property. When this property returns
                        //true, we can get the specified attribute by calling
                        //navigator.GetAttribute() method
                        if (xPathNavigator.HasAttributes == true)
                        {
                            Console.WriteLine("Book ID: " + xPathNavigator.GetAttribute("id", ""));
                        }

                        //Iterate through a book node's child nodes
                        //and list all child node information, like 
                        //name, author, price, publish date and so on
                        if (xPathNavigator.HasChildren)
                        {
                            xPathNavigator.MoveToFirstChild();
                            do
                            {
                                Console.Write("\t{0}:\t{1}\r\n", xPathNavigator.Name, xPathNavigator.Value);
                            } while (xPathNavigator.MoveToNext());
                            //When all child nodes are reached. The MoveToNext() method returns
                            //false. Then we need to call MoveToParent to go back to the book level
                            xPathNavigator.MoveToParent();
                        }
                        //Move to the next book element.
                    } while (xPathNavigator.MoveToNext());
                }
            }

            #endregion

            #region Use of XPath Expression
            
            //Use XPath Expression to select out book bk103
            //The expression should be "/catalog/book[@id='bk103']"
            //@ means to look id attribute and match bk103
            Console.WriteLine("Use XPath Expression to select out the book with ID bk103:");
            XPathExpression expression = xPathNavigator.Compile("/catalog/book[@id='bk103']");
            XPathNodeIterator iterator = xPathNavigator.Select(expression);

            //After compile the XPath expression, we can call navigator.Select
            //to retrieve the XPathNodeIterator. With this interator, we can loop
            //trough the results filtered by the XPath expression
            //The following codes print the book bk103's detailed information
            while (iterator.MoveNext())
            {
                XPathNavigator nav = iterator.Current.Clone();
                Console.WriteLine("Book ID: " + nav.GetAttribute("id", ""));
                if (nav.HasChildren)
                {
                    nav.MoveToFirstChild();
                    do
                    {
                        Console.Write("\t{0}:\t{1}\r\n", nav.Name, nav.Value);
                    } while (nav.MoveToNext());
                }
            }


            //Use XPath to select out all books whose price are more than 10.00
            // '[]' means to look into the child node to match the condition "price > 10"
            Console.WriteLine("\r\nUse XPath Expression to select out all books whose price are more than 10:");
            expression = xPathNavigator.Compile("/catalog/book[price>10]");
            iterator = xPathNavigator.Select(expression);

            //After getting the iterator, we print title and price for books 
            //whose price are more than 10
            while (iterator.MoveNext())
            {
                XPathNavigator nav = iterator.Current.Clone();
                Console.WriteLine("Book ID: " + nav.GetAttribute("id", ""));
                if (nav.HasChildren)
                {
                    nav.MoveToFirstChild();
                    do
                    {
                        if (nav.Name == "title")
                        {
                            Console.Write("\t{0}:\t{1}\r\n", nav.Name, nav.Value);
                        }
                        if (nav.Name == "price")
                        {
                            Console.Write("\t{0}:\t{1}\r\n", nav.Name, nav.Value);
                        }
                    } while (nav.MoveToNext());
                }
            }

            //Use XPath Expression to calculate the average price of all books.
            //Here in XPath, we use the sum, div, and count formula.
            Console.WriteLine("\r\nUse XPath Expression to calculate the average price of all books:");
            expression = xPathNavigator.Compile("sum(/catalog/book/price) div count(/catalog/book/price)");
            string averagePrice = xPathNavigator.Evaluate(expression).ToString();
            Console.WriteLine("The average price of the books are {0}", averagePrice);


            #endregion

            #region End. Read a char to exit

            Console.WriteLine("Input any key to quit the sample application");
            Console.ReadLine();

            #endregion
        }
    }
}
