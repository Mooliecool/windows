using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Numerics;

namespace CSBasics
{
    class BigIntegerDemo
    {
        public static void Run()
        {
            Console.WriteLine("BigInteger Demo ...");

            Console.WriteLine("Fac(10000) = {0}", Fac(10000));
        }

        static BigInteger Fac(int n)
        {
            return (n == 0) ? new BigInteger(1) : n * Fac(n - 1);
        }
    }
}
