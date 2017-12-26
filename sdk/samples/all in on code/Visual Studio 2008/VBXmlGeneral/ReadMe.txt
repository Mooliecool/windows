========================================================================
    CONSOLE APPLICATION : CSXmlGeneral Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This VB.NET sample project shows how to read a XML file by using XmlTextReader 
or XmlNodeReader. It also shows, instead of using forward-only reader, how to 
read, modify, and update Xml element using the XmlDocument class. This class 
will load the whole document into memory for modification and we can save the 
modified XML file to the file system.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Read XML document using the XmlTextReader class. The XmlTextReader acts as 
a reader pointer that only moves forward. Because it always moves forward 
and reads a piece of data into memory buffer, it has a better performance than
the XmlDocument class which loads the whole document into memory.

2. Read XML document using the XmlNodeReader class. This class is similar tp 
XmlTextReader but ot accepts an XmlNode instance as the target to read. 

3. Use XmlDocument to load the whole XML file into memory.

4. Call SelectSingleNode to navigate to the desired node and change its 
contents by setting the InnerText property

5. Call XmlDocument.CreateElement and CreateAttribute to create a new element
and attribute. Call AppendChild function to add the new element to where we 
want to add it.

6. Save the XmlDocument instance to a local file.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Employing XML in the .NET Framework
http://msdn.microsoft.com/en-us/library/2bcctyt8.aspx


/////////////////////////////////////////////////////////////////////////////
