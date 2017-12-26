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
using System.Reflection;
using System.Runtime.InteropServices;

[Serializable()]
public class MyValue
{
    int _Value;

    public MyValue(int value) {
        _Value = value;
    }

    public int Value {
        get { return _Value; }
    }

    public void BadError() {
        throw new Exception("Bad Error");
    }
};

[StructLayout(LayoutKind.Sequential)]
struct MyStruct {
    [MarshalAs(UnmanagedType.IUnknown)] public Object value;
};

class MainApp : MarshalByRefObject {

    public delegate int MySimpleDelegate(int x); 
    public delegate double MyDelegate([MarshalAs(UnmanagedType.IUnknown)] Object obj); 

    MySimpleDelegate _simpleDelegate;
    MyDelegate _delegate;

#if !PLATFORM_UNIX
       internal const String DLLPREFIX = "";
       internal const String DLLSUFFIX = ".dll";
#else // !PLATFORM_UNIX
 #if __APPLE__
       internal const String DLLPREFIX = "lib";
       internal const String DLLSUFFIX = ".dylib";
 #else
       internal const String DLLPREFIX = "lib";
       internal const String DLLSUFFIX = ".so";
 #endif
#endif // !PLATFORM_UNIX

    const String NATIVEDLL = DLLPREFIX + "nativedll" + DLLSUFFIX;

    [DllImport(NATIVEDLL)]
    static extern int CacheSimpleDelegate(MySimpleDelegate pfn);

    [DllImport(NATIVEDLL)]
    static extern int CallSimpleDelegate(MySimpleDelegate pfn, int x);

    [DllImport(NATIVEDLL)]
    static extern int CacheDelegate(MyDelegate pfn);

    [DllImport(NATIVEDLL)]
    static extern double CallDelegate(MyDelegate pfn, [MarshalAs(UnmanagedType.IUnknown)] Object obj);
    
    [DllImport(NATIVEDLL)]
    static extern double CallFFI([MarshalAs(UnmanagedType.IUnknown)] Object callback, [MarshalAs(UnmanagedType.IUnknown)] Object obj);

    [DllImport(NATIVEDLL)]
    static extern int Sum(int count, int[] a);

    [DllImport(NATIVEDLL)]
    static extern int ObjectSum(int count, MyStruct[] a);

    [DllImport(NATIVEDLL, PreserveSig = false)]
    static extern void ReallyBadError([MarshalAs(UnmanagedType.IUnknown)] Object callback);

    [DllImport(NATIVEDLL)]
    static extern void Exception12345678(MyDelegate pfn, [MarshalAs(UnmanagedType.IUnknown)] Object e);
    
    [DllImport(NATIVEDLL)]
    static extern MySimpleDelegate DelegateMarshal(MySimpleDelegate dlg);
    
    static public double JustThrow(Object o) {
        throw (Exception)o;
    }

    public int MySimpleCallback(int x) {
        Console.WriteLine("AppDomain in MySimpleCallback: " + AppDomain.CurrentDomain.FriendlyName);
        return x;
    }

    public MySimpleDelegate CreateSimpleCallback(bool cache)
    {
      MySimpleDelegate callback = new MySimpleDelegate(this.MySimpleCallback);
      if (cache) {
          CacheSimpleDelegate(callback);
          _simpleDelegate = callback;
          callback = null;
      }
      return callback;
    }
    
    public double MyCallback(Object obj) {
        Console.WriteLine("AppDomain in MyCallback: " + AppDomain.CurrentDomain.FriendlyName);

        Object result = obj.GetType().InvokeMember("Value", 
            BindingFlags.Instance | BindingFlags.Public | BindingFlags.GetProperty, null,
            obj, null, null, null, null);

        return (double)(int)result;
    }

    public MyDelegate CreateCallback(bool cache)
    {
      MyDelegate callback = new MyDelegate(this.MyCallback);
      if (cache) {
          CacheDelegate(callback);
          _delegate = callback;
          callback = null;
      }
      return callback;
    }

