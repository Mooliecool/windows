/****************************** Module Header ******************************\
* Module Name:    ImagePreviewControl.cs
* Project:        CSASPNETAJAXImagePreviewExtender
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to design an AJAX Control Extender.
* In this sample, it is extender for images.
* The images which use the extender control will be shown in a thumbnail mode at
* first, if user click the image, a big picture will be popped up and show the
* true size of the image.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Xml.Linq;


namespace ImagePreviewExtender
{

    [TargetControlType(typeof(Control))]
    public class ImagePreviewControl : ExtenderControl
    {
        /// <summary>
        /// Define the css class which the image use in thumbnail mode.
        /// </summary>
        public string ThumbnailCssClass { get; set; }

        /// <summary>
        /// return the resource url of the close icon.
        /// </summary>
        private string closeImage
        {
            get
            {
                return Page.ClientScript.GetWebResourceUrl(this.GetType(),
                                        "ImagePreviewExtender.Close.png");
            }
        }

        protected override IEnumerable<ScriptDescriptor>
                GetScriptDescriptors(System.Web.UI.Control targetControl)
        {
            ScriptBehaviorDescriptor descriptor =
                new ScriptBehaviorDescriptor(
                    "ImagePreviewExtender.ImagePreviewBehavior",
                    targetControl.ClientID);

            descriptor.AddProperty("ThumbnailCssClass", ThumbnailCssClass);

            descriptor.AddProperty("closeImage", closeImage);
            yield return descriptor;
        }

        // Generate the script reference
        protected override IEnumerable<ScriptReference>
                GetScriptReferences()
        {
            yield return new ScriptReference(
                "ImagePreviewExtender.ImagePreviewBehavior.js",
                this.GetType().Assembly.FullName);
        }
    }
}