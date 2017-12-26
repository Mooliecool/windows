using System;
using myMath;
class MathClient 
{
   // Static method Main is the entry point method.
   public static void Main() 
   { 
      MathStuff myMathStuff = new MathStuff();
      Console.WriteLine("Sqaring 5");
      myMathStuff.SquareIt(5);
   }
}