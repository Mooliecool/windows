/******************************** Module Header ********************************\
Module Name:  MemoryLeaks.cs
Project:      CSResourceLeaks
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#region Using directives
using System;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections.Specialized;
using System.Threading;
using System.Xml.Serialization;
#endregion


namespace CSResourceLeaks
{
    public class MemoryLeaks
    {
        #region Native Memory Leak (Demo 1)

        /// <summary>
        /// 
        /// </summary>
        public static void LeakNativeMemory1()
        {
            Console.Write("Press ENTER to leak native memory ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");
            while (true)
            {
                StringBuilder time = new StringBuilder(512);
                GetDateTimeString(time, time.Capacity);
                Console.Write("Current time: {0}\r", time);

                Thread.Sleep(1); // Simulate a busy task.
            }
        }

        [DllImport("CSResourceLeaks.NativeDll.dll", CharSet = CharSet.Unicode,
            CallingConvention = CallingConvention.Cdecl)]
        static extern void GetDateTimeString(StringBuilder date, int length);

        #endregion


        #region Native Memory Leak (Demo 2)

        /// <summary>
        /// 
        /// </summary>
        public static void LeakNativeMemory2()
        {
            Console.Write("Press ENTER to leak native memory ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");
            while (true)
            {
                // Allocate 1KB process heap memory.
                const int cb = 1024;
                IntPtr pMem = Marshal.AllocHGlobal(cb);

                // Use the memory.
                // For example, pass the memory pointer to a native function.
                ZeroMemory(pMem, new IntPtr(cb));

                Thread.Sleep(1); // Simulate a busy task.

                // Forget to free the memory.
                //Marshal.FreeHGlobal(pMem);
            }
        }

        [DllImport("kernel32.dll", EntryPoint = "RtlZeroMemory")]
        static extern void ZeroMemory(IntPtr dest, IntPtr size);

        #endregion


        #region Managed GC Heap Memory Leak (Demo 1)

        /// <summary>
        /// 
        /// </summary>
        /// <example>
        /// ASP.NET Memory Issues - High Memory Usage with AjaxPro (fixed in 
        /// current version) by Tess Ferrandez http://tinyurl.com/highgcmem1
        /// </example>
        public static void LeakManagedGCHeapMemory1()
        {
            Console.Write("Press ENTER to leak managed memory ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");
            while (true)
            {
                // Allocate an array object of about 1KB GC heap memory.
                byte[] arr = new byte[1024];

                // Generate a random name for the demo purpose.
                string name = Path.GetRandomFileName().Replace(".", "");

                // Add the name-array pair into the cache.
                cache.Add(name, arr);

                Thread.Sleep(1); // Simulate a busy task.
            }
        }

        static HybridDictionary cache = new HybridDictionary();

        #endregion


        #region Managed GC Heap Memory Leak (Demo 2)

        /// <summary>
        /// 
        /// </summary>
        /// <example>
        /// .NET Memory Leak Case Study: The Event Handlers That Made The Memory 
        /// Baloon by Tess Ferrandez http://tinyurl.com/highgcmem2
        /// </example>
        public static void LeakManagedGCHeapMemory2()
        {
            Console.Write("Press ENTER to leak managed memory ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");
            while (true)
            {
                MyObject obj = new MyObject();

                Thread.Sleep(1); // Simulate a busy task.
            }
        }

        class MyObject
        {
            public MyObject()
            {
                // Hook onto the static event.
                Application.Idle += new EventHandler(Application_Idle);
            }

            void Application_Idle(object sender, EventArgs e)
            {
                Console.WriteLine("Application idles ...");
            }

            private byte[] buffer = new byte[1024];
        }

        class Application
        {
            // A static event.
            public static event EventHandler Idle;
        }

        #endregion


        #region Managed Loader Heap Memory Leak

        /// <summary>
        /// 
        /// </summary>
        public static void LeakManagedLoaderHeapMemory()
        {
            Console.Write("Press ENTER to leak managed memory ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");

            XmlRootAttribute root = new XmlRootAttribute();
            root.ElementName = "PersonRoot";
            root.Namespace = "http://www.contoso.com";
            root.IsNullable = true;

            string tempFile = Path.GetTempFileName();

            while (true)
            {
                Person person = new Person("Mike", 26, "Canada");

                using (Stream steam = new FileStream(tempFile, FileMode.Create))
                {
                    XmlSerializer ser = new XmlSerializer(typeof(Person), root);
                    ser.Serialize(steam, person);
                }
            }
        }

        public class Person
        {
            public Person(string name, int age, string country)
            {
                this.Name = name;
                this.Age = age;
                this.Country = country;
            }

            public string Name { get; set; }
            public int Age { get; set; }
            public string Country { get; set; }
        }

        #endregion
    }
}