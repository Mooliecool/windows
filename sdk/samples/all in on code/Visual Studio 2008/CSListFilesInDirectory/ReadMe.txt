========================================================================
   WINDOWS FORMS APPLICATION : CSListFilesInDirectory Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSListFilesInDirectory project demonstrates how to implement an 
IEnumerable<string> that utilizes the Win32 File Management functions to 
enable application to get files and sub-directories in a specified directory
one item a time.

The FileEnumerator class in this sample project solved a common problem in
the System.IO.Directory.GetFiles method - if a directory contains a large
number of items, this it will take a long time for this method to return
because it will enumerate all the files and put the names in an array as
an entire operation. This will also cause a very high memory load if the
array gets huge.

The FileEnumerator class works differently. It returns one file at a time.
And the enumeration can be canceled or reset at anytime.


/////////////////////////////////////////////////////////////////////////////
References:

FindFirstFile Function
http://msdn.microsoft.com/en-us/library/aa364418(VS.85).aspx

FindNextFile Function
http://msdn.microsoft.com/en-us/library/aa364428(VS.85).aspx

FindClose Function
http://msdn.microsoft.com/en-us/library/aa364413(VS.85).aspx

IEnumerable<T> Interface
http://msdn.microsoft.com/en-us/library/9eekhta0.aspx

IEnumerator<T> Interface
http://msdn.microsoft.com/en-us/library/78dfe2yb.aspx


/////////////////////////////////////////////////////////////////////////////