    public static int Main() {
        MainApp a = new MainApp();

        // simple delegate roundtrip
        {
            int x = CallSimpleDelegate(a.CreateSimpleCallback(false), 1);
            if (x != 2) {
                Console.WriteLine("Unexpected value: " + x.ToString());
                return 1;
            }
        }
        Console.WriteLine("Simple delegate roundtrip passed");

        // delegate roundtrip
        {
            double x = CallDelegate(a.CreateCallback(false), new MyValue(1));
            if (x != 2) {
                Console.WriteLine("Unexpected value: " + x.ToString());
                return 1;
            }
        }
        Console.WriteLine("Delegate roundtrip passed");

        // simple delegate roundtrip - crossdomain
        {
            AppDomain app = AppDomain.CreateDomain("MyDomain");
            MainApp remoteapp = (MainApp)app.CreateInstanceAndUnwrap ("interoptest1", "MainApp");
            MySimpleDelegate callback = remoteapp.CreateSimpleCallback(true);

            int x = CallSimpleDelegate(callback, 1);
            if (x != 2) {
                Console.WriteLine("Unexpected value: " + x.ToString());
                return 1;
            }
        }
        Console.WriteLine("Simple delegate roundtrip crossdomain passed");

        // delegate roundtrip - crossdomain
        {
            AppDomain app = AppDomain.CreateDomain("MyDomain");
            MainApp remoteapp = (MainApp)app.CreateInstanceAndUnwrap ("interoptest1", "MainApp");
            MyDelegate callback = remoteapp.CreateCallback(true);

            double x = CallDelegate(callback, new MyValue(1));
            if (x != 2) {
                Console.WriteLine("Unexpected value: " + x.ToString());
                return 1;
            }
        }
        Console.WriteLine("Delegate roundtrip crossdomain passed");

        // ffi roundtrip
        {
            double d = CallFFI(a, new MyValue(1));
            if (d != 2) {
                Console.WriteLine("Unexpected value: " + d.ToString());
                return 2;
            }
        }
        Console.WriteLine("FFI roundtrip passed");

        // array marshaling
        {
            int[] rg = new int[4];
            rg[0] = 12;
            rg[1] = 33;
            rg[2] = -21;
            rg[3] = 18;
            int sum = Sum(rg.Length, rg);
            if (sum != 42) {
                Console.WriteLine("Unexpected value: " + sum.ToString());
                return 3;
            }                           
        }
        Console.WriteLine("Array marshalling passed");

        // array marshaling ex
        {
            MyStruct[] rg = new MyStruct[7];
            rg[0].value = new MyValue(2314);
            rg[1].value = new MyValue(3452);
            rg[2].value = new MyValue(3235);
            rg[3].value = new MyValue(3452);
            rg[4].value = new MyValue(6980);
            rg[5].value = new MyValue(3133);
            rg[6].value = new MyValue(3426);
            int sum = ObjectSum(rg.Length, rg);
            if (sum != 25992) {
                Console.WriteLine("Unexpected value: " + sum.ToString());
                return 4;
            }                           
        }
        Console.WriteLine("Array marshalling ex passed");

        // errorinfo roundtrip
        {
            bool thrown = false;
          
            try {
                ReallyBadError(new MyValue(0));
            }
            catch (Exception e)
            {
                string s = e.ToString();
                if (s.IndexOf("qwerty") == -1) {
                    Console.WriteLine("Unexpected value: " + s);
                    return 5;
                }                           
                thrown = true;
            }

            if (!thrown) {
                Console.WriteLine("Exception wasn't thrown");
                return 5;
            }                            
        }
        Console.WriteLine("IErrorInfo roundtrip passed");

        // delegate roundtrip
        {
            MySimpleDelegate d1 = a.CreateSimpleCallback(false);            
            MySimpleDelegate d2 = DelegateMarshal(d1);
            if (d1 != d2) {
                Console.WriteLine("Delegate marshal failed");
                return 8;
            }
        }
        Console.WriteLine("Delegate marshal passed");

        Console.WriteLine("All test passed");
        return 0;
    }
}
