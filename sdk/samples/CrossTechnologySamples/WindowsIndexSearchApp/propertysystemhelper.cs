using System;
using System.Text;
using System.Data;
using System.Runtime.InteropServices;

namespace WinFormsSearchApp
{
	class PropertySystemHelper
	{
		// Defined in PropSys.h
		// PSSTDAPI PSEnumeratePropertyDescriptions(PROPDESC_ENUMFILTER filterOn, REFIID riid, VOID** ppv);
		[DllImport("propsys.dll")]
		public static extern int PSEnumeratePropertyDescriptions(
			int filterOn,
			[In] ref Guid riid,
			out IPropertyDescriptionList ppv);

		public static DataTable GetPropertyDefinitions()
		{
			DataTable dt = CreateTable();
			Guid riid;
			IPropertyDescriptionList propList = null;
			IPropertyDescription propDesc = null;

			try
			{
				riid = new Guid(IIDStrings.IPropertyDescriptionList);
				int hr = PSEnumeratePropertyDescriptions(0, // PDEF_ALL,
						ref riid,
						out propList);

				if (hr < 0) 
				{ 
					throw (new ApplicationException(string.Format("PSEnumeratePropertyDescriptions returned: {0}", hr))); 
				}

				uint numProps = propList.GetCount();
				riid = new Guid(IIDStrings.IPropertyDescription);

				for (int i = 0; i < numProps; i++)
				{
					// Iterates through each item in the description list 
					propDesc = propList.GetAt((uint)i, ref riid);

					string propName = propDesc.GetCanonicalName(); 

					DataRow dr = dt.Rows.Add(propName);
					InitRow(dr, propDesc);
					Marshal.ReleaseComObject(propDesc);
				}
			}
			finally
			{
				Marshal.ReleaseComObject(propList);
				Marshal.ReleaseComObject(propDesc);
			}

			return (dt);
		}

		static void InitRow(DataRow Row, IPropertyDescription propDesc)
		{
            IPropertyDescriptionSearchInfo searchInfo;
			uint flags = propDesc.GetPropertyType();

			Row["Type"] = TypeFlagToString(flags);
			flags = propDesc.GetTypeFlags(PropDescTypeFlags.PDTF_MASK_ALL);

			bool multiValued = (flags & (uint)PropDescTypeFlags.PDTF_MULTIPLEVALUES) != 0;
			Row["MultipleValues"] = (multiValued ? "True" : "");

			bool isViewable = (flags & (uint)PropDescTypeFlags.PDTF_ISVIEWABLE) != 0;
			Row["IsViewable"] = (isViewable ? "True" : "");

			bool isQueryable = (flags & (uint)PropDescTypeFlags.PDTF_ISVIEWABLE) != 0;
			Row["IsViewable"] = (isViewable ? "True" : "");

            // TODO: fix this - we're not able to get a IPDSI interface from our prop desc
            if (propDesc is IPropertyDescriptionSearchInfo)
            {
                searchInfo = (IPropertyDescriptionSearchInfo)propDesc;
                flags = (uint)searchInfo.GetSearchInfoFlags();
                bool isColumn = (flags & (uint)PROPDESC_SEARCHINFO_FLAGS.PDSIF_ISCOLUMN) != 0;
                Row["IsColumn"] = (isColumn ? "True" : "");
                Marshal.ReleaseComObject(searchInfo);
            }

			
		}

		static DataTable CreateTable()
		{
			DataTable dt = new DataTable("PropDefs");
			dt.Columns.Add("Name", typeof(string));
			dt.Columns.Add("Type", typeof(string));
			dt.Columns.Add("MultipleValues", typeof(string));
			dt.Columns.Add("InInvertedIndex", typeof(string));
			dt.Columns.Add("IsColumn", typeof(string));
			dt.Columns.Add("IsViewable", typeof(string));
			dt.Columns.Add("Scope", typeof(string));

			return (dt);
		}

		static string TypeFlagToString(uint TypeFlag)
		{
			string retVal = "";

			switch (TypeFlag & ~(uint)VarEnum.VT_VECTOR)
			{
				case (uint)VarEnum.VT_NULL:
					retVal = "Any";
					break;
				case (uint)VarEnum.VT_LPWSTR:
					retVal = "String";
					break;
				case (uint)VarEnum.VT_BOOL:
					retVal = "Boolean";
					break;
				case (uint)VarEnum.VT_UI1:
					retVal = "Byte";
					break;
				case (uint)VarEnum.VT_I2:
					retVal = "Int16";
					break;
				case (uint)VarEnum.VT_UI2:
					retVal = "UInt16";
					break;
				case (uint)VarEnum.VT_I4:
					retVal = "Int32";
					break;
				case (uint)VarEnum.VT_UI4:
					retVal = "UInt32";
					break;
				case (uint)VarEnum.VT_I8:
					retVal = "Int64";
					break;
				case (uint)VarEnum.VT_UI8:
					retVal = "UInt64";
					break;
				case (uint)VarEnum.VT_R8:
					retVal = "Double";
					break;
				case (uint)VarEnum.VT_FILETIME:
					retVal = "DateTime";
					break;
				case (uint)VarEnum.VT_CLSID:
					retVal = "Guid";
					break;
				case (uint)VarEnum.VT_BLOB:
					retVal = "Blob";
					break;
				case (uint)VarEnum.VT_UNKNOWN:
					retVal = "Object";
					break;
				case (uint)VarEnum.VT_STREAM:
					retVal = "Stream";
					break;
				case (uint)VarEnum.VT_CF:
					retVal = "Clipboard";
					break;
				default:
					retVal = "(unknown)";
					break;
			}
			if ((TypeFlag & (uint)VarEnum.VT_VECTOR) == (uint)VarEnum.VT_VECTOR)
			{
				retVal += "[]";
			}
			if (TypeFlag == ((uint)VarEnum.VT_UI1 | (uint)VarEnum.VT_VECTOR))
			{
				retVal = "Buffer";
			}

			return (retVal);
		}
	}
}
