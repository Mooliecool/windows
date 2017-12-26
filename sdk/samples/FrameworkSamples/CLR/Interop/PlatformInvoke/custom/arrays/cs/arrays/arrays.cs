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
//-----------------------------------------------------------------------

// Arrays.cs

using System;
using System.Runtime.InteropServices;

namespace Microsoft.Samples
{
	[StructLayout(LayoutKind.Sequential)]
	public struct MyPoint : IComparable
	{
		int _x;

		int _y;

		public MyPoint(int x, int y)
		{
			this._x = x;
			this._y = y;
		}

		public int X { get { return _x; } set { _x = value; } }
		public int Y { get { return _y; } set { _y = value; } }

		public override bool Equals(Object obj)
		{
			if (!(obj is MyPoint))
				return false;

			return (this.CompareTo(obj) == 0);
		}

		public int CompareTo(Object obj)
		{
			if (!(obj is MyPoint))
				return -1;

			MyPoint mp = (MyPoint)obj;
			int result = (this._x.CompareTo(mp.X));

			if (result != 0)
				return result;

			return this._y.CompareTo(mp.Y);
		}

		// Omitting getHashCode violates FxCop rule: EqualsOverridesRequireGetHashCodeOverride.
		public override int GetHashCode()
		{
			return _x.GetHashCode() ^ _y.GetHashCode();
		}

		// Omitting any of the following operator overloads
		// violates FxCop rule: IComparableImplementationsOverrideOperators.
		public static bool operator ==(MyPoint mp1, MyPoint mp2)
		{
			return mp1.Equals(mp2);
		}

		public static bool operator !=(MyPoint mp1, MyPoint mp2)
		{
			return !(mp1 == mp2);
		}

		public static bool operator <(MyPoint mp1, MyPoint mp2)
		{
			return (mp1.CompareTo(mp2) < 0);
		}

		public static bool operator >(MyPoint mp1, MyPoint mp2)
		{
			return (mp1.CompareTo(mp2) > 0);
		}
	}

	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct MyPerson : IComparable
	{
		String _firstName;
		String _lastName;

		public MyPerson(String firstName, String lastName)
		{
			this._firstName = firstName;
			this._lastName = lastName;
		}

		public string FirstName { get { return _firstName; } set { _firstName = value; } }
		public string LastName { get { return _lastName; } set { _lastName = value; } }

		public override bool Equals(Object obj)
		{
			if (!(obj is MyPerson))
				return false;

			return (this.CompareTo(obj) == 0);
		}

		public int CompareTo(Object obj)
		{
			if (!(obj is MyPerson))
				return -1;
			MyPerson mp = (MyPerson)obj;
			int result = (this._firstName.CompareTo(mp.FirstName));

			if (result != 0)
				return result;

			return this._lastName.CompareTo(mp.LastName);
		}

		// Omitting getHashCode violates FxCop rule: EqualsOverridesRequireGetHashCodeOverride.
		public override int GetHashCode()
		{
			return  _firstName.GetHashCode() ^ _lastName.GetHashCode();
		}

		// Omitting any of the following operator overloads
		// violates FxCop rule: IComparableImplementationsOverrideOperators.
		public static bool operator ==(MyPerson mp1, MyPerson mp2)
		{
			return mp1.Equals(mp2);
		}

		public static bool operator !=(MyPerson mp1, MyPerson mp2)
		{
			return !(mp1 == mp2);
		}

		public static bool operator <(MyPerson mp1, MyPerson mp2)
		{
			return (mp1.CompareTo(mp2) < 0);
		}

		public static bool operator >(MyPerson mp1, MyPerson mp2)
		{
			return (mp1.CompareTo(mp2) > 0);
		}
	}

	internal class NativeMethods
	{
		private NativeMethods() { }
		// this way array size can't be changed and array can be copied back
		// int TestArrayOfInts(int* pArray, int pSize)	
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestArrayOfInts([In, Out] int[] array, int size);

		// this way we could change array size, but array can't be copied back
		// since marshaler doesn't know resulting size, we must do this manually
		// int TestRefArrayOfInts(int** ppArray, int* pSize)	
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestRefArrayOfInts(ref IntPtr array, ref int size);

		// int TestMatrixOfInts(int pMatrix[][COL_DIM], int row)
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestMatrixOfInts([In, Out] int[,] pMatrix, int row);

		// int TestArrayOfStrings(char** ppStrArray, int size)
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestArrayOfStrings([In, Out] String[] stringArray, int size);

		// int TestArrayOfStructs(MYPOINT* pPointArray, int size)
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestArrayOfStructs([In, Out] MyPoint[] pointArray, int size);

