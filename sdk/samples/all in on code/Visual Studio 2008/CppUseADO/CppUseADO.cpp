/****************************** Module Header ******************************\
* Module Name:  CppUseADO.cpp
* Project:      CppUseADO
* Copyright (c) Microsoft Corporation.
* 
* The CppUseADO sample demonstrates the Microsoft ActiveX Data Objects (ADO) 
* technology to access databases using #import and Visual C++. It shows the 
* basic structure of connecting to a data source, issuing SQL commands,  
* using the Recordset object and performing the cleanup.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/11/2009 10:23 PM Jialiang Ge Created
\***************************************************************************/

#pragma region Includes and Imports
#include "stdafx.h"

#include <atlstr.h>

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
	rename("EOF", "EndOfFile")
#pragma endregion


DWORD ReadImage(PCTSTR pszImage, SAFEARRAY FAR **psaChunk);

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr;
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	ADODB::_ConnectionPtr spConn = NULL;
	ADODB::_RecordsetPtr spRst = NULL;

	try
	{
		/////////////////////////////////////////////////////////////////////
		// Connect to the data source.
		// 

		_tprintf(_T("Connecting to the database ...\n"));

		// Define the connection string. (The data source is created in the 
		// sample SQLServer2005DB)
		_bstr_t bstrUserID("HelloWorld");
		_bstr_t bstrPassword("111111");
		_bstr_t bstrConn("Provider=SQLOLEDB.1;" \
			"Data Source=(local);Initial Catalog=SQLServer2005DB;");

		// Open the connection
		hr = spConn.CreateInstance(__uuidof(ADODB::Connection));
		hr = spConn->Open(bstrConn, bstrUserID, bstrPassword, NULL);
		

		/////////////////////////////////////////////////////////////////////
		// Build and Execute an ADO Command.
		// It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a  
		// stored procedure call. Here is the sample of an INSERT command.
		// 

		_tprintf(_T("Inserting a record to the Person table\n"));

		// 1. Create a command object
		ADODB::_CommandPtr spInsertCmd;
		hr = spInsertCmd.CreateInstance(__uuidof(ADODB::Command));
		
		// 2. Assign the connection to the command
		spInsertCmd->ActiveConnection = spConn;	

		// 3. Set the command text
		// SQL statement or the name of the stored procedure 
		_bstr_t bstrInsertCmd(
			"INSERT INTO Person(LastName, FirstName, EnrollmentDate, Picture)" \
			" VALUES (?, ?, ?, ?)");
		spInsertCmd->CommandText = bstrInsertCmd;

		// 4. Set the command type
		// ADODB::adCmdText for oridinary SQL statements; 
		// ADODB::adCmdStoredProc for stored procedures.
		spInsertCmd->CommandType = ADODB::adCmdText;

		// 5. Append the parameters

		// Append the parameter for LastName (nvarchar(50))
		variant_t vtLastName("Ge");
		ADODB::_ParameterPtr spLastNameParam;
		spLastNameParam = spInsertCmd->CreateParameter(
			_bstr_t("LastName"),		// Parameter name
			ADODB::adVarWChar,			// Parameter type (NVarChar)
			ADODB::adParamInput,		// Parameter direction
			50,							// Max size in bytes (NVarChar(50)
			vtLastName);				// Parameter value
		hr = spInsertCmd->Parameters->Append(spLastNameParam);

		// Append the parameter for FirstName (nvarchar(50))
		variant_t vtFirstName("Jialiang");
		ADODB::_ParameterPtr spFirstNameParam;
		spFirstNameParam = spInsertCmd->CreateParameter(
			_bstr_t("FirstName"),		// Parameter name
			ADODB::adVarWChar,			// Parameter type (NVarChar)
			ADODB::adParamInput,		// Parameter direction
			50,							// Max size in bytes (NVarChar(50)
			vtFirstName);				// Parameter value
		hr = spInsertCmd->Parameters->Append(spFirstNameParam);

		// Append the parameter for EnrollmentDate (datetime)
		SYSTEMTIME sysNow;
		GetSystemTime(&sysNow);
		double dNow;
		SystemTimeToVariantTime(&sysNow, &dNow);
		variant_t vtEnrollmentDate(dNow, VT_DATE);
		ADODB::_ParameterPtr spEnrollmentParam;
		spEnrollmentParam = spInsertCmd->CreateParameter(
			_bstr_t("EnrollmentDate"),	// Parameter name
			ADODB::adDate,				// Parameter type (DateTime)
            ADODB::adParamInput,		// Parameter direction
            -1,							// Max size (ignored for datetime)
            vtEnrollmentDate);			// Parameter value
		hr = spInsertCmd->Parameters->Append(spEnrollmentParam);

		// Append the parameter for Image (image)

		// Read the image file into a safe array of bytes
		SAFEARRAY FAR *psaChunk = NULL;
		int cbChunkBytes = ReadImage(_T("MSDN.jpg"), &psaChunk);
		variant_t vtChunk;
		if (cbChunkBytes > 0)	// If the image is read successfully
		{
			// Assign the Safe array to a variant
			vtChunk.vt = VT_ARRAY | VT_UI1;
			// The Safe array is freed when vtChunk is released, so it is 
			// unnecessary to call SafeArrayDestroy to destroy the array.
			vtChunk.parray = psaChunk;
		}
		else 
		{
			// Set the Max size of the parameter to be a valid value
			cbChunkBytes = 1;
			// Set the value of the parameter to be DBNull
			vtChunk.ChangeType(VT_NULL);
		}
		ADODB::_ParameterPtr spImageParam;
		spImageParam = spInsertCmd->CreateParameter(
			_bstr_t("Image"),			// Parameter name
			ADODB::adLongVarBinary,		// Parameter type (Image)
			ADODB::adParamInput,		// Parameter direction
			cbChunkBytes,				// Max size in bytes
			vtChunk);					// Parameter value
		hr = spInsertCmd->Parameters->Append(spImageParam);

		// 6. Execute the command
		spInsertCmd->Execute(NULL, NULL, ADODB::adExecuteNoRecords);


		/////////////////////////////////////////////////////////////////////
		// Use the Recordset Object.
		// http://msdn.microsoft.com/en-us/library/ms681510.aspx
		// Recordset represents the entire set of records from a base table 
		// or the results of an executed command. At any time, the Recordset 
		// object refers to only a single record within the set as the 
		// current record.
		// 

		_tprintf(_T("Enumerating the records in the Person table\n"));

		// 1. Create a Recordset object
		hr = spRst.CreateInstance(__uuidof(ADODB::Recordset));
		
		// 2. Open the Recordset object
		_bstr_t bstrSelectCmd("SELECT * FROM Person"); // WHERE ...
		hr = spRst->Open(bstrSelectCmd,	// SQL statement / table,view name / 
										// stored procedure call / file name
			spConn.GetInterfacePtr(),	// Connection / connection string
			ADODB::adOpenForwardOnly,	// Cursor type. (forward-only cursor)
			ADODB::adLockOptimistic,	// Lock type. (locking records only 
										// when you call the Update method.
			ADODB::adCmdText);			// Evaluate the first parameter as
										// a SQL command or stored procedure.

		// 3. Enumerate the records by moving the cursor forward
		spRst->MoveFirst();	// Move to the first record in the Recordset
		while (!spRst->EndOfFile)
		{
			int nPersonId = spRst->Fields->Item["PersonID"]->Value.intVal;
			variant_t vtLastName(spRst->Fields->Item["LastName"]->Value);
			variant_t vtFirstName(spRst->Fields->Item["FirstName"]->Value);
			
			// When dumping a SQL-Nullable field in the table, you need to 
			// test it for VT_NULL.
			_tprintf(_T("%d\t%s %s\n"), nPersonId,
				vtFirstName.vt == VT_NULL ? _T("(DBNull)") : (PCTSTR)vtFirstName.bstrVal, 
				vtLastName.vt == VT_NULL ? _T("(DBNull)") : (PCTSTR)vtLastName.bstrVal
				);

			// Update the current record while enumerating the Recordset.
			//spRst->Fields->Item["XXXX"]->Value = XXXX
			//spRst->Update(); Or spRst->UpdateBatch(); outside the loop.

			spRst->MoveNext();			// Move to the next record
		}

	}
	catch (_com_error &err)
	{
		_tprintf(_T("The application throws the error: %s\n"), 
			err.ErrorMessage());
		_tprintf(_T("Description = %s\n"), (LPCTSTR) err.Description());
	}


	/////////////////////////////////////////////////////////////////////////
	// Clean up objects before exit.
	// 

	_tprintf(_T("Closing the connections ...\n"));

	// Close the record set if it is open
	if (spRst && spRst->State == ADODB::adStateOpen)
		spRst->Close();

	// Close the connection to the database if it is open
	if (spConn && spConn->State == ADODB::adStateOpen)
		spConn->Close();

	// Uninitialize COM for this thread
	::CoUninitialize();

	return 0;
}


