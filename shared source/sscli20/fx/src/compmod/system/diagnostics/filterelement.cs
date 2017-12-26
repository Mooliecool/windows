//------------------------------------------------------------------------------
// <copyright file="FilterElement.cs" company="Microsoft Corporation">
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
using System.Configuration;
using System;

namespace System.Diagnostics {
    internal class FilterElement : TypedElement {
        public FilterElement() : base(typeof(TraceFilter)) {}

        public TraceFilter GetRuntimeObject() {
            TraceFilter newFilter = (TraceFilter) BaseGetRuntimeObject();
            newFilter.initializeData = InitData;
            return newFilter;
        }

        internal TraceFilter RefreshRuntimeObject(TraceFilter filter) {
            if (Type.GetType(TypeName) != filter.GetType() || InitData != filter.initializeData) {
                // type or initdata changed
                _runtimeObject = null;
                return GetRuntimeObject();
            }
            else {
                return filter;
            }
        }
    }
}


