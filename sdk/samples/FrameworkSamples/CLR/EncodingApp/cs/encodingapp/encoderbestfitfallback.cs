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
using System.Text;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// Best fit fallback defining custom encoding
	/// </summary>
	public class EncoderBestFitFallback :EncoderFallback
	{

		private Encoding parentEncoding=null;

		public EncoderBestFitFallback(Encoding targetEncoding)
		{
			this.ParentEncoding = targetEncoding;
		}
    
		public Encoding ParentEncoding
		{
			set
			{
				this.parentEncoding = (Encoding)value.Clone();
				this.parentEncoding.EncoderFallback=new EncoderReplacementFallback("");
				this.parentEncoding.DecoderFallback=new DecoderReplacementFallback("");
			}

			get
			{
				return this.parentEncoding;
			}            
		}

		// Get maximum character count 
		public override int MaxCharCount
		{
			get
			{
				return 18;
			}
		}

		public override EncoderFallbackBuffer CreateFallbackBuffer()
		{
			return new EncoderBestFitFallbackBuffer(this);
		}
	}

	/// <summary>
	/// Class to encapsulate the buffer for best fit fallback
	/// </summary>
	public sealed class EncoderBestFitFallbackBuffer: EncoderFallbackBuffer
	{
		private String strFallback = String.Empty;
		private int charIndex = 1;
		private Encoding parentEncoding;

		// Constructor
		public EncoderBestFitFallbackBuffer(EncoderBestFitFallback fallback)
		{
			parentEncoding = fallback.ParentEncoding;
		}

		// Fallback methods
		public override bool Fallback(char charUnknown, int index)
		{
			// Since both fallback methods require normalizing a string, make a string out of our char
			String strUnknown = new String(charUnknown, 1);

			return Fallback(strUnknown);
		}

		public override bool Fallback(char charUnknownHigh, char charUnknownLow, int index)
		{
			// Since both fallback methods require normalizing a string, make a string out of our chars
			String strUnknown = new String(new char[] { charUnknownHigh, charUnknownLow });

			return Fallback(strUnknown);
		}

		private bool Fallback(String strUnknown)
		{
			// If they were falling back already they need to fail
			if (charIndex <= strFallback.Length)
			{
				// Throw it, using our complete character
				charIndex = 1;
				strFallback = String.Empty;
				throw new ArgumentException("Unexpected recursive fallback", "chars");
			}

			// They need to fallback our character
			String strNormal = "";
			try
			{
				// Normalize our character
				strNormal = strUnknown.Normalize(NormalizationForm.FormKD);

				// If it didn't change we'll still have to be ?
				if (strNormal == strUnknown)
				{
					strNormal = "";
				}
			}
				// Illegal Unicode can throw a normalization exception
			catch (ArgumentException)
			{
				// Allow the string to become a ? fallback
			}

			// Try encoding the normalized string
			strFallback = parentEncoding.GetString(parentEncoding.GetBytes(strNormal));

			// Any data from this fallback?
			if ((strFallback.Length == 0) || (strFallback[0] != strNormal[0]))
			{
				// We didn't have data or didn't have the first character, so use ?
				strFallback = "?";
			}

			charIndex = 0;

			return true;
		}

		// Default version is overridden in EncoderReplacementFallback.cs
		public override char GetNextChar()
		{
			// If we're above string length then return nothing
			if (charIndex >= strFallback.Length)
			{
				// We use charIndex = length + 1 as a flag to tell us we're long past done.
				if (charIndex == strFallback.Length)
					charIndex++;
				return '\0';
			}

			// Return the character at our charIndex (& increment it)
			return strFallback[charIndex++];
		}

		// Caller needs to back up for some reason
		public override bool MovePrevious()
		{
			// Back up only if we're within the string or just did the last char
			if (charIndex <= strFallback.Length)
				charIndex--;

			// Return true if we were able to move previous
			return (charIndex >= 0 || charIndex < strFallback.Length);
		}


		// How many characters left to output?
		public override int Remaining
		{
			get
			{
				// Any characters remaining?  If so, how many?
				if (charIndex < strFallback.Length)
					return strFallback.Length - charIndex;

				return 0;
			}
		}

		// Clear the buffer
		public override void Reset()
		{
			strFallback = String.Empty;
			charIndex = 1;
		}

	}
}
