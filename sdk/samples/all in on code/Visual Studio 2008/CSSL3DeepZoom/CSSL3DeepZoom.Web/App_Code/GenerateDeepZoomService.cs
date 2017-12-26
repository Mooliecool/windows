/****************************** Module Header ******************************\
* Module Name:  GenerateDeepZoomService.cs
* Project:      DeepZoomProjectSite
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to generate the deep zoom content programmatically in Silverlight using C#. It wraps the functionality in a WCF service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/27/2009 15:40 Yilun Luo Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;
using System.IO;
using System.ServiceModel.Activation;
using System.Web;
using System.Net;
using Microsoft.DeepZoomTools;
using System.Xml.Linq;
using System.Windows;

// NOTE: If you change the class name "GenerateDeepZoomService" here, you must also update the reference to "GenerateDeepZoomService" in Web.config.
[AspNetCompatibilityRequirements(RequirementsMode = AspNetCompatibilityRequirementsMode.Allowed)]
public class GenerateDeepZoomService : IGenerateDeepZoomService
{

	public bool PrepareDeepZoom(bool forceGenerateDeepZoom)
	{
		if(!Directory.Exists(HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/dzc_output_images")) || forceGenerateDeepZoom)
		{
			try
			{
				this.CreateDeepZoom();
			}
			catch
			{
				return false;
			}
		}
		return true;
	}

	/// <summary>
	/// Generate the deep zoom content using a CollectionCreator.
	/// </summary>
	private void CreateDeepZoom()
	{
		CollectionCreator creator = new CollectionCreator();
		List<Image> images = new List<Image>();
		XDocument doc = XDocument.Load(HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/Metadata.xml"));
		var imageElements = doc.Root.Elements("Image");
		double aspectRatio = double.Parse(doc.Root.Element("AspectRatio").Value);
		//Populates a list of Microsoft.DeepZoomTools.Image objects using the value provided in Metadata.xml.
		foreach (XElement imageElement in imageElements)
		{
			int zOrder = int.Parse(imageElement.Element("ZOrder").Value);
			double width = 1d / double.Parse(imageElement.Element("Width").Value);
			images.Add(new Image(HttpContext.Current.Server.MapPath("~/SourceImages/" + imageElement.Element("FileName").Value))
			{
				ViewportWidth = width,
				ViewportOrigin = new Point(double.Parse(imageElement.Element("x").Value) * -width, double.Parse(imageElement.Element("y").Value) * -width / aspectRatio),
			});
		}
		creator.Create(images, HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/dzc_output"));
	}
}
