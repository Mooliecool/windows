/****************************** Module Header ******************************\
* Module Name:                IConfigControl.cs
* Project:                    ConfigControl.Contract
* Copyright (c) Microsoft Corporation.
* 
* IConfigControl interface
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Reflection;
using System.ComponentModel.Composition;
using System.Windows;

namespace ConfigControl.Contract
{
    public interface IConfigControl
    {
        FrameworkElement CreateView(PropertyInfo property);
        MatchResult MatchTest(PropertyInfo property);
    }

    public enum MatchResult
    {
        NotMatch,
        NotRecommended,
        Match,
        Recommended
    }
}
