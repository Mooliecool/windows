using System;

namespace System.Windows.Automation
{
    ///
    public static class AutomationProperties
    {
        #region AutomationId
        /// <summary>
        /// AutomationId Property
        /// </summary>
        public static readonly DependencyProperty AutomationIdProperty =
                    DependencyProperty.RegisterAttached(
                                "AutomationId",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting AutomationId property on a DependencyObject. 
        /// </summary>
        public static void SetAutomationId(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(AutomationIdProperty, value);
        }

        /// <summary>
        /// Helper for reading AutomationId property from a DependencyObject.
        /// </summary>
        public static string GetAutomationId(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(AutomationIdProperty));
        }
        #endregion AutomationId

        #region Name
        /// <summary>
        /// Name Property
        /// </summary>
        public static readonly DependencyProperty NameProperty =
                    DependencyProperty.RegisterAttached(
                                "Name",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting Name property on a DependencyObject. 
        /// </summary>
        public static void SetName(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(NameProperty, value);
        }

        /// <summary>
        /// Helper for reading Name property from a DependencyObject.
        /// </summary>
        public static string GetName(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(NameProperty));
        }
        #endregion Name

        #region HelpText
        /// <summary>
        /// HelpText Property
        /// </summary>
        public static readonly DependencyProperty HelpTextProperty =
                    DependencyProperty.RegisterAttached(
                                "HelpText",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting HelpText property on a DependencyObject. 
        /// </summary>
        public static void SetHelpText(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(HelpTextProperty, value);
        }

        /// <summary>
        /// Helper for reading HelpText property from a DependencyObject.
        /// </summary>
        public static string GetHelpText(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(HelpTextProperty));
        }
        #endregion HelpText

        #region AcceleratorKey
        /// <summary>
        /// AcceleratorKey Property
        /// </summary>
        public static readonly DependencyProperty AcceleratorKeyProperty =
                    DependencyProperty.RegisterAttached(
                                "AcceleratorKey",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting AcceleratorKey property on a DependencyObject. 
        /// </summary>
        public static void SetAcceleratorKey(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(AcceleratorKeyProperty, value);
        }

        /// <summary>
        /// Helper for reading AcceleratorKey property from a DependencyObject.
        /// </summary>
        public static string GetAcceleratorKey(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(AcceleratorKeyProperty));
        }
        #endregion AcceleratorKey

        #region AccessKey
        /// <summary>
        /// AccessKey Property
        /// </summary>
        public static readonly DependencyProperty AccessKeyProperty =
                    DependencyProperty.RegisterAttached(
                                "AccessKey",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting AccessKey property on a DependencyObject. 
        /// </summary>
        public static void SetAccessKey(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(AccessKeyProperty, value);
        }

        /// <summary>
        /// Helper for reading AccessKey property from a DependencyObject.
        /// </summary>
        public static string GetAccessKey(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(AccessKeyProperty));
        }
        #endregion AccessKey

        #region ItemStatus
        /// <summary>
        /// ItemStatus Property
        /// </summary>
        public static readonly DependencyProperty ItemStatusProperty =
                    DependencyProperty.RegisterAttached(
                                "ItemStatus",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting ItemStatus property on a DependencyObject. 
        /// </summary>
        public static void SetItemStatus(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(ItemStatusProperty, value);
        }

        /// <summary>
        /// Helper for reading ItemStatus property from a DependencyObject.
        /// </summary>
        public static string GetItemStatus(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(ItemStatusProperty));
        }
        #endregion ItemStatus

        #region ItemType
        /// <summary>
        /// ItemType Property
        /// </summary>
        public static readonly DependencyProperty ItemTypeProperty =
                    DependencyProperty.RegisterAttached(
                                "ItemType",
                                typeof(string),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(string.Empty),
                                new ValidateValueCallback(IsNotNull));

        /// <summary>
        /// Helper for setting ItemType property on a DependencyObject. 
        /// </summary>
        public static void SetItemType(DependencyObject element, string value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(ItemTypeProperty, value);
        }

        /// <summary>
        /// Helper for reading ItemType property from a DependencyObject.
        /// </summary>
        public static string GetItemType(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((string)element.GetValue(ItemTypeProperty));
        }
        #endregion ItemType

        #region IsColumnHeader
        /// <summary>
        /// IsColumnHeader Property
        /// </summary>
        public static readonly DependencyProperty IsColumnHeaderProperty =
                    DependencyProperty.RegisterAttached(
                                "IsColumnHeader",
                                typeof(bool),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(MS.Internal.KnownBoxes.BooleanBoxes.FalseBox));

