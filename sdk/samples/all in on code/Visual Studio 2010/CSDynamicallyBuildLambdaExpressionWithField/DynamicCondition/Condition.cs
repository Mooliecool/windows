/********************************** Module Header ***********************************************\
* Module Name:  Condition.cs
* Project:      CSDynamicallyBuildLambdaExpressionWithField
* Copyright (c) Microsoft Corporation.
*
* The Condition.cs file defines a LINQ operator concerning with System.Linq.Expressions namespace.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\************************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;

namespace DynamicCondition
{
    public static class DynamicQuery
    {
        #region Condition Classes

        public abstract class Condition
        {

            #region Fields

            // Used to ensure we get the same instance of a particular ParameterExpression
            // across multiple queries
            private static Dictionary<string, ParameterExpression> paramTable =
                new Dictionary<string, ParameterExpression>();

            // The expression tree which will be passed to the LINQ to SQL runtime
            protected internal LambdaExpression LambdaExpr;

            // Enumerates all the different comparisons which can be performed.
            public enum Compare : int
            {
                Or = ExpressionType.Or,
                And = ExpressionType.And,
                Xor = ExpressionType.ExclusiveOr,
                Not = ExpressionType.Not,
                Equal = ExpressionType.Equal,
                Like = ExpressionType.TypeIs + 1,
                NotEqual = ExpressionType.NotEqual,
                OrElse = ExpressionType.OrElse,
                AndAlso = ExpressionType.AndAlso,
                LessThan = ExpressionType.LessThan,
                GreaterThan = ExpressionType.GreaterThan,
                LessThanOrEqual = ExpressionType.LessThanOrEqual,
                GreaterThanOrEqual = ExpressionType.GreaterThanOrEqual
            }
            #endregion

            /// <summary>
            /// Constructs a Condition with T as the element type and S as the value's type
            /// </summary>
            public static Condition<T, S> Create<T, S>(IEnumerable<T> dataSource, string propertyName,
                Compare condType, S value)
            {

                return new Condition<T, S>(propertyName, condType, value);
            }

            /// <summary>
            /// Constructs a Condition with T as the element type and valueType as the value's type
            /// This is useful for situations where you won't know the value's type until runtime.
            /// </summary>
            public static Condition<T> Create<T>(IEnumerable<T> dataSource, string propertyName,
                Compare condType, object value, Type valueType)
            {

                return new Condition<T>(propertyName, condType, value, valueType);
            }


            /// <summary>
            /// Creates a Condition which combines two other Conditions
            /// </summary>
            /// <typeparam name="T">The type the condition will execute against</typeparam>
            /// <param name="cond1">The first Condition</param>
            /// <param name="condType">The operator to use on the conditions</param>
            /// <param name="cond2">The second Condition</param>
            /// <returns>A new Condition which combines two Conditions into one according to the 
            /// specified operator</returns>
            /// <remarks></remarks>
            public static Condition<T> Combine<T>(Condition<T> cond1, Compare condType, Condition<T> cond2)
            {
                return Condition<T>.Combine(cond1, condType, cond2);
            }

            /// <summary>
            /// Combines multiple conditions according to the specified operator
            /// </summary>
            public static Condition<T> Combine<T>(Condition<T> cond1, Compare condType,
                params Condition<T>[] conditions)
            {
                return Condition<T>.Combine(cond1, condType, conditions);
            }


            #region Protected Methods
            /// <summary>
            /// Combines two Expressions according to the specified operator (condType)
            /// </summary>
            protected static Expression CombineExpression(Expression left, Compare condType, Expression right)
            {

                //Join the Expressions based on the operator
                switch (condType)
                {
                    case Compare.Or:
                        return Expression.Or(left, right);
                    case Compare.And:
                        return Expression.And(left, right);
                    case Compare.Xor:
                        return Expression.ExclusiveOr(left, right);
                    case Compare.Equal:
                        return Expression.Equal(left, right);
                    case Compare.OrElse:
                        return Expression.OrElse(left, right);
                    case Compare.AndAlso:
                        return Expression.AndAlso(left, right);
                    case Compare.NotEqual:
                        return Expression.NotEqual(left, right);
                    case Compare.LessThan:
                        return Expression.LessThan(left, right);
                    case Compare.GreaterThan:
                        return Expression.GreaterThan(left, right);
                    case Compare.LessThanOrEqual:
                        return Expression.LessThanOrEqual(left, right);
                    case Compare.GreaterThanOrEqual:
                        return Expression.GreaterThanOrEqual(left, right);
                    case Compare.Like:

                        //For the Like operator we encode a call to the LikeString method in the VB runtime
                        var m = typeof(Microsoft.VisualBasic.CompilerServices.Operators).GetMethod("LikeString");

                        return Expression.Call(m, left, right, Expression.Constant(Microsoft.VisualBasic.CompareMethod.Binary));

                    default:
                        throw new ArgumentException("Not a valid Condition Type", "condType", null);
                }
            }

            /// <summary>
            /// Since both type parameters must be the same, we can turn what would normally
            /// be a Func(Of T, T, Boolean) into a Func(Of T, Boolean)
            /// </summary>
            protected static Func<T, Boolean> CombineFunc<T>(Func<T, Boolean> d1, Compare condType, Func<T, Boolean> d2)
            {

                //Return a delegate which combines delegates d1 and d2
                switch (condType)
                {
                    case Compare.Or:
                        return (x) => d1(x) | d2(x);
                    case Compare.And:
                        return (x) => d1(x) & d2(x);
                    case Compare.Xor:
                        return (x) => d1(x) ^ d2(x);
                    case Compare.Equal:
                        return (x) => d1(x) == d2(x);
                    case Compare.OrElse:
                        return (x) => d1(x) || d2(x);
                    case Compare.AndAlso:
                        return (x) => d1(x) && d2(x);
                    case Compare.NotEqual:
                        return (x) => d1(x) != d2(x);
                    case Compare.LessThan:
                        return (x) => int.Parse(d1(x).ToString()) < int.Parse(d2(x).ToString());
                    case Compare.GreaterThan:
                        return (x) => int.Parse(d1(x).ToString()) > int.Parse(d2(x).ToString());
                    case Compare.LessThanOrEqual:
                        return (x) => int.Parse(d1(x).ToString()) <= int.Parse(d2(x).ToString());
                    case Compare.GreaterThanOrEqual:
                        return (x) => int.Parse(d1(x).ToString()) >= int.Parse(d2(x).ToString());
                    default:
                        throw new ArgumentException("Not a valid Condition Type", "condType");
                }
            }

            /// <summary>
            /// Guarantees that we get the same instance of a ParameterExpression for a given type t.
            /// </summary>
            protected static ParameterExpression GetParamInstance(Type dataType)
            {

                //Parameters are matched by reference, not by name, so we cache the instances in a Dictionary.
                if (!(paramTable.ContainsKey(dataType.Name)))
                {
                    paramTable.Add(dataType.Name, Expression.Parameter(dataType, dataType.Name));
                }

                return paramTable[dataType.Name];
            }
            #endregion

        }

        public class Condition<T> : Condition
        {

            //Delegate that contains a compiled expression tree which can be run locally
            internal Func<T, bool> del;

            #region Constructors

            internal Condition()
            {
            }

            internal Condition(string propName, Compare condType, object value, Type valueType)
            {
                // Split the string to handle nested property access
                var s = propName.Split('.');

                // Get the PropertyInfo instance for propName
                var pInfo = typeof(T).GetProperty(s[0]);
                var paramExpr = GetParamInstance(typeof(T));
                var callExpr = Expression.MakeMemberAccess(paramExpr, pInfo);

                // For each member specified, construct the additional MemberAccessExpression
                // For example, if the user says "myCustomer.Order.OrderID = 4" we need an
                // additional MemberAccessExpression for "Order.OrderID = 4"
                for (var i = 1; i <= s.GetUpperBound(0); i++)
                {
                    pInfo = pInfo.PropertyType.GetProperty(s[i]);
                    callExpr = Expression.MakeMemberAccess(callExpr, pInfo);
                }

                // ConstantExpression representing the value on the left side of the operator
                var valueExpr = Expression.Constant(value, valueType);

                Expression b = CombineExpression(callExpr, condType, valueExpr);
                LambdaExpr = Expression.Lambda<Func<T, bool>>(b, new ParameterExpression[] { paramExpr });

                // Compile the lambda expression into a delegate
                del = (Func<T, bool>)(LambdaExpr.Compile());
            }

            #endregion

            #region Methods

            // Combines two conditions according to the specified operator
            internal static Condition<T> Combine(Condition<T> cond1, Compare condType, Condition<T> cond2)
            {
                Condition<T> c = new Condition<T>();

                Expression b = CombineExpression(cond1.LambdaExpr.Body, condType, cond2.LambdaExpr.Body);

                var paramExpr = new ParameterExpression[] { GetParamInstance(typeof(T)) };

                // Create the LambdaExpression and compile the delegate
                c.LambdaExpr = Expression.Lambda<Func<T, bool>>(b, paramExpr);
                c.del = Condition.CombineFunc(cond1.del, condType, cond2.del);

                return c;
            }

            // Combines multiple conditions according to the specified operator
            internal static Condition<T> Combine(Condition<T> cond1, Compare condType, params Condition<T>[] conditions)
            {
                var finalCond = cond1;
                foreach (var c in conditions)
                {
                    finalCond = Condition.Combine(finalCond, condType, c);
                }

                return finalCond;
            }

            // Run query locally instead of remotely
            public bool Matches(T row)
            {
                return del(row); // Passes the row into the delegate to see if it's a match
            }

            #endregion

            #region Overloaded Operators

            // Overloaded operators - allows syntax like "(condition1 Or condition2) And condition3"
            public static Condition<T> operator &(Condition<T> c1, Condition<T> c2)
            {
                return Condition.Combine(c1, Compare.And, c2);
            }

            public static Condition<T> operator |(Condition<T> c1, Condition<T> c2)
            {
                return Condition.Combine(c1, Compare.Or, c2);
            }

            public static Condition<T> operator ^(Condition<T> c1, Condition<T> c2)
            {
                return Condition.Combine(c1, Compare.Xor, c2);
            }

            #endregion

        }

        // Represents a condition like "object.Property = value"
        // In this case object is of type T, and value is of type S
        //
        // Even though most of the logic for this is already in the base class, 
        // defining a second generic parameter means the user doesn't have to
        // pass in a System.Type - it can just be inferred.
        public class Condition<T, S> : Condition<T>
        {
            internal Condition(string propName, Compare condType, S value)
                : base(propName, condType,
                    value, typeof(S))
            {
            }
        }

        #endregion

        #region Extension Methods

        /// <summary>
        /// Filters an IQueryable(Of T) according to the specified condition
        /// </summary>
        public static IQueryable<T> Where<T>(this IQueryable<T> source, Condition<T> condition)
        {

            var callExpr = Expression.Call(typeof(Queryable), "Where", new Type[] { source.ElementType },
                source.Expression, Expression.Quote(condition.LambdaExpr));

            return (IQueryable<T>)(source.Provider.CreateQuery(callExpr));
        }

        /// <summary>
        /// Filters an IEnumerable(Of T) according to the specified condition
        /// </summary>
        public static IEnumerable<T> Where<T>(this IEnumerable<T> source, Condition<T> condition)
        {
            return source.Where(condition.del);
        }

        /// <summary>
        ///  Extension method that can be called off any type that implements IEnumerable(Of T), 
        ///  which constructs a Condition with T as the element type and S as the value's type
        /// </summary>
        public static Condition<T, S> CreateCondition<T, S>(this IEnumerable<T> dataSource, string propName,
            DynamicCondition.DynamicQuery.Condition.Compare condType, S value)
        {

            return Condition.Create(dataSource, propName, condType, value);
        }
        #endregion
    }
}