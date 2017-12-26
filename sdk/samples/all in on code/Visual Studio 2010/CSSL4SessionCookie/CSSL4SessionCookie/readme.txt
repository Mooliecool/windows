=============================================================================
       Sivlerlight APPLICATION: CSSL4SessionCookie Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:  

This sample demonstrates a simple technique to preserve ASP.NET session ID 
from a web page hosting a Silverlight component making a client WebRequest to 
another web page on the same site.  Normally, the WebRequest will not by 
default preserve session id, and this can be frustrating for a Silverlight 
developer.  But by appending the Session ID cookie manually to the request, 
passing it into the Silverlight component through a parameter on the calling 
web page, the session can in fact be preserved.


/////////////////////////////////////////////////////////////////////////////
Demo:

To see the sample, run the code and launch CSSL4SessionCookieTestPage.aspx 
and VerifySessionMaintained.aspx. It demonstrates preserving ASP.NET session 
ID from a web page hosting a Silverlight component making a client WebRequest 
to another web page on the same site.  

In MainPage.xaml.cs of CSSL4SessionCookie, commenting out the following line 
will demonstrate the session ID being lost in the resulting web client request.  

    wr.CookieContainer.Add(new Uri("http://localhost:7777"), 
        new Cookie("ASP.NET_SessionID", session));


/////////////////////////////////////////////////////////////////////////////
Implementation:

CSSL4SessionCookieTestPage.aspx simply launches the Silverlight component 
which is built into the CSSSLSessionCookie.xap, in the ClientBin directory of 
the output, and passes a parameter which includes the ASP.NET Session ID, on 
line 83 of CSSL4SessionCookieTestPage.aspx.  This originating web page also 
displays its current session ID so the user can see and compare to confirm 
that indeed, the session ID will match what is reported from the Silverlight 
request as well.

When the Silverlight component launches, the code in MainPage.xaml.cs is 
launched, and creates a WebRequest used to load 
VerifySessionStateMaintained.aspx, another web page on the web site from 
which the original .aspx was loaded.  The WebRequest appends a cookie for the 
ASP.NET_SessionID, providing the parameter value that was originally passed 
via URL to the Silverlight component.

Finally, the VerifySessionStateMaintained.aspx page is loaded and returned to 
the WebRequest.  This page simply returns a line of text confirming the 
Session ID of its current session.

The Silverlight component just outputs the text returned from 
VerifySessionStateMaintained.aspx, when it is received, so the user can 
compare the session ID of the original web page, alongside that of the page 
called from within Silverlight, to verify they are the same.


/////////////////////////////////////////////////////////////////////////////
References:  

For more information about Silverlight, go to 
http://www.silverlight.net/.  

For more information about ASP.NET session state, go to 
http://msdn.microsoft.com/en-us/library/ms972429.aspx.

For more information about using the WebRequest, go to 
http://msdn.microsoft.com/en-us/library/system.net.webrequest.aspx.


/////////////////////////////////////////////////////////////////////////////