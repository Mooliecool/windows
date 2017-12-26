//---------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All rights reserved.
//
// File: SolidColorBrush.cs
//
// Description: This file contains the implementation of SolidColorBrush.
//              The SolidColorBrush is the simplest of the Brushes. consisting
//              as it does of just a color.
//
// History:
//  04/28/2003 : [....] - Created it.
//
//---------------------------------------------------------------------------

using MS.Internal;
using MS.Internal.PresentationCore;
using System;
using System.IO;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using System.Windows.Markup;
using System.Windows.Media.Animation;
using System.Windows.Media.Composition;

using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

namespace System.Windows.Media 
{
    /// <summary>
    /// SolidColorBrush
    /// The SolidColorBrush is the simplest of the Brushes.  It can be used to 
    /// fill an area with a solid color, which can be animate.
    /// </summary>
    public sealed partial class SolidColorBrush : Brush
    {
        #region Constructors
        
        /// <summary>
        /// Default constructor for SolidColorBrush.
        /// </summary>
        public SolidColorBrush()
        {
        }

        /// <summary>
        /// SolidColorBrush - The constructor accepts the color of the brush
        /// </summary>
        /// <param name="color"> The color value. </param>
        public SolidColorBrush(Color color)
        {
            Color = color;
        }

        #endregion Constructors

        #region Serialization

        // This enum is used to identify brush types for deserialization in the 
        // ConvertCustomBinaryToObject method.  If we support more types of brushes,
        // then we may have to expose this publically and add more enum values.
        internal enum SerializationBrushType : byte
        {
            Unknown = 0,
            KnownSolidColor = 1,
            OtherColor = 2,
        }
        
        ///<summary>
        /// Serialize this object using the passed writer in compact BAML binary format.
        ///</summary>
        /// <remarks>
        /// This is called ONLY from the Parser and is not a general public method.  
        /// </remarks>
        /// <exception cref="System.ArgumentNullException">
        /// Thrown if "writer" is null.
        /// </exception>
        [FriendAccessAllowed] // Built into Core, also used by Framework.
        internal static bool SerializeOn(BinaryWriter writer, string stringValue)
        {
            // ********* VERY IMPORTANT NOTE *****************
            // If this method is changed, then XamlBrushSerilaizer.SerializeOn() needs
            // to be correspondingly changed as well. That code is linked into PBT.dll
            // and duplicates the code below to avoid pulling in SCB & base classes as well.
            // ********* VERY IMPORTANT NOTE *****************

            if (writer == null)
            {
                throw new ArgumentNullException("writer");
            }
            
            KnownColor knownColor = KnownColors.ColorStringToKnownColor(stringValue);
            if (knownColor != KnownColor.UnknownColor)
            {
                // Serialize values of the type "Red", "Blue" and other names
                writer.Write((byte)SerializationBrushType.KnownSolidColor);
                writer.Write((uint)knownColor);
                return true;
            }
            else
            {
                // Serialize values of the type "#F00", "#0000FF" and other hex color values.
                // We don't have a good way to check if this is valid without running the 
                // converter at this point, so just store the string if it has at least a
                // minimum length of 4.
                stringValue = stringValue.Trim();
                if (stringValue.Length > 3)
                {
                    writer.Write((byte)SerializationBrushType.OtherColor);
                    writer.Write(stringValue);
                    return true;
                }
            }
            return false;
        }

        ///<summary>
        /// Deserialize this object using the passed reader.  Throw an exception if
        /// the format is not a solid color brush.
        ///</summary>
        /// <remarks>
        /// This is called ONLY from the Parser and is not a general public method.  
        /// </remarks>
        /// <exception cref="System.ArgumentNullException">
        /// Thrown if "reader" is null.
        /// </exception>
        public static object DeserializeFrom(BinaryReader reader)
        {
            if (reader == null)
            {
                throw new ArgumentNullException("reader");
            }

            return DeserializeFrom(reader, null);
        }

        internal static object DeserializeFrom(BinaryReader reader, ITypeDescriptorContext context)
        {
            SerializationBrushType brushType = (SerializationBrushType)reader.ReadByte();

            if (brushType == SerializationBrushType.KnownSolidColor)
            {
                uint knownColorUint = reader.ReadUInt32();
                return KnownColors.SolidColorBrushFromUint(knownColorUint);
            }
            else if (brushType == SerializationBrushType.OtherColor)
            {
                string colorValue = reader.ReadString();
                BrushConverter converter = new BrushConverter();
                return converter.ConvertFromInvariantString(context, colorValue);
            }
            else
            {
                throw new Exception(SR.Get(SRID.BrushUnknownBamlType));
            }
        }

        #endregion Serialization

        #region ToString
        
        /// <summary>
        /// CanSerializeToString - an internal helper method which determines whether this object
        /// can fully serialize to a string with no data loss.
        /// </summary>
        /// <returns>
        /// bool - true if full fidelity serialization is possible, false if not.
        /// </returns>
        internal override bool CanSerializeToString()
        {
            if (HasAnimatedProperties
                || HasAnyExpression()
                || !Transform.IsIdentity
                || !DoubleUtil.AreClose(Opacity, Brush.c_Opacity))
            {
                return false;
            }
            
            return true;
        }

        /// <summary>
        /// Creates a string representation of this object based on the format string 
        /// and IFormatProvider passed in.  
        /// If the provider is null, the CurrentCulture is used.
        /// See the documentation for IFormattable for more information.
        /// </summary>
        /// <returns>
        /// A string representation of this object.
        /// </returns>
        internal override string ConvertToString(string format, IFormatProvider provider)
        {
            return Color.ConvertToString(format, provider);
        }

        #endregion
    }
}
