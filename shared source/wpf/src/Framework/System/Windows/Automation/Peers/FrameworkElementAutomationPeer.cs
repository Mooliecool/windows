using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Windows;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Markup;

using MS.Internal;
using MS.Win32;

namespace System.Windows.Automation.Peers
{
    ///
    public class FrameworkElementAutomationPeer : UIElementAutomationPeer
    {
        ///
        public FrameworkElementAutomationPeer(FrameworkElement owner)
            : base(owner)
        { }

        ///
        protected override string GetAutomationIdCore()
        {
            //  1. fetch AutomationProperties.AutomationIdProperty
            string result = base.GetAutomationIdCore();

            if (string.IsNullOrEmpty(result))
            {
                //  2. fetch x:Uid
                FrameworkElement owner = (FrameworkElement)Owner;
                result = Owner.Uid;

                if (string.IsNullOrEmpty(result))
                {
                    //  3. fetch FrameworkElement.NameProperty
                    result = owner.Name;
                }
            }

            return result ?? string.Empty;
        }

        ///
        override protected string GetNameCore()
        {
            string result = base.GetNameCore();
            if (string.IsNullOrEmpty(result))
            {
                AutomationPeer labelAutomationPeer = GetLabeledByCore();
                if (labelAutomationPeer != null)
                    result = labelAutomationPeer.GetName();

                if (string.IsNullOrEmpty(result))
                    result = ((FrameworkElement)Owner).GetPlainText();
            }

            return result ?? string.Empty;
        }

        ///
        protected override string GetHelpTextCore()
        {
            string result = base.GetHelpTextCore();
            if (string.IsNullOrEmpty(result))
            {
                object toolTip = ((FrameworkElement)Owner).ToolTip;
                if (toolTip != null)
                {
                    result = toolTip as string;
                    if (string.IsNullOrEmpty(result))
                    {
                        FrameworkElement toolTipElement = toolTip as FrameworkElement;
                        if (toolTipElement != null)
                            result = toolTipElement.GetPlainText();
                    }
                }
            }
            return result ?? String.Empty;
        }

    }
}

