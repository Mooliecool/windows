// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==

#ifndef __GENERIC_LIST_H__
#define __GENERIC_LIST_H__

// Simple parameterized linked list
// with some good ctors
template<typename _T> struct list {
    _T arg;
    list<_T> *next;

    list(_T t, list<_T> *n) { arg = t, next = n; }
    list() : arg(), next(NULL) {}
};

#endif // __GENERIC_LIST_H__
