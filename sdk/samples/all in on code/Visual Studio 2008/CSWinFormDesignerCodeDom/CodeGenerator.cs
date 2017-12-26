/************************************* Module Header **************************************\
* Module Name:	CodeGenerator.cs
* Project:		CSWinFormDesignerCodeDom
* Copyright (c) Microsoft Corporation.
* 
* 
* This sample demonstrates how to add your own customized generate code for 
* control.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Namespace
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.CodeDom;
#endregion

namespace CSWinFormDesignerCodeDom
{
    #region MyComponent
    [DesignerSerializer(typeof(MyCodeGenerator), typeof(CodeDomSerializer))]
    public class MyComponent : Component
    {
        private List<string> myList;
        public List<string> MyList
        {
            get { return myList; }
            set { myList = value; }
        }

        public MyComponent()
        {
            myList = new List<string>();
            myList.Add("string1");
            myList.Add("string2");
            myList.Add("string3");
            myList.Add("string4");
            myList.Add("string5");
        }
    }
    #endregion

    #region MyCodeGenerator
    public class MyCodeGenerator : CodeDomSerializer
    {
        public override object Deserialize(IDesignerSerializationManager manager, object codeObject)
        {
            CodeDomSerializer baseClassSerializer = (CodeDomSerializer)manager.
            GetSerializer(typeof(MyComponent).BaseType, typeof(CodeDomSerializer));

            return baseClassSerializer.Deserialize(manager, codeObject);
        }

        public override object Serialize(IDesignerSerializationManager manager, object value)
        {
            CodeDomSerializer baseClassSerializer = (CodeDomSerializer)manager.
                GetSerializer(typeof(MyComponent).BaseType, typeof(CodeDomSerializer));

            object codeObject = baseClassSerializer.Serialize(manager, value);

            if (codeObject is CodeStatementCollection)
            {
                CodeStatementCollection statements = (CodeStatementCollection)codeObject;
                // Initial MyList
                // Generate "myComponent1.MyList = new System.Collections.Generic.List<string>();"
                CodeObjectCreateExpression objectCreate1;
                objectCreate1 = new CodeObjectCreateExpression("System.Collections.Generic.List<string>", new CodeExpression[] { });
                CodeAssignStatement as2 =
                    new CodeAssignStatement(new CodeVariableReferenceExpression(manager.GetName(value) + ".MyList"),
                        objectCreate1);
                statements.Insert(0, as2);

                // Add my generated code comment
                string commentText = "MyList generation code";
                CodeCommentStatement comment = new CodeCommentStatement(commentText);
                statements.Insert(1, comment);

                // Add items to MyList
                // Generate the following code
                // this.myComponent1.MyList.Add("string5");
                // this.myComponent1.MyList.Add("string4");
                // this.myComponent1.MyList.Add("string3");
                // this.myComponent1.MyList.Add("string2");
                // this.myComponent1.MyList.Add("string1");
                MyComponent myCom = (MyComponent)manager.GetInstance(manager.GetName(value));
                for (int i = 0; i < myCom.MyList.Count; i++)
                {
                    CodeMethodInvokeExpression methodInvoke = new CodeMethodInvokeExpression(
                        // targetObject that contains the method to invoke.
                        new CodeThisReferenceExpression(),
                        // methodName indicates the method to invoke.
                        manager.GetName(value) + ".MyList.Add",
                        // parameters array contains the parameters for the method.
                        new CodeExpression[] { new CodePrimitiveExpression(myCom.MyList[i]) });
                    CodeExpressionStatement expressionStatement;
                    expressionStatement = new CodeExpressionStatement(methodInvoke);
                    statements.Insert(2, expressionStatement);

                }
                // Remove system generated code
                statements.RemoveAt(statements.Count - 1);
            }
            return codeObject;
        }
    }
    #endregion
}