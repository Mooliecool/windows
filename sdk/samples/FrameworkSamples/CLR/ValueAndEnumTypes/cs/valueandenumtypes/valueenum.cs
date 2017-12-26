//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
/*=====================================================================
  File:      ValueEnum.cs

  Summary:   Demonstrates things you can do with ValueType/Enum types.

=====================================================================*/
using System;

class App{
    public static void Main () {
        DemoValueTypes();
        DemoReferenceTypes();
        DemoEnums();
        DemoFlags();
    }

    // This is a value type because of 'struct'
    struct Point {
        Int32 x, y;
        public Point(Int32 x, Int32 y) {
            this.x = x;
            this.y = y;
        }
        public override String ToString() {
            return("(" + x + "," + y + ")");
        }
    }

    private static void DemoValueTypes() {
        Console.WriteLine("Demo start: Demo of value types.");

        Point p1 = new Point(5, 10);
        Point p2 = new Point(5, 10);
        Point p3 = new Point(3, 4);
        // What type is this valuetype & what is it derived from
        Console.WriteLine("   The " + p1.GetType() + " type is derived from " + p1.GetType().BaseType);

        // Value types compare for equality by comparing the fields
        Console.WriteLine("   Does p1 equal p1: " + p1.Equals(p1));   // True
        Console.WriteLine("   Does p1 equal p2: " + p1.Equals(p2));   // True
        Console.WriteLine("   Does p1 equal p3: " + p1.Equals(p3));   // False
        Console.WriteLine("   p1={0}, p3={1}", p1.ToString(), p3.ToString());

        Console.WriteLine("Demo stop: Demo of value types.");
    }

    // This is a reference type because of 'class'
    class Rectangle {
        Int32 x, y, width, height;
        public Rectangle(Int32 x, Int32 y, Int32 width, Int32 height) {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }
        public override String ToString() {
            return("(" + x + "," + y + ")x("+width+","+height+")");
        }
        public override bool Equals(Object o) {
            // Change the symantics of this reference type so that it is
            // equal to the same type of object if the fields are equal.
            Console.WriteLine("   In Rectangle.Equals method");
            Rectangle r = (Rectangle) o;
            return(r.x == x && r.y == y && r.width == width && r.height == height);
        }
	public override int GetHashCode() {
            // Attempting a minor degree of "hash-ness" here
            return ((x^y)^width)^height;
	}
    }

    private static void DemoReferenceTypes() {
        Console.WriteLine("\n\nDemo start: Demo of reference types.");
        Rectangle r = new Rectangle(1, 2, 3, 4);
        // What type is this reference type & what is it derived from
        Console.WriteLine("   The " + r.GetType() + " type is derived from " + r.GetType().BaseType);
        Console.WriteLine("   " + r);

        // Reference types are equal if they refer to the same object
        Console.WriteLine("   Is r equivalent to (1, 2, 3, 4): " + (r == new Rectangle(1, 2, 3, 4)));       // False
        Console.WriteLine("   Is r equal to (1, 2, 3, 4): " + (r.Equals(new Rectangle(1, 2, 3, 4))));  // True
        Console.WriteLine("   Is r equivalent to (1, 1, 1, 1): " + (r == new Rectangle(1, 1, 1, 1)));       // False
        Console.WriteLine("   Is r equal to (1, 1, 1, 1): " + (r.Equals(new Rectangle(1, 1, 1, 1))));  // False

        Console.WriteLine("Demo stop: Demo of reference types.");
    }

