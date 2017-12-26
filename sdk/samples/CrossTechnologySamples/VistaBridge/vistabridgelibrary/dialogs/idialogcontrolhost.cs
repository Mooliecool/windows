using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Indicates that the implementing class is a dialog that can host
    /// customizable dialog controls (subclasses of DialogControl).
    /// </summary>
    internal interface IDialogControlHost
    {
        // Handle notifications of pseudo-controls being added 
        // or removed from the collection.
        // PreFilter should throw if a control cannot 
        // be added/removed in the dialog's current state.
        // PostProcess should pass on changes to native control, 
        // if appropriate.

        bool IsCollectionChangeAllowed();
        void ApplyCollectionChanged();

        // Handle notifications of individual child 
        // pseudo-controls' properties changing..
        // Prefilter should throw if the property 
        // cannot be set in the dialog's current state.
        // PostProcess should pass on changes to native control, 
        // if appropriate.
        bool IsControlPropertyChangeAllowed(string propertyName, DialogControl control);
        void ApplyControlPropertyChange(string propertyName, DialogControl control);
    }
}
