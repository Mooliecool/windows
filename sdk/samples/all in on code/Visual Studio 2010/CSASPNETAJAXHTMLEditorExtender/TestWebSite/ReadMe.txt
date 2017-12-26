========================================================================
                 CSASPNETHTMLEditorExtender Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to add a custom button to the toolbar of a 
HTMLEditor in the Ajax Control Toolkit 4.1.40412.0.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the CSASPNETHTMLEditorExtender.sln.

Step 2: [This step is very important!] Rebuild the solution.

Step 3: Expand the TestWebSite and right-click the Default.aspx  
        and click "View in Browser". 

Step 4: Input some text into the Editor.

Step 5: Select some text from your input.

Step 6: Click the last "H1" button form the top toolbar.

Step 7: You will see the selected text has been formatted to the style of H1.
        If you click the second button from the bottom toolbar, you will see
        that the selected text has been surrounded with tag <H1>.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1: Download the AjaxControlToolkit from this link:
        http://www.asp.net/ajaxlibrary/act.ashx

Step 2.  Create a C# "Class Library" project in Visual Studio 2010 or
         Visual Web Developer 2010. Change the name to HTMLEditorExtender.

Step 3.  Add references in the list below:
         AjaxControlToolkit (version 4.1.40412.0)
         System.Web 
         System.Web.Extensions

Step 4.  Create two new folders, call them Images and ToolBar_buttons.

Step 5.  we need two icons for the button, one for the actived button and one 
         for the un-actived button. I create two images, 
         one called ed_format_h1_a.gif and another is ed_format_h1_n.gif. 
         Add the two images into the folder Images.

Step 6.  Select the two images in the Solution Explorer of the VS.
         Right-click them and select Properties. 
         You can find Build Action, 
         set the Build Action to Embedded Resource.


Step 7.  In the folder ToolBar_buttons, create a js file, I named it H1.pre.js.
         
Step 8.  Write the JavaScript functions in H1.pre.js to register 
         the client features for the H1 button. We can get the full functions in
         the sample file H1.pre.js.
         
Step 9.  Do the same thing in Step 5, make the Build Action to Embedded Resource 
         for the H1.pre.js

Step 10.  Create a new class file, I call it H1.cs, in the  ToolBar_buttons folder.

Step 11. Write the codes to register server side class for the button. Refer to 
         sample file H1.cs.

Step 12. Create a new class in the root of the project, I call it MyEditor.cs.
         Write the code like below to make an extender Editor.
         [CODE]
         namespace HTMLEditorExtender   
         {   
             public class MyEditor : Editor   
             {   
                 protected override void FillTopToolbar()   
                 {   
                     base.FillTopToolbar();   
                     TopToolbar.Buttons.Add(new H1());   
                 }   
             }   
         }  
         [/CODE]

Step 13. Build the project.

Step 14. Create a new C# "Web Site", change the last folder name to TestWebSite.

Step 15. Add reference of the class project, HTMLEditorExtender.

Step 16. Create a test page. And add two Register declaration like below.
         [CODE]
         <%@ Register Assembly="AjaxControlToolkit" Namespace="AjaxControlToolkit" TagPrefix="asp" %>
         <%@ Register Assembly="HTMLEditorExtender" Namespace="HTMLEditorExtender" TagPrefix="asp" %>
         [/CODE]

Step 17. Create a ToolScriptManager and a MyEditor to the page.
         [CODE]
         <asp:ToolkitScriptManager runat="server" ID="ToolkitScriptManager1"></asp:ToolkitScriptManager>
         <asp:MyEditor runat="server" ID="MyEditor1" />
         [/CODE]

Step 18. Test the Default.aspx.

/////////////////////////////////////////////////////////////////////////////
Reference:

HTMLEditor Tutorials
http://www.asp.net/ajaxlibrary/act_HTMLEditor.ashx