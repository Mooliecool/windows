/****************************** Module Header ******************************\
Module Name:    TypeCasting.cpp
Project:        CppBasics
Copyright (c) Microsoft Corporation.

1. Classic C casts
Classic C casts make no distinction between what we call static_cast<>(),  
reinterpret_cast<>() and const_cast<>(). Classic C casting is powerful, 
but it can cause headaches because in some situations it forces the 
compiler to treat data as if it were (for instance) larger than it really 
is, so it will occupy more space in memory; this can trample over other 
data. 

2. static_cast<>() 
A static_cast<>() is usually safe. There is a valid conversion in the 
language, or an appropriate constructor that makes it possible. The only 
time is a bit risky is to cast down to an inherited class.

3. reinterpret_cast<>()
A reinterpret_cast<>() is always dangerous. It can cast a type to a 
completely different meaning. (Take the Animal* -> Computer* casting as 
an example) The key is that you will need to cast back to the original 
type to use it safely. The type you cast to is typically used only for 
bit twiddling or some other mysterious purpose. 

4. const_cast<>()
If you want to convert from a const to a nonconst or from a volatile to 
a nonvolatile, you use const_cast. 

More readings:

MSDN: Casting Operators
http://msdn.microsoft.com/en-us/library/5f6c9f8h.aspx

What static_cast<> is actually doing
http://www.codeproject.com/KB/cpp/static_cast.aspx

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>


class Animal
{
};

class Cat : 
    public Animal 
{
};

class Computer
{
};


void TypeCasting(void)
{
    Animal *pAnimal = NULL;
    Cat *pCat;
    Computer *pComputer;


    // 
    // Cast Animal* to Cat* (Related type casting).
    // 

    // The C++ language explicitly allows you to cast a pointer from base 
    // class to a pointer to derived class.  If the pointer being cast doesn 
    // not actually point to the derived class, the result is undefined.

    // static_cast<>() (Safe)
    pCat = static_cast<Cat*>(pAnimal);

    // Classic C cast (Safe)
    pCat = (Cat*)pAnimal;


    // 
    // Cast Animal* to Computer* (Unrelated type casting).
    // 

    // static_cast<>() (Safe)
    // The line generates the compile-time error C2440: 'static_cast' : 
    // cannot convert from 'Animal *' to 'Computer *'.
    //pComputer = static_cast<Computer*>(pAnimal);

    // reiterpret_cast<>() (Dangerous)
    // No compiler error.
    pComputer = reinterpret_cast<Computer*>(pAnimal);

    // Classic C cast (Dangerous)
    // No compiler error. Same as reiterpret_cast<>().
    pComputer = (Computer*)pAnimal;


    // 
    // Cast const type to non-const type.
    // 

    const int i = 0;

    // Classic C cast (Deprecated)
    int* pi1 = (int*)&i;
    *pi1 = 1;
    wprintf(L"i = %d, *pi1 = %d\n", i, *pi1);

    // const_cast<>() (Preferred)
    int* pi2  = const_cast<int*>(&i); 
    *pi2 = 2;
    wprintf(L"i = %d, *pi2 = %d\n", i, *pi2);
}