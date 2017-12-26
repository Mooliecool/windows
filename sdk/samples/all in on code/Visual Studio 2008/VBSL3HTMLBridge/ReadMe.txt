========================================================================
    SILVERLIGHT APPLICATION : VBSL3HTMLBridge Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project create a group of samples demonstrating the interactivity between
silverlight application and javascript. The sample includes:
    Call Javascript method from managed code
    Handle Html event from managed code
    Call managed code method from javascript
    Handle managed code event from javascript


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Sample 1: Call Javascript method from managed code
    1. Write javascript method and put in header of silverlight hosted page.
        function changetext(name) {
            document.getElementById('Text1').value = name;
        }
        
    2. Invoke javascript method in managed code.
        HtmlPage.Window.Invoke("changetext", tb1.Text)

Sample 2: Handle Html event from managed code
    1. Attach html event in MainPage Loaded event handler.
        HtmlPage.Document.GetElementById("Text2").AttachEvent("onkeyup", New EventHandler(AddressOf HtmlKeyUp))
    
    2. Write handler method "HtmlKeyUp".
        Private Sub HtmlKeyUp(ByVal sender As Object, ByVal e As EventArgs)
            Me.tb2.Text = DirectCast(sender, HtmlElement).GetProperty("value").ToString()
        End Sub

Sample 3: Call managed code method from javascript
    1. Write method in MainPage code behind and apply ScriptableMember attribute
        <ScriptableMember()> _
        Public Sub ChangeTB3Text(ByVal text As String)
            tb3.Text = text
        End Sub
        
    2. Register ScriptableObject instance in MainPage Loaded event.
        HtmlPage.RegisterScriptableObject("silverlightPage", Me)

    3. Write javascript event handler for html textbox onchanged event. In handler function, call managed code.
        function ontext3keydown() {
            var text = document.getElementById('Text3').value;
            // calling managed code method
            var silverlight1 = document.getElementById('silverlight1');
            silverlight1.Content.silverlightPage.ChangeTB3Text(text);
        }
    
Sample 4: Handle managed code event from javascript
    1. Write custom EventArgs for transmitting TextBox.Text value.
        Public Class TextEventArgs
            Inherits EventArgs

            Private _text As String

            <ScriptableMember()> _
            Public Property [Text]() As String
                Get
                    Return _text
                End Get
                Set(ByVal value As String)
                    _text = value
                End Set
            End Property
        End Class
        
    2. Write Event in MainPage code behind.
        <ScriptableMember()> _
        Public Event TextChanged As EventHandler(Of TextEventArgs)
    
    3. Fire event when silverlight TextBox.Text changed.
        Private Sub tb4_TextChanged(ByVal sender As Object, ByVal e As TextChangedEventArgs)
            Dim myargs = New TextEventArgs()
            myargs.Text = tb4.Text
            RaiseEvent TextChanged(Me, myargs)
        End Sub
        
    4. Register ScriptableObject instance in MainPage Loaded event.
        HtmlPage.RegisterScriptableObject("silverlightPage", Me)
        
    5. In silverlight hosted html page, attaching Silverlight loaded event.
        <param name="onLoad" value="onSilverlightLoaded" />

    6. In silverlight loaded eventhandler, handle managed code event.
        function onSilverlightLoaded() {
            var silverlight1 = document.getElementById('silverlight1');
            silverlight1.Content.silverlightPage.TextChanged = function(sender, e) {
                document.getElementById('Text4').value = e.Text;
            }
        }        


/////////////////////////////////////////////////////////////////////////////
References:

HTML Bridge: Interaction Between HTML and Managed Code
http://msdn.microsoft.com/en-us/library/cc645076(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
