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

public struct _1 {
    public Object o;
    public int x;
};

public struct _2 {
     public _1 a;
     public _1 b;
     public _1 c;
     public _1 d;
};

public struct _3 {
     public _2 a;
     public _2 b;
     public _2 c;
     public _2 d;
};

public struct _4 {
     public _3 a;
     public _3 b;
     public _3 c;
     public _3 d;
};

public struct _5 {
     public _4 a;
     public _4 b;
     public _4 c;
     public _4 d;
};

public struct My {
     public _5 a;
     public _5 b;
};

delegate My MyDelegate(My p4, int x, My p5);

class MainApp {

   My _m;
   My _n;
   
   MyDelegate _callback;
   
   My Callback(My p4, int x, My p5) {
        if (x == 0) {
            Console.WriteLine("Here!");
            GC.Collect();
            return p4;
        }
        p5.b.d.a.b.c.o = new Object();
        return _callback(p4, x-1, p5);
   }

   public void RunMe() {
      My x;
      My y;
      int j = 123456789;
Backward:
      if (j == 0)
         goto Forward;
      for (int i = 0; i < 5; i++) {
          x.b.d.a.b.c.o = new Object();
          if ((j & 1) != 0) {
              x = _m;
              if (j < 0) break;
              y = _n;
              if (j < -1) continue;
              _m = y;
              if (j < -2) break;
              _n = x;
          } else {
              x = _m;
              y = _n;
              if ((j & 2) != 0) goto Q;
              if ((j & 4) != 0)                  
                  _m = x;
              else
                  _n = y;
          }
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
Q:
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          x = _m; _m = _n; _n = x; y = _m; _m = _n; _n = y;
          j = j >> 1;
          GC.Collect();
      }
      j = 0;
      if (j == 0) goto Backward;
      
Forward:      
      _callback = new MyDelegate(Callback);
      _callback += new MyDelegate(Callback);
      _callback(_n, 5, _m);
    
      Console.WriteLine("Passed");
   }

   public static void Main() {       
       new MainApp().RunMe();
   }
}
