========================================================================
    SILVERLIGHT APPLICATION : CSSL3OOB Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Overview:

This example demonstrates how to work with OOB using CS.
It includes the following features:
Install OOB with code (but you cannot remove OOB with code).
Check if the application is already installed.
Check for updates.
Check for network state.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Required:
Silverlight 3 Chained Installer
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a
-88f3-5e7b5409e9dd&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Usage:

As for the usage of this sample, first please make sure you've set the
CSSL3OOB.Web project as the startup project. Otherwise some features will not
work properly.

Click the Install with code Button, or right click and choose Install CSSL3OOB
application onto this computer, to install the application.

Close the browser, but do not close the OOB window. Update the source code,
rebuild and launch the application in browser again. You'll notice the OOB
application has not been updated yet. Click Check for update in the OOB
application (not the browser application), and you'll see it asks you to
restart the OOB. After you restart, you'll notice the update has been applied.

Disable your network connection, and note the red text (information about
network status) in the bottom of the screen being updated. Enable your network
connection again, and the red text will be updated again.


/////////////////////////////////////////////////////////////////////////////
Documentation:
OOB, as the name indicates, allows you to work with Silverlight applications
out of browser. It is very easy to configure a Silverlight application to
support OOB within Visual Studio. In the Properties page of the Silverlight
project, simply check "Enable running application out of browser". Click the
Out-Of-Browser Settings Button to configure the OOB properties. The MSDN
document has detailed description about the properties.
http://msdn.microsoft.com/en-us/library/dd833073(VS.95).aspx
    
Under the hook, what Visual Studio does is exactly creating a manifest file
that corresponds to that described in the MSDN document. You can find it in
the Bin folder after you build your application. You can also edit the
OutOfBrowserSettings.xml file under the Properties folder directly, if you
don't want to use the tool.

OOB supports update, if it is downloaded from a web site (instead of local
file system). But you have to manually check if an update is available by
handling the Application.Current.CheckAndDownloadUpdateCompleted event.

Most Silverlight features work fine in OOB. However, certain features will
only work if the application is installed from a web site. For example, check
for update. That's why this sample includes a web application. Please run the
web application instead of the Silverlight application directly. Also, if the
application needs to perform network access, you'll have to host it in a web
site. Otherwise you'll ge cross-scheme errors even if you run inside the
browser...

Network features work fine in OOB. If you're accessing a network resource
from the same domain where you install the application, no cross-domain
policy file is needed. Otherwise, as long as the cross-domain policy file
allows you to access the resource, it will work fine.

So how does OOB work? Actually the following application is always launched
when you open an OOB application:
For x86: C:\Program Files\Microsoft Silverlight\sllauncher.exe
For x64: C:\Program Files (x86)\Microsoft Silverlight\sllauncher.exe

This application accepts a command line argument like:
(a number as ID).domainname

When you install the OOB, several files will be downloaded to the following
folder:

For Vista and later:
Users\yourname\AppData\LocalLow\Microsoft\Silverlight\OutOfBrowser
\NumberAsID.domain

For ealier OS:
Documents and Settings\yourname\Local Settings\Application Data\Microsoft
\Silverlight\OutOfBrowser\NumberAsID.domain

You'll find a metadata file in this location. This metadata file stores
information such as where this OOB was downloaded. This affects both update
and network. If you modify this file to specify another domain, update will
no longer work, and the original domain does not contain a cross-domain
policy file, you will no longer be able to access to the network resource.
However, you can actually use it at your advantage to access network
resources on another domain that does not have a cross-domain policy file.
Please refer to the following instructions:

Create your application so that it accesses a network resource on another
domain that does not have a cross-domain policy file (such as www.bing.com).
Install the OOB.
Open the metadata file for the OOB application.
Modify the FinalAppUri, OriginalSourceUri, and SourceDomain properties, so
that they point to the external domain.
Launch the OOB application, and you'll notice you're able to access the
network resources on the other domain.

However, you have to instruct your users to perform all those steps.


/////////////////////////////////////////////////////////////////////////////
References:

http://msdn.microsoft.com/en-us/library/dd833073(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
