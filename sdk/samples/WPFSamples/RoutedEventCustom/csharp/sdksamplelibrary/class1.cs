using System;
using System.Windows;
using System.Windows.Controls;

namespace SDKSample
{
    public class MyButtonSimple: Button
    {
        // Create a custom routed event by first registering a RoutedEventID
        // This event uses the bubbling routing strategy
        public static readonly RoutedEvent TapEvent = EventManager.RegisterRoutedEvent(
            "Tap", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(MyButtonSimple));

        // Provide CLR accessors for the event
        public event RoutedEventHandler Tap
        {
                add { AddHandler(TapEvent, value); } 
                remove { RemoveHandler(TapEvent, value); }
        }

        // This method raises the Tap event
        void RaiseTapEvent()
        {
                RoutedEventArgs newEventArgs = new RoutedEventArgs(MyButtonSimple.TapEvent);
                RaiseEvent(newEventArgs);
        }
        // For demonstration purposes we raise the event when the MyButtonSimple is clicked
        protected override void OnClick()
        {
            RaiseTapEvent();
        }

    }
}
