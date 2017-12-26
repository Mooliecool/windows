using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Library;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Strongly typed collection for dialog controls.
    /// </summary>
    /// <typeparam name="T">DialogControl</typeparam>
    public sealed class DialogControlCollection<T> : Collection<T> where T : DialogControl
    {
        private IDialogControlHost hostingDialog;

        internal DialogControlCollection(IDialogControlHost host)
        {
            hostingDialog = host;
        }

        /// <summary>
        /// Inserts an dialog control at the specified index.
        /// </summary>
        /// <param name="index">The location to insert the control.</param>
        /// <param name="control">The item to insert.</param>
        /// <permission cref="System.InvalidOperationException">A control with 
        /// the same name already exists in this collection -or- 
        /// the control is being hosted by another dialog -or- the associated dialog is 
        /// showing and cannot be modified.</permission>
        protected override void InsertItem(int index, T control)
        {
            // Check for duplicates, lack of host, 
            // and during-show adds.
            if (Items.Contains(control))
                throw new InvalidOperationException(
                    "Dialog cannot have more than one control with the same name.");
            if (control.HostingDialog != null)
                throw new InvalidOperationException(
                    "Dialog control must be removed from current collections first.");
            if (!hostingDialog.IsCollectionChangeAllowed())
                throw new InvalidOperationException(
                    "Modifying controls collection while dialog is showing is not supported.");

            // Reparent, add control.
            control.HostingDialog = hostingDialog;
            base.InsertItem(index, control);

            // Notify that we've added a control.
            hostingDialog.ApplyCollectionChanged();
        }
        /// <summary>
        /// Removes the control at the specified index.
        /// </summary>
        /// <param name="index">The location of the control to remove.</param>
        /// <permission cref="System.InvalidOperationException">
        /// The associated dialog is 
        /// showing and cannot be modified.</permission>
        protected override void RemoveItem(int index)
        {
            // Notify that we're about to remove a control.
            // Throw if dialog showing.
            if (!hostingDialog.IsCollectionChangeAllowed())
                throw new InvalidOperationException(
                    "Modifying controls collection while dialog is showing is not supported.");

            DialogControl control = (DialogControl)Items[index];

            // Unparent and remove.
            control.HostingDialog = null;
            base.RemoveItem(index);

            hostingDialog.ApplyCollectionChanged();
        }
        /// <summary>
        /// Defines the indexer that supports accessing controls by name. 
        /// </summary>
        /// <remarks>
        /// <para>Control names are case sensitive.</para>
        /// <para>This indexer is useful when the dialog is created in XAML
        /// rather than constructed in code.</para></remarks>
        ///<exception cref="System.ArgumentException">
        /// The name cannot be null or a zero-length string.</exception>
        public T this[string name]
        {
            get
            {
                if (String.IsNullOrEmpty(name))
                    throw new ArgumentException(
                        "Control name must not be null or zero length.");

                foreach (T control in base.Items)
                {
                    // NOTE: we don't ToLower() the strings - casing effects 
                    // hash codes, so we are case-sensitive.
                    if (control.Name == name)
                        return control;
                }
                return null;
            }
        }

        internal DialogControl GetControlbyId(int id)
        {
            foreach (DialogControl control in Items)
            {
                // Match?
                if (control.Id == id)
                    return control;
            }

            // Control id not found - likely an error, but the calling function
            // should ultimately decide.
            return null;
        }
    }
}