        /// <summary>
        /// Helper for setting IsColumnHeader property on a DependencyObject. 
        /// </summary>
        public static void SetIsColumnHeader(DependencyObject element, bool value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(IsColumnHeaderProperty, value);
        }

        /// <summary>
        /// Helper for reading IsColumnHeader property from a DependencyObject.
        /// </summary>
        public static bool GetIsColumnHeader(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((bool)element.GetValue(IsColumnHeaderProperty));
        }
        #endregion IsColumnHeader

        #region IsRowHeader
        /// <summary>
        /// IsRowHeader Property
        /// </summary>
        public static readonly DependencyProperty IsRowHeaderProperty =
                    DependencyProperty.RegisterAttached(
                                "IsRowHeader",
                                typeof(bool),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(MS.Internal.KnownBoxes.BooleanBoxes.FalseBox));

        /// <summary>
        /// Helper for setting IsRowHeader property on a DependencyObject. 
        /// </summary>
        public static void SetIsRowHeader(DependencyObject element, bool value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(IsRowHeaderProperty, value);
        }

        /// <summary>
        /// Helper for reading IsRowHeader property from a DependencyObject.
        /// </summary>
        public static bool GetIsRowHeader(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((bool)element.GetValue(IsRowHeaderProperty));
        }
        #endregion IsRowHeader

        #region IsRequiredForForm
        /// <summary>
        /// IsRequiredForForm Property
        /// </summary>
        public static readonly DependencyProperty IsRequiredForFormProperty =
                    DependencyProperty.RegisterAttached(
                                "IsRequiredForForm",
                                typeof(bool),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(MS.Internal.KnownBoxes.BooleanBoxes.FalseBox));

        /// <summary>
        /// Helper for setting IsRequiredForForm property on a DependencyObject. 
        /// </summary>
        public static void SetIsRequiredForForm(DependencyObject element, bool value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(IsRequiredForFormProperty, value);
        }

        /// <summary>
        /// Helper for reading IsRequiredForForm property from a DependencyObject.
        /// </summary>
        public static bool GetIsRequiredForForm(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((bool)element.GetValue(IsRequiredForFormProperty));
        }
        #endregion IsRequiredForForm

        #region LabeledBy
        /// <summary>
        /// LabeledBy Property
        /// </summary>
        public static readonly DependencyProperty LabeledByProperty =
                    DependencyProperty.RegisterAttached(
                                "LabeledBy",
                                typeof(UIElement),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata((UIElement)null));

        /// <summary>
        /// Helper for setting LabeledBy property on a DependencyObject. 
        /// </summary>
        public static void SetLabeledBy(DependencyObject element, UIElement value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(LabeledByProperty, value);
        }

        /// <summary>
        /// Helper for reading LabeledBy property from a DependencyObject.
        /// </summary>
        public static UIElement GetLabeledBy(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((UIElement)element.GetValue(LabeledByProperty));
        }
        #endregion LabeledBy

        #region IsOffscreenBehavior
        /// <summary>
        /// IsOffscreenBehavior Property
        /// </summary>
        public static readonly DependencyProperty IsOffscreenBehaviorProperty =
                    DependencyProperty.RegisterAttached(
                                "IsOffscreenBehavior",
                                typeof(IsOffscreenBehavior),
                                typeof(AutomationProperties),
                                new UIPropertyMetadata(IsOffscreenBehavior.Default));

        /// <summary>
        /// Helper for setting IsOffscreenBehavior property on a DependencyObject. 
        /// </summary>
        public static void SetIsOffscreenBehavior(DependencyObject element, IsOffscreenBehavior value)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            element.SetValue(IsOffscreenBehaviorProperty, value);
        }

        /// <summary>
        /// Helper for reading IsOffscreenBehavior property from a DependencyObject.
        /// </summary>
        public static IsOffscreenBehavior GetIsOffscreenBehavior(DependencyObject element)
        {
            if (element == null)
            {
                throw new ArgumentNullException("element");
            }

            return ((IsOffscreenBehavior)element.GetValue(IsOffscreenBehaviorProperty));
        }
        #endregion IsOffscreenBehavior

        #region private implementation
        // Validation callback for string properties
        private static bool IsNotNull(object value)
        {
            return (value != null);
        }
        #endregion
    }
}

