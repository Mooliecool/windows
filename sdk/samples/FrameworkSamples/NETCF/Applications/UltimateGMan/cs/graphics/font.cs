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
using System.Runtime.InteropServices;

#if DESKTOP
using Microsoft.DirectX.Direct3D;
#else
using Microsoft.WindowsMobile.DirectX;
using Microsoft.WindowsMobile.DirectX.Direct3D;
#endif

namespace GraphicsLibrary
{
    /// <summary>
    /// Draw modes
    /// </summary>
    [FlagsAttribute]
    public enum FontDrawOptions
    {
        None = 0,
        DrawTextLeft = 0x0001,
        DrawTextCenter = 0x0002,
        DrawTextRight = 0x0004
    }


    public interface IFont
    {
        void Dispose();
    }

    /// <summary>
    /// Summary description for Font.
    /// </summary>
    public class GdiFont : IFont
    {
        System.Drawing.Font fontValue = null;

        /// <summary>
        /// Gets the GDI font associated with this font data.
        /// </summary>
        public System.Drawing.Font Font { get { return fontValue; } }

        /// <summary>
        /// Allocate and load a font.
        /// </summary>
        /// <param name="fontName">Name of font family</param>
        public GdiFont(string fontName)
        {
            fontValue = new System.Drawing.Font(fontName, 12,
                System.Drawing.FontStyle.Regular);
        }

        public void Dispose()
        {
            if (fontValue != null)
                fontValue.Dispose();
        }
    }

    public class DirectXFont : IFont
    {
        /// <summary>
        /// Gets the Direct3D font associated with this font data.
        /// </summary>
        Font fontValue = null;
        public Font Font { get { return fontValue; } }

        /// <summary>
        /// Allocate and load a font.
        /// </summary>
        /// <param name="fontName">Name of font family</param>
        public DirectXFont(string fontName, Device device)
        {
            System.Drawing.Font gdiFont = new System.Drawing.Font(
                fontName, 12, System.Drawing.FontStyle.Regular);
            fontValue = new Font(device, gdiFont);
        }

        public void Dispose()
        {
            if (fontValue != null)
                fontValue.Dispose();
        }
    }

}
