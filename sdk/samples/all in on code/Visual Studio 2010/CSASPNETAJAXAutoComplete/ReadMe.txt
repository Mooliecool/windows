========================================================================
         ASP.NET APPLICATION : CSASPNETAJAXAutoComplete Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to use AutoCompleteExtender to display words 
that begin with the prefix that is entered into a text box.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a C# ASP.NET Web Application in Visual Studio 2010 and name it 
as CSASPNETAJAXAutoComplete.

Step2. Delete the following default folders and files created automatically 
by Visual Studio.

Account folder
Script folder
Style folder
About.aspx file
Default.aspx file
Global.asax file
Site.Master file

Step3. Add a new web form page to the website and name it as Default.aspx.

Step4. Add ToolkitScriptManager into the beginning page. Then add a TextBox 
and a AutoCompleteExtender in the page.You can find the AutoCompleteExtender 
in the Ajax Control Toolkit category of the Toolbox.

[NOTE] When a ToolkitScriptManager is added into the page, such a Register 
Info will be added to the same page automatically.

<%@ Register Assembly="AjaxControlToolkit" Namespace="AjaxControlToolkit" 
TagPrefix="asp" %>

For more details of how to add Ajax Control Toolkit, please refer to:
http://www.asp.net/ajaxlibrary/act.ashx.

Step5: Add a new web service to the website and name it as Searcher.asmx. 
Then add WebMethod in the Searcher.cs.

[WebMethod]
public string[] HelloWorld(string prefixText, int count) 
    {
        if (count == 0)
        {
            count = 10;
        }

        if (prefixText.Equals("xyz"))
        {
            return new string[0];
        }

        Random random = new Random();
        List<string> items = new List<string>(count);
        char c1;
        char c2;
        char c3;
        for (int i = 0; i < count; i++)
        {
            c1 = (char)random.Next(65, 90);
            c2 = (char)random.Next(97, 122);
            c3 = (char)random.Next(97, 122);
            items.Add(prefixText + c1 + c2 + c3);
        }

        return items.ToArray();
    }

[NOTE] When a web service is added into the application, a same name cs file 
will be added in the App_Code folder automatically. For more details of web
service, please refer to:http://msdn.microsoft.com/en-us/library/t745kdsh.aspx.

Step6: Set the corresponding properties for AutoCompleteExtender.

<asp:AutoCompleteExtender ID="AutoCompleteExtender1" runat="server" 
TargetControlID="txtSearch" ServicePath="~/Searcher.asmx" 
ServiceMethod="HelloWorld" MinimumPrefixLength="1" CompletionSetCount="10">
</asp:AutoCompleteExtender>

[NOTE] For the details of AutoCompleteExtender properties, please refer to:
http://www.asp.net/ajaxlibrary/act_AutoComplete.ashx.

Step7: Now, you can run the page to see the achievement we did before :)

/////////////////////////////////////////////////////////////////////////////
References:

AutoComplete Tutorials:
http://www.asp.net/ajaxlibrary/act_AutoComplete.ashx

AutoComplete Sample:
http://www.asp.net/ajax/ajaxcontroltoolkit/Samples/AutoComplete/AutoComplete.aspx

/////////////////////////////////////////////////////////////////////////////