		// without [In, Out] strings will not be copied out
		// int TestArrayOfStructs2(MYPERSON* pPersonArray, int size)
		[DllImport("PinvokeLib.dll")]
		internal static extern int TestArrayOfStructs2([In, Out] MyPerson[] personArray, int size);
	}

	public sealed class App
	{
		private App() { }

		public static void Main()
		{
			// *************** array ByVal ********************************
			int[] array1 = new int[10];

			Console.WriteLine("Integer array passed ByVal before call:");
			for (int i = 0; i < array1.Length; i++)
			{
				array1[i] = i;
				Console.Write(" " + array1[i]);
			}

			int sum1 = NativeMethods.TestArrayOfInts(array1, array1.Length);

			Console.WriteLine("\nSum of elements:" + sum1);
			Console.WriteLine("\nInteger array passed ByVal after call:");
			foreach (int i in array1)
			{
				Console.Write(" " + i);
			}

			// *************** array ByRef ********************************
			int[] array2 = new int[10];
			int size = array2.Length;

			Console.WriteLine("\n\nInteger array passed ByRef before call:");
			for (int i = 0; i < array2.Length; i++)
			{
				array2[i] = i;
				Console.Write(" " + array2[i]);
			}

			IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(size) * array2.Length);

			Marshal.Copy(array2, 0, buffer, array2.Length);

			int sum2 = NativeMethods.TestRefArrayOfInts(ref buffer, ref size);

			Console.WriteLine("\nSum of elements:" + sum2);
			if (size > 0)
			{
				int[] arrayRes = new int[size];

				Marshal.Copy(buffer, arrayRes, 0, size);
				Marshal.FreeCoTaskMem(buffer);
				Console.WriteLine("\nInteger array passed ByRef after call:");
				foreach (int i in arrayRes)
				{
					Console.Write(" " + i);
				}
			}
			else
				Console.WriteLine("\nArray after call is empty");

			// *************** matrix ByVal ********************************
			const int DIM = 5;
			int[,] matrix = new int[DIM, DIM];

			Console.WriteLine("\n\nMatrix before call:");
			for (int i = 0; i < DIM; i++)
			{
				for (int j = 0; j < DIM; j++)
				{
					matrix[i, j] = j;
					Console.Write(" " + matrix[i, j]);
				}

				Console.WriteLine("");
			}

			int sum3 = NativeMethods.TestMatrixOfInts(matrix, DIM);

			Console.WriteLine("\nSum of elements:" + sum3);
			Console.WriteLine("\nMatrix after call:");
			for (int i = 0; i < DIM; i++)
			{
				for (int j = 0; j < DIM; j++)
				{
					Console.Write(" " + matrix[i, j]);
				}

				Console.WriteLine("");
			}

			// *************** string array ByVal ********************************
			String[] strArray = { "one", "two", "three", "four", "five" };

			Console.WriteLine("\n\nString array before call:");
			foreach (String s in strArray)
				Console.Write(" " + s);

			int lenSum = NativeMethods.TestArrayOfStrings(strArray, strArray.Length);

			Console.WriteLine("\nSum of string lengths:" + lenSum);
			Console.WriteLine("\nString array after call:");
			foreach (String s in strArray)
			{
				Console.Write(" " + s);
			}

			// *************** struct array ByVal ********************************
			MyPoint[] points = { new MyPoint(1, 1), new MyPoint(2, 2), new MyPoint(3, 3) };

			Console.WriteLine("\n\nPoints array before call:");
			foreach (MyPoint p in points)
				Console.WriteLine("x = {0}, y = {1}", p.X, p.Y);

			int allSum = NativeMethods.TestArrayOfStructs(points, points.Length);

			Console.WriteLine("\nSum of points:" + allSum);
			Console.WriteLine("\nPoints array after call:");
			foreach (MyPoint p in points)
				Console.WriteLine("x = {0}, y = {1}", p.X, p.Y);

			// *************** struct with strings array ByVal *************************
			MyPerson[] persons = {
				new MyPerson("Kim", "Akers"), new MyPerson("Adam", "Barr"), new MyPerson("Jo", "Brown")
			};

			Console.WriteLine("\n\nPersons array before call:");
			foreach (MyPerson pe in persons)
				Console.WriteLine("first = {0}, last = {1}", pe.FirstName, pe.LastName);

			int namesSum = NativeMethods.TestArrayOfStructs2(persons, persons.Length);

			Console.WriteLine("\nSum of name lengths:" + namesSum);
			Console.WriteLine("\n\nPersons array after call:");
			foreach (MyPerson pe in persons)
				Console.WriteLine("first = {0}, last = {1}", pe.FirstName, pe.LastName);
		}
	}
}