//------------------------------------------------------------------------------

// <copyright file="CaseSensitiveStringDictionary.cs" company="Microsoft">

//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     

// </copyright>                                                                

//------------------------------------------------------------------------------

/*

On UNIX systems the environment variable names are case sensitive (as the opposite from Windows). 

Thus using StringDictionary to store the environment settings is wrong 

(StringDictionary converts key values to lower case).

CaseSensitiveStringDictionary is derived from the StringDictionary and it does the same thing, 

except the conversion of the key to lower case. So its fully usable for UNIX systems.

This class is used to create the StringDictionary object everywhere 

its used for environment settings storage (only ProcessStartInfo.cs and Executor.cs). 

This change enables the correct UNIX behavior along with not changing public API.

Author: vitkaras

*/



#if PLATFORM_UNIX



namespace System.Collections.Specialized

{

    using System.Runtime.InteropServices;

    using System.Diagnostics;

    using System;

    using System.Collections;

    using System.ComponentModel.Design.Serialization;

    using System.Globalization;

    using System.Security.Permissions;



    // This is copied from the StringDictionary

    // All of it except that the ToLower is removed for the keys -> this class maintains case

    internal class CaseSensitiveStringDictionary : StringDictionary

    {

        public CaseSensitiveStringDictionary () {

        }



        public override string this[ string key ]

        {

            get

            {

                if ( key == null )

                {

                    throw new ArgumentNullException ( "key" );

                }



                return (string) contents[ key ];

            }

            set

            {

                if ( key == null )

                {

                    throw new ArgumentNullException ( "key" );

                }



                contents[ key ] = value;

            }

        }



        public override void Add ( string key, string value )

        {

            if ( key == null )

            {

                throw new ArgumentNullException ( "key" );

            }



            contents.Add ( key , value );

        }



        public override bool ContainsKey ( string key )

        {

            if ( key == null )

            {

                throw new ArgumentNullException ( "key" );

            }



            return contents.ContainsKey ( key );

        }



        public override void Remove ( string key )

        {

            if ( key == null )

            {

                throw new ArgumentNullException ( "key" );

            }



            contents.Remove ( key );

        }

    }

}



#endif // PLATFORM_UNIX

