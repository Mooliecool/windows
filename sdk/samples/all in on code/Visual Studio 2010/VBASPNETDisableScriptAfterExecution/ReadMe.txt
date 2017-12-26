========================================================================
              VBASPNETDisableScriptAfterExecution Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample code illustrates how to register script code at code behind and to 
be disabled after execution. Sometimes users who register scripts do not want 
them execute again, Actually they with to achieve this either in an automatic
manner or by imitating an action for example, by clicking a link or button. 
This maybe due to functional purpose, user experience or security concerns. 


/////////////////////////////////////////////////////////////////////////////
Demo:

Please follow these demonstration steps below.

Step 1: Open the VBASPNETDisableScriptAfterExecution.sln.

Step 2: Expand the VBASPNETDisableScriptAfterExecution web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You will see one button on the page, click it, you can find a dialog 
        box that tell your this is user's defined logic. In this sample code, 
		your web page will move down.

Step 4: The second step, you can click this button again, the web page will 
        tell you the JavaScript function has been disabled.

Step 5: Validation finished.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a VB "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "VBASPNETDisableScriptAfterExecution".

Step 2. Add one web form in the root directory, name them as "DisableScript.aspx".

Step 3. Add a HTML button on the DisableScript page, which have an onclick event.

Step 4. Register JavaScript functions at DisableScript code behind page, we
        need add two functions, "main()" and "callBackFunc()". The main function 
		includes user logic, call back function is used to return error information.
		The VB code as shown below:
		[code]
		''' <summary>
    ''' Register JavaScript functions at code behind page(Page_Load event). 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim clientManager As ClientScriptManager = Page.ClientScript

        ' JavaScript function part 1, this function is used to execute user's logical
        ' code with a additional parameter, this parameter is a callback function 
        ' which is make a condition that if the main function had executed.
        Dim mainScript As String = vbCr & vbLf & " var flag = true;" & vbCr & vbLf &
                                                 " var mainFunc = function main(callBack) {" & vbCr & vbLf &
                                                 " var callBackPara = callBack;" & vbCr & vbLf &
                                                 " if (flag) {" & vbCr & vbLf &
                                                 " // User code." & vbCr & vbLf &
                                                 " alert('This is user code, in this sample code, your page will move down.');" & vbCr & vbLf &
                                                 " " & vbCr & vbLf &
                                                 " for (var i = 1; i <= 900; i++)" & vbCr & vbLf &
                                                 " {" & vbCr & vbLf &
                                                 " window.moveBy(0, 1);" & vbCr & vbLf &
                                                 " }" & vbCr & vbLf &
                                                 " window.moveBy(0,-750);" & vbCr & vbLf &
                                                 " " & vbCr & vbLf &
                                                 " // Disable JS function." & vbCr & vbLf &
                                                 " flag = undefined;" & vbCr & vbLf &
                                                 " }" & vbCr & vbLf &
                                                 " else {" & vbCr & vbLf &
                                                 " alert(callBackPara);" & vbCr & vbLf &
                                                 " }}"
        clientManager.RegisterClientScriptBlock(Me.[GetType](), "mainScript", mainScript, True)

        ' JavaScript function part 2, the callback function, check if flag variable is undefined.
        Dim callbackScript As String = " var callFunc = function callBackFunc() {" & vbCr & vbLf &
                                       " if (!flag) {" & vbCr & vbLf &
                                       " return 'The JavaScript function has been disabled..';" & vbCr & vbLf &
                                       " }}"
        clientManager.RegisterStartupScript(Me.[GetType](), "callBackScript", callbackScript, True)

    End Sub
		[/code]  

Step 5. Build the application and you can debug it.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: JavaScript
http://msdn.microsoft.com/en-us/library/ms970435.aspx

MSDN: ClientScriptManager Class
http://msdn.microsoft.com/en-us/library/system.web.ui.clientscriptmanager(v=vs.80).aspx


/////////////////////////////////////////////////////////////////////////////