/*!
 * \brief
 * Read an image file to a safe array of bytes.
 * 
 * \param pszImage
 * The path of the image file.
 * 
 * \param ppsaChunk
 * The output of the safe array.
 * 
 * \returns
 * The number of bytes read from the image file. 0 means failure.
 */
DWORD ReadImage(PCTSTR pszImage, SAFEARRAY FAR **ppsaChunk)
{
	// Open the image file
	HANDLE hImage = CreateFile(pszImage, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hImage == INVALID_HANDLE_VALUE) 
	{
		_tprintf(_T("Could not open the image %s\n"), pszImage);
		return 0;
	}

	// Get the size of the file in bytes
	LARGE_INTEGER liSize;
	if (!GetFileSizeEx(hImage, &liSize))
	{
		_tprintf(_T("Could not get the image size w/err 0x%08lx\n"), 
			GetLastError());
		CloseHandle(hImage);
		return 0;
	}
	if (liSize.HighPart != 0)
	{
		_tprintf(_T("The image file is too big\n"));
		CloseHandle(hImage);
		return 0;
	}
	DWORD dwSize = liSize.LowPart, dwBytesRead;

	// Create a safe array with cbChunkBytes elements
	*ppsaChunk = SafeArrayCreateVector(VT_UI1, 1, dwSize);

	// Initialize the content of the safe array
	BYTE *pbChunk;
	SafeArrayAccessData(*ppsaChunk, (void **)&pbChunk);

	// Read the image file
	if (!ReadFile(hImage, pbChunk, dwSize, &dwBytesRead, NULL) 
		|| dwBytesRead == 0 || dwBytesRead != dwSize)
	{
		_tprintf(_T("Could not read from file w/err 0x%08lx\n"),
			GetLastError());
		CloseHandle(hImage);
		// Destroy the safe array
		SafeArrayUnaccessData(*ppsaChunk);
		SafeArrayDestroy(*ppsaChunk);
        return 0;
	}

	SafeArrayUnaccessData(*ppsaChunk);

	CloseHandle(hImage);

	return dwSize;
}