//------------------------------------------------------------------------------
// <copyright file="LocationUpdates.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Configuration {

    // 
    // LocationUpdates contains all the updates that share the same location characteristics.
    //
    internal class LocationUpdates {
        bool            _allowOverride;
        bool            _inheritInChildApps;
        SectionUpdates  _sectionUpdates;        // root of section

        internal LocationUpdates(bool allowOverride, bool inheritInChildApps) {
            _allowOverride = allowOverride;
            _inheritInChildApps = inheritInChildApps;
            _sectionUpdates = new SectionUpdates(string.Empty);
        }

        internal bool AllowOverride {
            get {return _allowOverride;}
        }

        internal bool InheritInChildApps {
            get {return _inheritInChildApps;}
        }

        internal SectionUpdates SectionUpdates {
            get {return _sectionUpdates;}
        }

        internal bool IsDefault {
            get {
                return _allowOverride && _inheritInChildApps;
            }
        }

        internal void CompleteUpdates() {
            _sectionUpdates.CompleteUpdates();
        }
    }
}
