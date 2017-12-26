//------------------------------------------------------------------------------
// <copyright file="ConfigDefinitionUpdates.cs" company="Microsoft">
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
    using System.Collections;

    //
    // Contains all the updates to section definitions across all location sections.
    //
    internal class ConfigDefinitionUpdates {
        private ArrayList   _locationUpdatesList;
        private bool        _requireLocationWritten;

        internal ConfigDefinitionUpdates() {
            _locationUpdatesList = new ArrayList();
        }

        //
        // Find the location update with a certain set of location attributes.
        //
        internal LocationUpdates FindLocationUpdates(bool allowOverride, bool inheritInChildApps) {
            foreach (LocationUpdates locationUpdates in _locationUpdatesList) {
                if (locationUpdates.AllowOverride == allowOverride && locationUpdates.InheritInChildApps == inheritInChildApps) {
                    return locationUpdates;
                }
            }

            return null;
        }

        //
        // Add a section definition update to the correct location update.
        //
        internal DefinitionUpdate AddUpdate(bool allowOverride, bool inheritInChildApps, bool moved, string updatedXml, SectionRecord sectionRecord) {
            LocationUpdates locationUpdates = FindLocationUpdates(allowOverride, inheritInChildApps);
            if (locationUpdates == null) {
                locationUpdates = new LocationUpdates(allowOverride, inheritInChildApps);
                _locationUpdatesList.Add(locationUpdates);
            }

            DefinitionUpdate definitionUpdate = new DefinitionUpdate(sectionRecord.ConfigKey, moved, updatedXml, sectionRecord);
            locationUpdates.SectionUpdates.AddSection(definitionUpdate);
            return definitionUpdate;
        }

        //
        // Determine which section definition updates are new.
        //
        internal void CompleteUpdates() {
            foreach (LocationUpdates locationUpdates in _locationUpdatesList) {
                locationUpdates.CompleteUpdates();
            }
        }

        internal ArrayList LocationUpdatesList {
            get {return _locationUpdatesList;}
        }

        internal bool RequireLocation {
            get { return _requireLocationWritten; }
            set { _requireLocationWritten = value; }
        }

        internal void FlagLocationWritten() {
            _requireLocationWritten = false;
        }
    }
}
