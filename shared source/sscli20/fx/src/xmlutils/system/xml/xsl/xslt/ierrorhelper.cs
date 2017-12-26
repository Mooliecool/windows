//------------------------------------------------------------------------------
// <copyright file="IErrorHelper.cs" company="Microsoft">
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

namespace System.Xml.Xsl {

    internal interface IErrorHelper {

        void ReportError(string res, params string[] args);

        void ReportWarning(string res, params string[] args);
    }
}