    // This is an enumerated type because of 'enum'
    enum Color { 
        Red   = 111,
        Green = 222,
        Blue  = 333
    }
    private static void DemoEnums() {
        Console.WriteLine("\n\nDemo start: Demo of enumerated types.");
        Color c = Color.Red;

        // What type is this enum & what is it derived from
        Console.WriteLine("   The " + c.GetType() + " type is derived from " + c.GetType().BaseType);

        // What is the underlying type used for the Enum's value
        Console.WriteLine("   Underlying type: " + Enum.GetUnderlyingType(typeof(Color)));

        // Display the set of legal enum values
        Color[] o = (Color[]) Enum.GetValues(c.GetType());
        Console.WriteLine("\n   Number of valid enum values: " + o.Length);
        for (int x = 0; x < o.Length; x++) {
            Color cc = ((Color)(o[x]));
            Console.WriteLine("   {0}: Name={1,7}\t\tNumber={2}", x, 
                cc.ToString("G"), cc.ToString("D"));
        }

        // Check if a value is legal for this enum
        Console.WriteLine("\n   111 is a valid enum value: " + Enum.IsDefined(c.GetType(), 111));   // True
        Console.WriteLine("   112 is a valid enum value: " + Enum.IsDefined(c.GetType(), 112));   // False

        // Check if two enums are equal
        Console.WriteLine("\n   Is c equal to Red: " + (Color.Red == c));  // True
        Console.WriteLine("   Is c equal to Blue: "  + (Color.Blue == c)); // False

        // Display the enum's value as a string using different format specifiers
        Console.WriteLine("\n   c's value as a string: " + c.ToString("G"));   // Red
        Console.WriteLine("   c's value as a number: " + c.ToString("D"));   // 111

        // Convert a string to an enum's value
        c = (Color) (Enum.Parse(typeof(Color), "Blue"));
        try {
            c = (Color) (Enum.Parse(typeof(Color), "NotAColor")); // Not valid, raises exception
        }
        catch (ArgumentException) {
            Console.WriteLine("   'NotAColor' is not a valid value for this enum.");
        }

        // Display the enum's value as a string
        Console.WriteLine("\n   c's value as a string: " + c.ToString("G"));   // Blue
        Console.WriteLine("   c's value as a number: " + c.ToString("D"));   // 333

        Console.WriteLine("Demo stop: Demo of enumerated types.");
    }


    // This is an enumerated type of flags
    [Flags] 
    enum ActionAttributes { 
        Read   =  1,
        Write  =  2,
        Delete =  4,
        Query  =  8,
        Sync   = 16 
    }
    private static void DemoFlags() {
        Console.WriteLine("\n\nDemo start: Demo of enumerated flags types.");
        ActionAttributes aa = ActionAttributes.Read 
            | ActionAttributes.Write | ActionAttributes.Query;

        // What type is this enum & what is it derived from
        Console.WriteLine("   The " + aa.GetType() + " type is derived from " + aa.GetType().BaseType);

        // What is the underlying type used for the Enum's value
        Console.WriteLine("   Underlying type: " + Enum.GetUnderlyingType(aa.GetType()));

        // Display the set of legal enum values
        ActionAttributes[] o = (ActionAttributes[]) Enum.GetValues(aa.GetType());
        Console.WriteLine("\n   Number of valid enum values: " + o.Length);
        for (int x = 0; x < o.Length; x++) {
            ActionAttributes aax = ((ActionAttributes)(o[x]));
            Console.WriteLine("   {0}: Name={1,10}\tNumber={2}", x, 
                aax.ToString("G"), ((ActionAttributes) aax).ToString("D"));
        }

        // Check if a value is legal for this enum
        Console.WriteLine("\n   8 is a valid enum value: " + Enum.IsDefined(aa.GetType(), 8));   // True
        Console.WriteLine("   6 is a valid enum value: "   + Enum.IsDefined(aa.GetType(), 6));   // False

        // Display the enum's value as a string
        Console.WriteLine("\n   aa's value as a string: " + aa.ToString("G"));   // Read|Write|Query
        Console.WriteLine("   aa's value as a number: "   + aa.ToString("D"));         // 11

        // Convert a string to an enum's value
        aa  = (ActionAttributes) (Enum.Parse(typeof(ActionAttributes), "Write"));
        aa |= (ActionAttributes) (Enum.Parse(typeof(ActionAttributes), "Sync"));
        Console.WriteLine("\n   aa's value as a string: " + aa.ToString("G"));   // Write|Sync
        Console.WriteLine("   aa's value as a number: "   + aa.ToString("D"));         // 18

        Console.WriteLine("Demo stop: Demo of enumerated flags types.");
    }
}



///////////////////////////////// End of File /////////////////////////////////
