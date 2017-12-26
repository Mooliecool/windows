//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
//-----------------------------------------------------------------------

using System;
using System.Globalization;
using System.Text;

namespace Microsoft.VisualStudio.Samples.TextEncoding.EncodingApplication
{
	/// <summary>
	/// Class to encapsulate encoding options of Encoding type, Fallback mechanism and Normalization scheme
	/// </summary>
	public class EncoderInfo
	{
		public static string NormalizationNone = "None";
		//Different fallback schemes
		public static string[] FallbackSchemes = { "Replacement", "Exception", "Best Fit" };

		//Code page of encoding selected in options
		private int encodingCodePage = 0;

		//Encoding that we're using
		private Encoding encoding = Encoding.Default;

		//EncoderFallback selected in options
		private EncoderFallback encFallback = EncoderFallback.ReplacementFallback;

		//Normalization scheme selected in options
		private string normalizationForm;

		//Constructor initializing members to default values
		public EncoderInfo()
		{
			//Initialize encoding to UTF8
			SetEncodingCodePage(Encoding.UTF8.CodePage);

			//Initialize fallback scheme to replacement
			SetEncoderFallback(FallbackSchemes[0]);

			//Initialize normalization scheme to none
			SetNormalizationForm(NormalizationNone);

		}

		//Property to get the encoding scheme
		public Encoding Encoding
		{
			get
			{
				return (Encoding)this.encoding.Clone();
			}
		}

		//Method to set the code page of the selected encoding
		public void SetEncodingCodePage(int encodingCodePage)
		{
			this.encodingCodePage = encodingCodePage;
			this.encoding = Encoding.GetEncoding(encodingCodePage, encFallback, DecoderFallback.ReplacementFallback);
			EncoderBestFitFallback bestFit = encFallback as EncoderBestFitFallback;
			if (bestFit != null)            
			{
				bestFit.ParentEncoding = encoding;
			}            
		}

		//Method to set the fallback scheme of the destination encoding
		public void SetEncoderFallback(string strFallback)
		{            
			// Replacement
			if (strFallback == FallbackSchemes[0])
			{
				this.encFallback = new EncoderReplacementFallback("?");
			}
			// Exception            
			else if (strFallback == FallbackSchemes[1])
			{
				this.encFallback = new EncoderExceptionFallback();
			}
			// Best Fit
			else
			{
				this.encFallback = new EncoderBestFitFallback(this.encoding);
			}
			this.encoding.EncoderFallback = this.encFallback;            
		}

		//Method to set the normalization scheme
		public void SetNormalizationForm(string strNormalization)
		{
			normalizationForm = strNormalization;
		}

		//Method to normalize a given string in the normalization form specified
		public string GetNormalizedString(string inputString)
		{
			if (normalizationForm != NormalizationNone)
			{
				NormalizationForm n = (NormalizationForm) Enum.Parse(typeof(NormalizationForm), normalizationForm);
				//Normalize string
				inputString = inputString.Normalize(n);
			}

			return inputString;
		}
	}
}
