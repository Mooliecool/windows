//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.IO;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

#if !DESKTOP
using Microsoft.WindowsMobile.DirectX;
using Microsoft.WindowsMobile.DirectX.Direct3D;
#else
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
#endif

namespace GraphicsLibrary
{

    public interface IBitmap : IDisposable
    {
        /// <summary>
        /// Get the color of the image which will be transparent
        /// </summary>
        Color SourceKey { get; }

        /// <summary>
        /// Gets the width of the image
        /// </summary>
        int Width { get; }

        /// <summary>
        /// Gets the height of the image
        /// </summary>
        int Height { get; }

        /// <summary>
        /// True if the object should be drawn with alpha transparency
        /// </summary>
        bool Transparent { get; }
    }

    /// <summary>
    /// Summary description for Bitmap.
    /// </summary>
    public class GdiBitmap : IBitmap
    {
        /// <summary>
        /// Access the internal GDI representation of the Bitmap object
        /// </summary>
        internal Bitmap Image { get { return bmp; } }

        /// <summary>
        /// Get the color of the images source key.
        /// </summary>
        public Color SourceKey { get { return sourceKeyValue; } }
        Color sourceKeyValue = Color.Empty;

        /// <summary>
        /// True if the image should be drawn with alpha transparency
        /// </summary>
        public bool Transparent { get { return transparentValue; } }
        bool transparentValue;

        /// <summary>
        /// Internal GDI representation of the image.
        /// </summary>
        Bitmap bmp = null;

        /// <summary>
        /// Get the width of this image.
        /// </summary>
        public int Width { get { return bmp.Width; } }

        /// <summary>
        /// Get the height of this image.
        /// </summary>
        public int Height { get { return bmp.Height; } }

        /// <summary>
        /// Allocate and load an image from the specified file.
        /// </summary>
        /// <param name="fileName">Name of image file</param>
        public GdiBitmap(string fileName, bool transparent)
        {
            bmp = new Bitmap(fileName);
            transparentValue = transparent;
            if (transparent)
                sourceKeyValue = bmp.GetPixel(0, 0);
        }

        /// <summary>
        /// Free any resources allocated for the image.
        /// </summary>
        public void Dispose()
        {
            bmp.Dispose();
        }
    }

    /// <summary>
    /// Summary description for Bitmap.
    /// </summary>
    public class DirectXBitmap : IBitmap
    {

        /// <summary>
        /// True if the image should be drawn with alpha transparency
        /// </summary>
        public bool Transparent { get { return transparentValue; } }
        bool transparentValue;

        /// <summary>
        /// Get the color of the images source key.
        /// </summary>
        public Color SourceKey { get { return sourceKey; } }
        Color sourceKey = Color.Empty;

        /// <summary>
        /// The original width of the image
        /// </summary>
        int widthValue;

        /// <summary>
        /// The original height of the image
        /// </summary>
        int heightValue;

        /// <summary>
        /// The texture object holding this image
        /// </summary>
        public Texture Texture { get { return tex; } }
        Texture tex;


        /// <summary>
        /// Get the width of this image.
        /// </summary>
        public int Width { get { return widthValue; } }

        /// <summary>
        /// Get the height of this image.
        /// </summary>
        public int Height { get { return heightValue; } }

        /// <summary>
        /// Allocate and load an image from the specified file.
        /// </summary>
        /// <param name="fileName">Name of image file</param>
        /// <param name="transparent">True if the image should have
        /// transparency</param>
        public DirectXBitmap(string fileName, Device dev, bool transparent)
        {
            Bitmap bmp = new Bitmap(fileName);
            widthValue = bmp.Width;
            heightValue = bmp.Height;
            if (transparent)
                sourceKey = bmp.GetPixel(0, 0);
            transparentValue = transparent;
            bmp.Dispose();
            bmp = null;

#if DESKTOP
            Format format = Format.A8R8G8B8;
            Pool pool = Pool.Default;
#else
            Format format = Format.A1R5G5B5;
            Pool pool = Pool.VideoMemory;
#endif
            tex = TextureLoader.FromFile(dev, fileName, Width, Height, 1,
                Usage.None, format, pool, Filter.Linear, Filter.None,
                SourceKey.ToArgb());
        }

        /// <summary>
        /// Free any resources allocated for the image.
        /// </summary>
        public void Dispose()
        {
            if (tex != null)
            {
                tex.Dispose();
                tex = null;
            }

        }


    }
}
