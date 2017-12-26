=============================================================================
                  CSASPNETServerClock Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

 This project illustrates how to get the time of the server side and show 
 it to the client page. Sometimes a website need to show an unified clock 
 on pages to all the visitors. However, if we use JavaScript to handle this
 target, the time will be different from each client. So we need the server
 to return the server time and refresh the clock per second via AJAX. 


/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

It is better to demo this sample with two computers. One is as the client
side and the other is working as the server side.

Step1: Publish this sample site to one of the computers to make the other
one can visit it from the browser.

Step2: Open the browser to view the Default.aspx page from the computer as 
the client side. You will find there is a clock on the page which displays 
the time.

Step3: Change the time of the client side computer. And open the browser
to view the Default.aspx page again. You can find that the time on the page
is different from the clock of the computer itself. This shows that the 
time there is not based on the client side's time, but the time from the 
server side.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a C# ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a new ASP.NET page to the application and named it as Clock.aspx. 
Write the code below to it Page_Load event.

    protected void Page_Load(object sender, EventArgs e)
    {
        Response.Expires = -1;
        Response.Write(DateTime.Now.ToString());
    } 

Step3: Add a Default.aspx ASP.NET page to the application. Design the HTML
code as follows.

    <form id="form1" runat="server">
    <div>
        The server time is now：<span id="time" />
    </div>
    </form>

Step4: Add the JavaScript function to get the time from the server side.
    
    function doing() {
        var xmlHttp;
        try {
            xmlHttp = new XMLHttpRequest();
        }
        catch (e) {
            try {
                xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
            }
            catch (e) {
                try {
                    xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
                }
                catch (e) {
                    alert("Error");
                    return false;
                }
            }
        }

        xmlHttp.onreadystatechange = function () {
            if (xmlHttp.readyState == 4) {
                var strResult = xmlHttp.responseText;
                document.getElementById("time").innerText = strResult;
            }
        }

        xmlHttp.open("GET", "Clock.aspx", true);
        xmlHttp.send(null);
    }

    function gettingTime() {
        setInterval(doing, 1000);
    } 
    
Step5: Set the onload event of the body element to call the function doing() 
function.

/////////////////////////////////////////////////////////////////////////////
References:

# MSDN: Calling Web Services from Client Script
http://msdn.microsoft.com/en-us/library/bb398995.aspx

/////////////////////////////////////////////////////////////////////////////