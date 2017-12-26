=============================================================================
        SILVERLIGHT APPLICATION : CSSL3HTMLBridge Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This project create a group of samples demonstrating the interactivity 
between Silverlight applications and javascript. The samples include:

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
        HtmlPage.Window.Invoke("changetext", tb1.Text);

Sample 2: Handle Html event from managed code
    1. Attach html event in MainPage Loaded event handler.
        HtmlPage.Document.GetElementById("Text2").AttachEvent("onkeyup", new EventHandler(HtmlKeyUp));
    
    2. Write handler method "HtmlKeyUp".
        private void HtmlKeyUp(object sender, EventArgs e)
        {
                var value = ((HtmlElement)sender).GetProperty("value");
                tb2.Text = value.ToString();
        }

Sample 3: Call managed code method from javascript
    1. Write method in MainPage code behind and apply ScriptableMember attribute
        [ScriptableMember]
        public void ChangeTB3Text(string text)
        {
            tb3.Text = text;
        }
        
    2. Register ScriptableObject instance in MainPage Loaded event.
        HtmlPage.RegisterScriptableObject("silverlightPage", this);

    3. Write javascript event handler for html textbox onchanged event. In handler function, call managed code.
        function ontext3keydown() {
            var text = document.getElementById('Text3').value;
            // calling managed code method
            var silverlight1 = document.getElementById('silverlight1');
            silverlight1.Content.silverlightPage.ChangeTB3Text(text);
        }
    
Sample 4: Handle managed code event from javascript
    1. Write custom EventArgs for transmitting TextBox.Text value.
        public class TextEventArgs : EventArgs
        {
            [ScriptableMember]
            public string Text { set; get; }
        }
        
    2. Write Event in MainPage code behind.
        [ScriptableMember]
        public event EventHandler<TextEventArgs> TextChanged;
    
    3. Fire event when silverlight TextBox.Text changed.
        void tb4_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (TextChanged != null)
                TextChanged(this, new TextEventArgs { Text = tb4.Text });
        }

    4. Register ScriptableObject instance in MainPage Loaded event.
        HtmlPage.RegisterScriptableObject("silverlightPage", this);
        
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
