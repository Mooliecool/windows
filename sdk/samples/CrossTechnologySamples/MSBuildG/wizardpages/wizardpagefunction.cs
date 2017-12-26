// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Navigation;
using System.Windows.Controls;


namespace Microsoft.Samples.MSBuildG
{


    // Summary:
    // Custom PageFunction deriving from PageFunction<BuildProject> which allows for custom attributes on Wizard Pages
    public class WizardPageFunction : PageFunction<BuildProject>
    {

        //Overridden code because PageFunctions are broken
        public delegate void ReturnEventDelegate(object sender,ReturnEventArgs<BuildProject> e);
        public event ReturnEventDelegate ReturnTemp;

        protected override void OnReturn(ReturnEventArgs<BuildProject> e)
        {
            if (ReturnTemp != null)
            {
                ReturnTemp(this, e);
            }
        }

        public WizardPageFunction()
        {
            //Keep the page in memory after we have passed it so we can come back to it if we need to do so
            this.KeepAlive = true;            
        }

        // Summary:
        // DependencyProperty for the Instructions property (used by XAML)
        
        public static readonly DependencyProperty InstructionsProperty = DependencyProperty.Register("Instructions", typeof(string), typeof(WizardPageFunction), new PropertyMetadata(""));

        public string Instructions
        {
            get { return GetValue(InstructionsProperty).ToString(); }
            set 
            { 
                SetValue(InstructionsProperty, value);
            }

        }

        // Summary:
        // DependencyProperty for the Title property (used by XAML)
        
        public static readonly new DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(string), typeof(WizardPageFunction), new PropertyMetadata(""));

        public new string Title
        {
            get { return GetValue(TitleProperty).ToString(); }
            set
            {
                SetValue(TitleProperty, value);
            }

        }

        // Summary:
        // Function to be overridden to execute code when the Wizard Page is to be started.
        
        public virtual void Initialize(BuildProject currentObject)
        {
        }

        // Summary:
        // Function to be supplemented by the parent Wizard Page to conduct error checking before continuing in the wizard. 
        // Once error checking is complete, this function then tells the Wizard to move forward.
        
        public virtual void Done(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }
          
            HideErrorBubble();

            OnReturn(new ReturnEventArgs<BuildProject>(currentObject));
        }

        private void HideErrorBubble()
        {
            Grid errorBubble = (Grid)this.GetTemplateChild("ErrorBubble");
            errorBubble.Visibility = Visibility.Collapsed;
        }

        // Summary:
        // Displays an error bubble on the Wizard Page to indicate a problem or missing information.
        
        protected void ShowErrorBubble(WizardErrorMessage message)
        {
            if (message == null)
            {
                throw new ArgumentNullException("message");
            }

            Grid errorBubble = (Grid)this.GetTemplateChild("ErrorBubble");

            TextBlock errorText = (TextBlock)this.GetTemplateChild("ErrorBubbleTitle");
            errorText.Text = message.Caption;

            TextBlock errorContent = (TextBlock)this.GetTemplateChild("ErrorBubbleContent");
            errorContent.Text = message.Content;

            errorBubble.Visibility = Visibility.Visible;
         
            message.Element.Focus();
        }

        // Summary:
        // Custom class for passing error information to the showErrorBubble function
        
        protected class WizardErrorMessage
        {
            private UIElement m_Element;

            public UIElement Element
            {
                get { return m_Element; }
            }
            private string m_Caption = "";

            public string Caption
            {
                get { return m_Caption; }
            }
            private string m_Content = "";

            public string Content
            {
                get { return m_Content; }
            }

            public WizardErrorMessage(UIElement element, string caption, string content)
            {
                if (element == null)
                {
                    throw new ArgumentNullException("element");
                }
                if (caption == null)
                {
                    throw new ArgumentNullException("caption");
                }
                if (content == null)
                {
                    throw new ArgumentNullException("content");
                }

                this.m_Element = element;
                this.m_Caption = caption;
                this.m_Content = content;
            }
        } 
    }
}
