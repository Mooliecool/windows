//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// This is header file of AutoFastBstr Class. AutoFastBstr is derived from FastStr. 
//
//-------------------------------------------------------------------------------------------------

#pragma once 

//-------------------------------------------------------------------------------------------------
//
// AutoFastBstr stores strings as BSTRs.
// AutoFastBstr uses all string comparison capabilities provided by FastStr. 
//
// Assumptions:
//      User should not provide a BSTR with NULL character (inside string)
//      other than terminating NULL character. In case of more than one NULLs in BSTR, 
//      AutoFastBstr stores string upto first NULL character.
//
//-------------------------------------------------------------------------------------------------
class AutoFastBstr : public FastStr
{
public:

    //Constructor: Object declaration 
    AutoFastBstr();

    // Constructor: Object declaration with wchar_t * 
    AutoFastBstr(_In_opt_z_ const wchar_t * wczStr);

    // Constructor: Object declaration with BSTR and attach option (false/true)
    AutoFastBstr(
        _In_opt_z_ const BSTR wczStr, 
        bool shouldAttach = false);
    
    // Copy Constructor
    AutoFastBstr(_In_ const AutoFastBstr &obj);

    // Destructor: Calls Release function
    ~AutoFastBstr();

    // Assignment Operator: Initializes the object with received string. 
    AutoFastBstr& operator=(_In_opt_z_ const wchar_t * wczStr);

    // Assignment Operator: Initializes the object with right-hand side object. 
    AutoFastBstr& operator=(_In_ const AutoFastBstr &rhsAutoFastBstr);

    // Free the string unless being used by some other object
    virtual __override void Release();

protected:

    // String's Reference count 
    unsigned * m_referenceCount;

    // Returns a BSTR 
    static BSTR CopyString(_In_opt_z_ const wchar_t * wczStr);

    //Initializes the class members
    virtual __override void Init(_In_opt_z_ const wchar_t * wczStr);
};




