/****************************** Module Header ******************************\
* Module Name:  CppSimpleClass.h
* Project:      CppStaticLibrary
* Copyright (c) Microsoft Corporation.
* 
* The header of the class CppSimpleClass in the static library.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once


class CppSimpleClass
{
private:
	float m_fField;

public:
	// Constructor
	CppSimpleClass();

	// Property
	float get_FloatProperty();
	void set_FloatProperty(float newVal);

	// Method
	
};
