/****************************** Module Header ******************************\
* Module Name:  Sphere.cs
* Project:      CSWpf3DMultiTouch
* Copyright (c) Microsoft Corporation.
* 
* Creates a reusable sphere.
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
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace CSWPF3DMultiTouch
{
	public class Sphere : UIElement3D
	{
        /// <summary>
        /// The 3D model.
        /// </summary>
		private static readonly DependencyProperty ModelProperty = DependencyProperty.Register("Model", typeof(GeometryModel3D), typeof(Sphere), new PropertyMetadata(ModelPropertyChanged));
		private GeometryModel3D Model
		{
			get
			{
				return (GeometryModel3D)GetValue(ModelProperty);
			}

			set
			{
				SetValue(ModelProperty, value);
			}
		}

		private static void ModelPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Sphere s = (Sphere)d;
			s.Visual3DModel = s.Model;
		}

        /// <summary>
        /// Longitudes: Controls how many triangles should be created.
        /// The larger the value, the more accurate the sphere is, but also more resources is used, and takes longer to calculate.
        /// </summary>
		public static readonly DependencyProperty LongitudesProperty = DependencyProperty.Register("Longitudes", typeof(int), typeof(Sphere), new PropertyMetadata(36, new PropertyChangedCallback(LongitudesChanged)));
		public int Longitudes
		{
			get { return (int)GetValue(LongitudesProperty); }
			set { SetValue(LongitudesProperty, value); }
		}

		private static void LongitudesChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Sphere s = (Sphere)d;
			s.InvalidateModel();
		}

        /// <summary>
        /// Latitudes: Controls how many triangles should be created.
        /// The larger the value, the more accurate the sphere is, but also more resources is used, and takes longer to calculate.
        /// </summary>
		public static readonly DependencyProperty LatitudesProperty = DependencyProperty.Register("Latitudes", typeof(int), typeof(Sphere), new PropertyMetadata(18, new PropertyChangedCallback(LongitudesChanged)));
		public int Latitudes
		{
			get { return (int)GetValue(LatitudesProperty); }
			set { SetValue(LatitudesProperty, value); }
		}

		private static void LatitudesChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Sphere s = (Sphere)d;
			s.InvalidateModel();
		}

        /// <summary>
        /// Radius: Controls the x, y, z raduises. The larger the value, the flatter the sphere becomes.
        /// The default value 1 indicates x, y, z values are the same, which renders a ball.
        /// </summary>
		public static readonly DependencyProperty RadiusProperty = DependencyProperty.Register("Radius", typeof(int), typeof(Sphere), new PropertyMetadata(1, new PropertyChangedCallback(RadiusChanged)));
		public int Radius
		{
			get { return (int)GetValue(RadiusProperty); }
			set { SetValue(RadiusProperty, value); }
		}

		private static void RadiusChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Sphere s = (Sphere)d;
			s.InvalidateModel();
		}

        /// <summary>
        /// The 3D material.
        /// </summary>
		public static readonly DependencyProperty MaterialProperty = DependencyProperty.Register("Material", typeof(Material), typeof(Sphere), new PropertyMetadata(new DiffuseMaterial(Brushes.Aqua), new PropertyChangedCallback(MaterialChanged)));
		public int Material
		{
			get { return (int)GetValue(MaterialProperty); }
			set { SetValue(MaterialProperty, value); }
		}

		private static void MaterialChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Sphere s = (Sphere)d;
			if (s.Model != null)
			{
				s.Model.Material = (Material)e.NewValue;
			}
		}

		protected override void OnUpdateModel()
		{
			GeometryModel3D model = new GeometryModel3D();
			model.Geometry = Calculate(Latitudes, Longitudes, Radius);
			model.Material = (Material)GetValue(MaterialProperty);
			Model = model;
		}

        /// <summary>
        /// Generates the geometries that compose the sphere.
        /// </summary>
		private Geometry3D Calculate(int Latitudes, int Longitudes, int Radius)
		{
			MeshGeometry3D geometry = new MeshGeometry3D();
			for (int latitude = 0; latitude <= Latitudes; latitude++)
			{
				double phi = Math.PI / 2 - latitude * Math.PI / Latitudes;
				double y = Math.Sin(phi);
				double radius = -Math.Cos(phi);
				for (int longitude = 0; longitude <= Longitudes; longitude++)
				{
					double theta = longitude * 2 * Math.PI / Longitudes;
					double x = radius * Math.Sin(theta);
					double z = radius * Math.Cos(theta);
					geometry.Positions.Add(new Point3D(x, y, z));
					geometry.Normals.Add(new Vector3D(x, y, z));
					geometry.TextureCoordinates.Add(new Point((double)longitude / Longitudes, (double)latitude / Latitudes));
				}
			}
			for (int latitude = 0; latitude < Latitudes; latitude++)
			{
				for (int longitude = 0; longitude < Longitudes; longitude++)
				{
					geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude);
					geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude);
					geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude + 1);
					geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude + 1);
					geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude);
					geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude + 1);
				}
			}
			geometry.Freeze();
			return geometry;
		}
	}
}
