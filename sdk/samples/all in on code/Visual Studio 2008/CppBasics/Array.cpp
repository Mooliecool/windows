/****************************** Module Header ******************************\
* Module Name:	Array.cpp
* Project:		CppBasics
* Copyright (c) Microsoft Corporation.
* 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/28/2009 9:10 PM Jialiang Ge Created
\***************************************************************************/

#include "stdafx.h"


void Array(void)
{
	/////////////////////////////////////////////////////////////////////////
	// One-dimensional array.
	// 

	// Define a one-dimensional array of 3 elements of type char
	char arr1[3] = { 'a', 'b', 'c' };

	// Access the first element
	_tprintf(_T("%c\n"), arr1[0]);


	/////////////////////////////////////////////////////////////////////////
	// Multidimensional array.
	// 

	// Define a bidimensional array of 2 per 3 elements of type int
	int arr2[2][3] = { { 1, 2, 3 }, {4, 5, 6} };

	// Access the second element vertically and third horizontally
	int x = 1, y = 2;
	_tprintf(_T("%d\n"), arr2[x][y]);
}