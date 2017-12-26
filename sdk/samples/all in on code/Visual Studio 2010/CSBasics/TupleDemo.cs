using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CSBasics
{
    class TupleDemo
    {
        public static void Run()
        {
            Console.WriteLine("Tuple Demo ...");

            var tuple = new Tuple<string, int>("Bart", 27);
            Console.WriteLine(tuple.Item1 + " is " + tuple.Item2);
        }
    }
}