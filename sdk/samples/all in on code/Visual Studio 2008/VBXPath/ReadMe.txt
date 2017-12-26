========================================================================
    CONSOLE APPLICATION : VBXPath Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample project shows how to use XPathDocument class to load the XML file
and manipulate. It includes two main parts, XPathNavigator usage and XPath
Expression usage. The first part shows how to use XPathNavigator to navigate 
through the whole document, read its content. The second part shows how to use
XPath expression to filter information.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Initialize XPathDocument and XPathNavigator instances

2. Call MoveToRoot and MoveToFirstChild to navigate to the book elements

3. Loop through all books and thier children nodes. Output author, title, genre
price, publish_date, description information for each book.

4. Use XPath Expression to select out the book with ID bk103 and output its
detailed information

5. Use XPath Expression to select out all books whose price are more than 10

6. Use XPath Expression to calculate the average price of all books


/////////////////////////////////////////////////////////////////////////////
References:

How to navigate XML with the XPathNavigator class by using C#
http://support.microsoft.com/kb/308343

How to query XML with an XPath expression by using C#
http://support.microsoft.com/kb/308333

/////////////////////////////////////////////////////////////////////////////
