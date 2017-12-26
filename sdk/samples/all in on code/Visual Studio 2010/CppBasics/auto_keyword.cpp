/****************************** Module Header ******************************\
Module Name:  auto.cpp
Project:      CppBasics
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <vector>
#include <iostream>
using namespace std;


template<class T> void printtall(const vector<T>& v)
{
    // The use of auto to deduce the type of a variable from its initializer 
    // is obviously most useful when that type is either hard to know exactly 
    // or hard to write. For example,
    for (auto p = v.begin(); p != v.end(); p++)
    {
        cout << *p << endl;
    }

    // In C++98, we would have to write:
    /*for (typename vector<T>::const_iterator p = v.begin(); p != v.end(); p++)
    {
        cout << *p << endl;
    }*/
}


void auto_keyword()
{
    // Here x will have the type int because it's the type of its intializer.
    auto x = 7;

    vector<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    printtall(numbers);
}