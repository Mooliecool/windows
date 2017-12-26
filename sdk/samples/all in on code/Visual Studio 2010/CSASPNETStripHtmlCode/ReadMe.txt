========================================================================
                  CSASPNETStripHtmlCode Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to strip and parse Html code.
As the web pages are always include much useful information such as title, 
text, images, links, tables, etc. Sometimes we need strip the key words or 
resources from a web page, so this code-sample can help us finish the work
in regular expression.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the CSASPNETStripHtmlCode.sln.

Step 2: Expand the CSASPNETStripHtmlCode web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You will see one MultiLine TextBox and some Button controls on the page.
        Click these Buttons in turn.

Step 4: The striped information displaying in the TextBox, you can find the 
        entire html code, script code, pure text, images and links.

Step 5: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETStripHtmlCode".

Step 2. Add two web forms in the root directory, name them as "Default.aspx", 
        "SourcePage.aspx".

Step 3. Add script code, some links, some images and text on the Source
        page. We will strip the useful message from it.

Step 4. Add one MultiLine TextBox control and five Button controls on Default
        page, this web form used to retrieve the message of Source page
		
Step 5  Add some C# code like this below, retrieve entire Html code: 
		[code]
            /// <summary>
            /// Retrieve the entire html code from SourcePage.aspx with WebRequest and
            /// WebRespond. We're transfer the format of html code to uft-8.
            /// </summary>
            /// <param name="url"></param>
            /// <returns></returns>
            public string GetWholeHtmlCode(string url)
            {
                string strHtml = string.Empty;
                StreamReader strReader = null;
                WebResponse wrpContent = null;
                try
                {
                    WebRequest wrqContent = WebRequest.Create(url);
                    wrqContent.Timeout = 300000;
                    wrpContent = wrqContent.GetResponse();
                    if (wrpContent != null)
                    {
                        strReader = new StreamReader(wrpContent.GetResponseStream(),
						    Encoding.GetEncoding("utf-8"));
                        strHtml = strReader.ReadToEnd();
                    }
                }
                catch (Exception e)
                {
                    strHtml = e.Message;
                }
                finally 
                {
                    if (strReader != null)
                        strReader.Close();
                    if (wrpContent != null)
                        wrpContent.Close();
                }
                return strHtml;
            }
	    [/code]
	    Use regular expression to strip and parse specifically part of entire Html
	    code, for example, strip pure text button, the C# code like this:
	    [code]
		    string strRegexScript = @"(?m)<body[^>]*>(\w|\W)*?</body[^>]*>";
            string strRegex = @"<[^>]*>";
            string strMatchScript = string.Empty;
            Match matchText = Regex.Match(strWholeHtml, strRegexScript, 
			    RegexOptions.IgnoreCase);
            strMatchScript = matchText.Groups[0].Value;
            string strPureText = Regex.Replace(strMatchScript, strRegex,
			    string.Empty, RegexOptions.IgnoreCase);
            tbResult.Text = string.Empty;
            tbResult.Text = strPureText;
	    [/code] 

Step 5. Add other Button control's click event with C# code like this sample's 
        "Default.aspx.cs" file.

Step 6. Build the application and you can debug it.
/////////////////////////////////////////////////////////////////////////////
References:

MSDN: .NET Framework Regular Expressions
http://msdn.microsoft.com/en-us/library/hs600312.aspx

MSDN: Regex Class
http://msdn.microsoft.com/en-us/library/system.text.regularexpressions.regex.aspx

MSDN: WebRequest Class
http://msdn.microsoft.com/en-us/library/system.net.webrequest.aspx

MSDN: WebResponse Class
http://msdn.microsoft.com/en-us/library/system.net.webresponse.aspx
/////////////////////////////////////////////////////////////////////////////