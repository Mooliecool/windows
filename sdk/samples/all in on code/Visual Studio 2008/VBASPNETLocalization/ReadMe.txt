========================================================================
      ASP.NET APPLICATION : VBASPNETLocalization Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to build a multi-lingual website with ASP.NET
  Localization. ASP.NET enables pages obtain content and other data based
  on the preferred language setting of the browser or based on the user's 
  explicit choice of language. If controls are configured to get property
  values from resources, at run time, the resource expressions are replaced
  by resources from the appropriate resource file.  


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a Visual Basic ASP.NET Web Application in Visual Studio 2008 
or Visual Web Developer. Then rename the defualt page from Defualt.aspx to
LocalResources.aspx

Step2. Right click the project's name and select contect menu item follow 
this order: "Add" -> "Add ASP.NET forlder" -> "App_LocalResources".

Step3: Add four new Resources files in this App_LocalResources folder and 
name them as follows:
    
    LocalResources.aspx.de.resx
    LocalResources.aspx.en.resx
    LocalResources.aspx.fr.resx
    LocalResources.aspx.resx

[NOTE] In App_LocalResources folder, the Resources is one on one related to 
ASP.NET page by its name. For example, if the Resources file is working for
Default.aspx, then its name should be Default.aspx.[CultureInfo].resx. Here,
our page's name is LocalResources.aspx, so the Resources files need to be
renamed as LocalResources.aspx.[CultureInfo].resx. The [CultureInfo] is an 
abbreviation to tell with which culture the Resources file is used. Please 
refer to this list for brief information on culture names and corresponding 
abbreviations.

Arabic      ar
German      de
Greece      el
English     en
Spanish     es
French      fr
Italian     it
Japanese    ja
Korean      ko
Chinese     zh

Step4: Edit these four Resources files according to the culture to set keys 
and corresponding values. For example, if we need lblLocal.Text to display 
"Hello World" in french, so we add the key "lblHelloWorld.Text" and the value 
"Bonjour tout le monde" to LocalResources.aspx.fr.resx file.

[NOTE] The LocalResources.aspx.resx is the file for an invariant culture. No 
culture is assigned to it. If no culture can be determined, the file is then 
utilized. In the demo, we set LocalResources.aspx.resx as the same to the one
in English.

Step5: Add a DropDownList to the page to let users select the language they 
want to read with. Edit the items' Text as the language name and the value as 
the appropriate abbreviations according to the table above.

Step6: Add InitializeCulture() event handler into Code-Behind page and over-
rides this handler to set the CurrentUICulture and CurrentCulture according 
to the DropDownList's value. 

    Protected Overrides Sub InitializeCulture()
        Dim strLanguageInfo As String = Request.Form("ddlLanguage")

        If Not strLanguageInfo Is Nothing Then
            'Thread.CurrentThread.CurrentUICulture = ...
            'Thread.CurrentThread.CurrentCulture = ...
        End If
    End Sub

[NOTE] The reason why here we don't use ddlLanguage.SelectedValue is all the 
page controls, including ddlLanguage is unavailable in this event.

Step6: Add a Label control to LocalResources.aspx and rename it to lblLocal.

Step7: Edit the HTML tags of the Label to add meta:resourceKey attribute and 
point this attribute to the key name in the Resources files.

    <asp:Label ID="lblHelloWorld"  runat="server" 
               meta:resourceKey="lblLocal" />
               
[NOTE] In the figure, the properties of the controls are defined in Local
Resource files. For example, the Label control has its Text property exposed 
as lblLocal.Text. ASP.NET then sets this property value to Label based on 
the culture selection.

[NOTE] All the steps we did above is to show how to use Local Resources 
localize pages. However, all these changes only works in the single page 
and if we goto another page, they will be lost. To handle this issue, 
please continue the following steps to learn how to use Global Resources.

Step8: Right click the project's name and select contect menu item follow 
this order: "Add" -> "Add ASP.NET forlder" -> "App_GlobalResources".

Step9: Add four new Resources files in this App_GlobalResources folder. Then
rename them and edit them to meet culture data of different languages.

[NOTE] When using Global Resources files, the file's name doesn't need to be
the same as any page's name. We can simply call them like GlobalResources. 

Step10: Add a new ASP.NET page and name it as GlobalResources.aspx. Also, add
a Button control to LocalResources.aspx to do the navigation to the page using
Global Resources via PostBackUrl property

[NOTE] Please don't use a HyperLink or write Response.Redirect to transfer
the page. This is because we need to pass the value in the DropDownList to
the new page.

Step11: Add a new Label to the page and rename it to lblGlobal. Then set the
Text property and the BackColor property of this Label as an Expression Text 
within <%$ ... %>.

    <asp:Label ID="lblGlobal" runat="server" 
               BackColor="<%$ Resources:GlobalResources, lblGlobalBackColor%>"
               Text="<%$ Resources:GlobalResources, lblGlobalText%>">
    </asp:Label>
               
[NOTE] This is the way Global Resources works. An Expression is set to the
property we need to change based on different cultures. We start it with 
"Resources" to declare this is resources field. Then, append the name of a 
certain Global Resources file and the key name in it with a comma to split 
these two parameters. By this way, the resouces data, like lblGlobalText is
able to be accessed from entire website.

Step12: Copy both the HTML source code and the Visual Basic behind code from
LocalResources.aspx to GlobalResources.aspx and edit the btnNavigate Button 
to give a back link.

[NOTE] Run the application from LocalResources.aspx and select a language.
We can see the two Labels in the page are both changed based on the value in 
DropDownList. However, if we redirect to GlobalResources.aspx via the Button, 
we find only lblGlobal is changed. Because only this Label is localized by 
Global Resources feature.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: ASP.NET Globalization and Localization
http://msdn.microsoft.com/en-us/library/c6zyy3s9.aspx

MSDN: Resources and Localization in ASP.NET 2.0
http://msdn.microsoft.com/en-us/magazine/cc163566.aspx


/////////////////////////////////////////////////////////////////////////////