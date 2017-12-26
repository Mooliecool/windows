using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IronPython.Hosting;

namespace CSBasics
{
    class DynamicDemo
    {
        public static void Run()
        {
            Console.WriteLine("Dynamic Demo ...");

            Static();
            Dynamic();
            IronPython();
        }

        static void Static()
        {
            var calc = new Calculator();
            int r = calc.Add(2, 3);
            Console.WriteLine(r);
        }

        static void Dynamic()
        {
            dynamic calc = Calculator.GetCalculator();
            int r = calc.Add(2, 3);
            Console.WriteLine(r);
        }

        static void IronPython()
        {
            // http://ironpython.codeplex.com
            var engine = Python.CreateEngine();
            dynamic scope = engine.ImportModule("Calculator");

            var calc = scope.GetCalculator();
            int r = calc.Add(2, 3);
            Console.WriteLine(r);
        }
    }


    class Calculator
    {
        public int Add(int a, int b)
        {
            return a + b;
        }

        public static dynamic GetCalculator()
        {
            return new Calculator();
        }
    }
}
