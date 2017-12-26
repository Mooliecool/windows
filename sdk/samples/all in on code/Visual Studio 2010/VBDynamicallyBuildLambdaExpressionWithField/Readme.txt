======================================================================================
      Windows APPLICATION: VBDynamicallyBuildLambdaExpressionWithField Overview                        
======================================================================================

//////////////////////////////////////////////////////////////////////////////////////
Summary:

  This sample demonstrates how to dynamically build lambda expression and show data 
  into DataGridView Control.

  This sample shows up multiple conditions jointing together and dynamically 
  generate  LINQ TO SQL. LINQ is a great way to declaratively filter and query data 
  in a Type_Safe,Intuitive,and very expressive way.this sample achieve it. For example,
  the search feature in this application allow the customer to find all records that 
  meet criteria defined on multiple columns.
 
   
//////////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build this project in Visual Studio 2010. 
 
Step2. Download SQL2000SampleDb.msi according to the website mentioned as follows:
       http://www.microsoft.com/downloads/en/details.aspx?FamilyID=06616212-0356-46a0-8da2-eebc53a68034&displaylang=en
      
Step3. Install it into your system catalog and startup your SQL Server Management Studio.

Step4. Right click the treenode written as "Databases" and left click "Attach...".

Step5. Make sure just Northwind.MDF and Northwind.LDF are installed in the system catalog,
       which has complete access control privileges.Under the above condition,
       Select the button written as "Add..." inside Attach Databases Dialog to attach 
       Northwind database. 

Step6. Select app.config inside the NorthwindApp project to modify the connections string 
       or double click Settings.settings under the NorthwindApp project and modify column
       written as "value".

Step7. Right click the NorthwindApp project and click the menuitem written as "Set as Startup Project".

Step8. Click F5 shortcut key and select condition field ,condition operator and condition value.

Step9. Click the button written as "Search" and view the result parsed by LINQ TO SQL.


//////////////////////////////////////////////////////////////////////////////////////
Code Logic:
1. There are three main classes: Condition, Condition(Of T), and Condition(Of T,S)
   a. Condition is an abstract class that is used to construct the generic versions.
   By structuring it this we get the benefits of generic type parameter inference-i.e.
   we don't have to worry about passing the generic type parameters to the method;
   the factory method figures it out for us.
   b. Condition(Of T) is used to join multiple conditions together. T is the element
   type(i.e. Order in the example above).
   c. Condition(Of T,S) is the simplest type;it represents an "object.propery<comparison> value"
   expression. The type parameter S will be inferred to be the type of the value passed
   in(i.e. String,Date,Boolean etc...).
2. For local execution of a query we compile the LambdaExpression to a delegate so that
   it can be executed in-memory. The user can invoke this delegate by calling the Matches 
   method.
   ' Compile the lambda expression into a delegate
  	del = CType(LambdaExpr.Compile(), Func(Of T, Boolean))
3. The extension methods at the bottom are defined on IQueryable(Of T) for remote execution,
   and IEnumerable(Of T) for local execution.


//////////////////////////////////////////////////////////////////////////////////////
References:

Implementing Dynamic Searching Using LINQ 
http://blogs.msdn.com/b/vbteam/archive/2007/08/29/implementing-dynamic-searching-using-linq.aspx

LINQ to SQL (Part 9 - Using a Custom LINQ Expression with the <asp:LinqDatasource> control) 
http://weblogs.asp.net/scottgu/archive/2007/09/07/linq-to-sql-part-9-using-a-custom-linq-expression-with-the-lt-asp-linqdatasource-gt-control.aspx


//////////////////////////////////////////////////////////////////////////////////////
