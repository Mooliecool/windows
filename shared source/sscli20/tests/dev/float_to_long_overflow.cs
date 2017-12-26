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
using System;
class Test{     

    static public void Main ()
    {
        int retCode = 0;

        double dlong_max2    = 9223372036854775807.0;
        double dulong_max2   = 18446744073709551615.0;
        double dlong_max2_2  = dlong_max2 / 2.0;
        double dulong_max2_2 = dulong_max2 / 2.0;

        float flong_max2    = (float)dlong_max2;
        float fulong_max2   = (float)dulong_max2;
        float flong_max2_2  = (float)dlong_max2_2;
        float fulong_max2_2 = (float)dulong_max2_2;

        // These conversions are OK
        
        long long_safe1 = checked((long)flong_max2_2);
        Console.WriteLine (long_safe1);
        ulong long_safe2 = checked((ulong)(fulong_max2_2));
        Console.WriteLine (long_safe2);

        long long_safe3 = checked((long)dlong_max2_2);
        Console.WriteLine (long_safe1);
        ulong long_safe4 = checked((ulong)(dulong_max2_2));
        Console.WriteLine (long_safe2);

        // These conversions overflow
        try {
            long long_max2 = checked((long)flong_max2);
            Console.WriteLine (long_max2);
            retCode = 1;
        } catch (Exception e) { Console.WriteLine(e); }

        try {
            ulong ulong_max2 = checked((ulong)fulong_max2);
            Console.WriteLine (ulong_max2);
            retCode = 2;
        } catch (Exception e) { Console.WriteLine(e); }        

        try {
            long long_max2 = checked((long)dlong_max2);
            Console.WriteLine (long_max2);
            retCode = 3;
        } catch (Exception e) { Console.WriteLine(e); }

        try {
            ulong ulong_max2 = checked((ulong)dulong_max2);
            Console.WriteLine (ulong_max2);
            retCode = 4;
        } catch (Exception e) { Console.WriteLine(e); }        
        
        Environment.ExitCode = retCode;
        Console.WriteLine("retCode=={0}",retCode);
    }
}
