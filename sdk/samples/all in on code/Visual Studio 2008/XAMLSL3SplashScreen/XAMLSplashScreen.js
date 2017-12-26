/****************************** Module Header ******************************\
* Module Name:  XAMLSplashScreen.js
* Project:      XAMLSL3SplashScreen
* Copyright (c) Microsoft Corporation.
* 
* This js file contains one JavaScript function. This function finds the 
* GradientStop objects and TextBlock control defined in SplashScreen.xaml, then
* updates their properties to show the download progress.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 8/5/2009 02:00 PM Allen Chen Created
\***************************************************************************/

function onSourceDownloadProgressChanged(sender, eventArgs) {
    sender.findName("uxStatus").Text = Math.round((eventArgs.progress * 1000)) / 10 + "%";
    sender.findName("uxGradientStop1").Offset = eventArgs.progress;
    sender.findName("uxGradientStop2").Offset = eventArgs.progress;
}