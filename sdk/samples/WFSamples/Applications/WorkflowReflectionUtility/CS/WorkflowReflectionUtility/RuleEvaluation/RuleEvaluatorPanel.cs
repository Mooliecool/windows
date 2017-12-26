//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
//---------------------------------------------------------------------

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components
{
    using System;
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Collections.Generic;
    using System.Drawing;
    using System.IO;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Text;
    using System.Windows.Forms;
    using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Rules;

    /// <summary>
    /// Panel for displaying and evaluating CodeExpressions.
    /// </summary>
    public class RuleEvaluatorPanel : Panel
    {
        private const int ExpressionTop = 10;
        private const int ExpressionLeft = 10;

        private CodeExpression currentExpressionValue;
        private Dictionary<RectangleF, Brush> highlightedAreas;
        private IExpressionEvaluator evaluator;
        private CodeDomProvider provider;
        private Button evaluateButton;
        private Label resultLabel;
        private ContextMenuStrip sharedContextMenu;
        private int expressionRight;

        /// <summary>
        /// Default constructor.  Sets all values to defaults.
        /// </summary>
        public RuleEvaluatorPanel()
        {
            // Perform initial layout.
            this.expressionRight = 0;

            this.evaluateButton = new Button();
            this.evaluateButton.Text = "Evaluate";

            Graphics graphics = this.CreateGraphics();
            SizeF fontHeightF = graphics.MeasureString("T", this.Font);
            Size fontHeight = Size.Truncate(fontHeightF);
            int safeHeight = fontHeight.Height + 1;

            this.evaluateButton.Left = ExpressionLeft;
            this.evaluateButton.Top = ExpressionTop + safeHeight + 10;
            this.evaluateButton.Click += OnEvaluateClick;
            this.Controls.Add(evaluateButton);

            this.Height = evaluateButton.Bottom + 10;

            Label results = new Label();
            results.Text = "Last Result:";

            SizeF textLengthF = graphics.MeasureString(results.Text, results.Font);
            Size textLength = Size.Truncate(textLengthF);
            int safeWidth = textLength.Width + 1;

            results.Width = safeWidth;
            results.Top = evaluateButton.Top;
            results.Left = evaluateButton.Right + 10;
            this.Controls.Add(results);

            this.resultLabel = new Label();
            this.resultLabel.Text = string.Empty;
            this.resultLabel.Font = new Font(this.Font, FontStyle.Bold);
            this.resultLabel.Top = results.Top;
            this.resultLabel.Left = results.Right + 5;
            this.resultLabel.Width = 0;
            this.Controls.Add(resultLabel);

            this.Width = resultLabel.Right + 10;

            this.sharedContextMenu = new ContextMenuStrip();

            this.provider = CodeDomProvider.CreateProvider("C#");
            this.evaluator = null;
            this.currentExpressionValue = null;
            this.highlightedAreas = new Dictionary<RectangleF, Brush>();
        }

        /// <summary>
        /// Clone an expression and display it on the panel.
        /// </summary>
        /// <param name="newExpression">Expression to clone and display</param>
        /// <param name="thisType">Type to which CodeThisReferenceExpression
        /// is meant to refer within the CodeExpression</param>
        public void CloneAndSetExpression(CodeExpression newExpression, Type thisType)
        {
            if (currentExpressionValue != null)
            {
                this.DisableButtons(currentExpressionValue);
                this.highlightedAreas.Clear();
            }

            this.currentExpressionValue = CloneExpression(newExpression);

            this.evaluator = EvaluatorGenerator.GenerateEvaluator(currentExpressionValue, thisType);

            this.AdornData(currentExpressionValue);

            this.Refresh();
            this.CheckEvaluateButtonEnable();
        }

        public CodeExpression CurrentExpression
        {
            get
            {
                return this.currentExpressionValue;
            }
        }

        private CodeExpression CloneExpression(CodeExpression original)
        {
            BinaryFormatter binaryFormatter = new BinaryFormatter();
            MemoryStream memory = new MemoryStream();
            binaryFormatter.Serialize(memory, original);
            memory.Seek(0, SeekOrigin.Begin);

            return binaryFormatter.Deserialize(memory) as CodeExpression;
        }

        private enum ParentSide
        {
            Left = 0,
            Right = 1
        }

        private class ParentLink
        {
            public CodeBinaryOperatorExpression parent;
            public ParentSide side;

            public ParentLink(CodeBinaryOperatorExpression parent, ParentSide side)
            {
                this.parent = parent;
                this.side = side;
            }

            public static CodeBinaryOperatorExpression GetParent(CodeExpression expression)
            {
                if (expression.UserData.Contains(typeof(ParentLink)))
                {
                    return ((ParentLink)expression.UserData[typeof(ParentLink)]).parent;
                }
                else
                {
                    throw new ArgumentException("Specified expression does not have a ParentLink", "expression");
                }
            }

            public static ParentSide GetSide(CodeExpression expression)
            {
                if (expression.UserData.Contains(typeof(ParentLink)))
                {
                    return ((ParentLink)expression.UserData[typeof(ParentLink)]).side;
                }
                else
                {
                    throw new ArgumentException("Specified expression does not have a ParentLink", "expression");
                }
            }
        }

        private bool AdornData(CodeExpression root)
        {
            if (root is CodeBinaryOperatorExpression)
            {
                CodeBinaryOperatorExpression bin = (CodeBinaryOperatorExpression)root;

                bin.Right.UserData.Add(typeof(ParentLink), new ParentLink(bin, ParentSide.Right));
                bin.Left.UserData.Add(typeof(ParentLink), new ParentLink(bin, ParentSide.Left));

                bool leftRequires = AdornData(bin.Left);

                bool rightRequires = AdornData(bin.Right);

                if (leftRequires || rightRequires)
                {
                    this.CreateButtonAndMenu(root);

                    return true;
                }

                return false;
            }
            else
            {
                if (root is CodePrimitiveExpression)
                {
                    return false;
                }
                else
                {
                    CreateButtonAndMenu(root);
                    return true;
                }
            }
        }

        private class ButtonAndMenuData
        {
            public Button button;
            public List<ToolStripItem> menuItems;

            public ButtonAndMenuData(Button button, List<ToolStripItem> menuItems)
            {
                this.button = button;
                this.menuItems = menuItems;
            }

            public static bool TryGetValue(CodeExpression expression, out Button button)
            {
                if (expression.UserData.Contains(typeof(ButtonAndMenuData)))
                {
                    button = ((ButtonAndMenuData)expression.UserData[typeof(ButtonAndMenuData)]).button;

                    return true;
                }
                else
                {
                    button = null;
                    return false;
                }
            }

            public static List<ToolStripItem> GetMenuItems(CodeExpression expression)
            {
                if (expression.UserData.Contains(typeof(ButtonAndMenuData)))
                {
                    return ((ButtonAndMenuData)expression.UserData[typeof(ButtonAndMenuData)]).menuItems;
                }
                else
                {
                    throw new ArgumentException("Expression provided does not contain ButtonAndMenuData.", "expression");
                }
            }
        }

        private void CreateButtonAndMenu(CodeExpression root)
        {
            Button menuButton = new Button();
            menuButton.Size = new Size(5, 5);
            menuButton.FlatStyle = FlatStyle.Popup;
            menuButton.Click += OnSubExpressionButtonClick;
            menuButton.Leave += OnSubExpressionButtonLeave;
            menuButton.Visible = false;

            this.Controls.Add(menuButton);

            List<ToolStripItem> menuItems = new List<ToolStripItem>();

            ButtonAndMenuData data = new ButtonAndMenuData(menuButton, menuItems);
            root.UserData.Add(typeof(ButtonAndMenuData), data);

            ToolStripMenuItem menuItem = new ToolStripMenuItem(Serialize(root));
            menuItem.Click += OnContextMenuItemClick;
            menuItem.Tag = new ToExchange(root, root);
            menuItems.Add(menuItem);

            Type rootType = EvaluatorGenerator.GetType(root, false);

            if (rootType != null)
            {
                if (typeof(bool) == rootType)
                {
                    ToolStripMenuItem trueItem = new ToolStripMenuItem("True");
                    CodePrimitiveExpression trueExpression = new CodePrimitiveExpression(true);
                    trueExpression.UserData.Add(typeof(ButtonAndMenuData), data);
                    EvaluatorGenerator.SetType(trueExpression, typeof(bool));
                    EvaluatorGenerator.SetParameterIndex(trueExpression, EvaluatorGenerator.GetParameterIndex(root, true));
                    trueItem.Tag = new ToExchange(root, trueExpression);
                    trueItem.Click += OnContextMenuItemClick;

                    menuItems.Add(trueItem);

                    ToolStripMenuItem falseItem = new ToolStripMenuItem("False");
                    CodePrimitiveExpression falseExpression = new CodePrimitiveExpression(false);
                    falseExpression.UserData.Add(typeof(ButtonAndMenuData), data);
                    EvaluatorGenerator.SetType(falseExpression, typeof(bool));
                    EvaluatorGenerator.SetParameterIndex(falseExpression, EvaluatorGenerator.GetParameterIndex(root, true));
                    falseItem.Tag = new ToExchange(root, falseExpression);
                    falseItem.Click += OnContextMenuItemClick;

                    menuItems.Add(falseItem);
                }
                else if (typeof(IConvertible).IsAssignableFrom(rootType))
                {
                    ToolStripSeparator separator = new ToolStripSeparator();
                    menuItems.Add(separator);

                    ToolStripTextBox textBox = new ToolStripTextBox();
                    textBox.AcceptsReturn = false;
                    menuItems.Add(textBox);

                    ToolStripMenuItem setValue = new ToolStripMenuItem(string.Format("Enter {0}", EvaluatorGenerator.GetType(root, true).FullName));
                    setValue.Tag = new ToExchange(root, textBox);
                    setValue.Click += OnSetValueClick;
                    menuItems.Add(setValue);
                }
            }
        }

        private string Serialize(CodeExpression codeExpression)
        {
            StringWriter stringWriter = new StringWriter();
            this.provider.GenerateCodeFromExpression(codeExpression, stringWriter, new CodeGeneratorOptions());
            String expression = stringWriter.ToString().Trim().Replace(stringWriter.NewLine, " ");

            int startRemove = -1;
            int removeCount = 0;

            for (int i = 0; i < expression.Length; i++)
            {
                if (expression[i] == ' ')
                {
                    if (startRemove == -1)
                    {
                        startRemove = i;
                    }
                    else
                    {
                        removeCount++;
                    }
                }
                else
                {
                    if (removeCount != 0)
                    {
                        expression = expression.Remove(startRemove, removeCount);
                        removeCount = 0;
                    }

                    startRemove = -1;
                }
            }

            return expression;
        }

        private class ButtonInfo
        {
            public CodeExpression associatedExpression;
            public RectangleF toHighlight;

            public ButtonInfo()
            {
            }

            public ButtonInfo(CodeExpression expression, RectangleF toHighlight)
            {
                this.associatedExpression = expression;
                this.toHighlight = toHighlight;
            }
        }

        private void Flush(ref StringBuilder stringBuffer, ref PointF point, Graphics graphics)
        {
            SizeF size = graphics.MeasureString(stringBuffer.ToString(), this.Font);
            graphics.DrawString(stringBuffer.ToString(), this.Font, Brushes.Black, point);

            stringBuffer = new StringBuilder();
            point = new PointF(point.X + size.Width, point.Y);
        }

        private void DrawOrFlush(CodeExpression expression, ref StringBuilder stringBuffer, Graphics graphics, PointF startPoint, ref PointF point)
        {
            Button button = null;
            if (ButtonAndMenuData.TryGetValue(expression, out button))
            {
                SizeF size = graphics.MeasureString(stringBuffer.ToString(), this.Font);
                graphics.DrawString(stringBuffer.ToString(), this.Font, Brushes.Black, point);

                stringBuffer = new StringBuilder();
                point = new PointF(point.X + size.Width + 5, point.Y);

                button.Location = new Point((int)point.X - 5, (int)point.Y - 5);
                button.Tag = new ButtonInfo(expression, new RectangleF(startPoint, new SizeF(point.X - startPoint.X - 5, size.Height)));
                button.Visible = true;
            }
            else
            {
                this.Flush(ref stringBuffer, ref point, graphics);
            }
        }

        private void DrawExpression(CodeExpression root, ref StringBuilder stringBuffer, Graphics graphics, ref PointF point)
        {
            if (root is CodeBinaryOperatorExpression)
            {
                PointF startPoint = point;
                CodeBinaryOperatorExpression binaryOperator = (CodeBinaryOperatorExpression)root;

                if (binaryOperator.Left is CodeBinaryOperatorExpression)
                {
                    stringBuffer.Append("(");
                    this.Flush(ref stringBuffer, ref point, graphics);
                }

                this.DrawExpression(binaryOperator.Left, ref stringBuffer, graphics, ref point);

                if (binaryOperator.Left is CodeBinaryOperatorExpression)
                {
                    stringBuffer.Append(")");

                    this.DrawOrFlush(binaryOperator.Left, ref stringBuffer, graphics, startPoint, ref point);
                }


                stringBuffer.Append(" " + ExtractOperator(binaryOperator) + " ");
                this.Flush(ref stringBuffer, ref point, graphics);

                startPoint = point;

                if (binaryOperator.Right is CodeBinaryOperatorExpression)
                {
                    stringBuffer.Append("(");
                    this.Flush(ref stringBuffer, ref point, graphics);
                }

                DrawExpression(binaryOperator.Right, ref stringBuffer, graphics, ref point);

                if (binaryOperator.Right is CodeBinaryOperatorExpression)
                {
                    stringBuffer.Append(")");

                    this.DrawOrFlush(binaryOperator.Right, ref stringBuffer, graphics, startPoint, ref point);
                }
            }
            else
            {
                stringBuffer.Append(Serialize(root));

                PointF startPoint = point;

                this.DrawOrFlush(root, ref stringBuffer, graphics, startPoint, ref point);
            }
        }

        private string ExtractOperator(CodeBinaryOperatorExpression expression)
        {
            CodeBinaryOperatorExpression newExpression =
                new CodeBinaryOperatorExpression(
                    new CodePrimitiveExpression(1),
                    expression.Operator,
                    new CodePrimitiveExpression(1));

            StringWriter stringWriter = new StringWriter();
            this.provider.GenerateCodeFromExpression(newExpression, stringWriter, new CodeGeneratorOptions());
            string returnValue = stringWriter.ToString();
            returnValue = returnValue.Substring(returnValue.IndexOf(' ') + 1);
            returnValue = returnValue.Substring(0, returnValue.IndexOf(' '));

            return returnValue.Trim();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (currentExpressionValue != null)
            {
                Graphics graphics = e.Graphics;

                foreach (KeyValuePair<RectangleF, Brush> toHighlight in highlightedAreas)
                {
                    graphics.FillRectangle(toHighlight.Value, toHighlight.Key);
                }

                StringBuilder stringBuilder = new StringBuilder();
                PointF point = new PointF((float)ExpressionLeft, (float)ExpressionTop);
                this.DrawExpression(currentExpressionValue, ref stringBuilder, graphics, ref point);

                Point integerPoint = Point.Truncate(point);
                this.expressionRight = integerPoint.X + 1;

                this.ResizePanel();
            }

            base.OnPaint(e);
        }

        private void OnSubExpressionButtonClick(object sender, EventArgs e)
        {
            Button button = (Button)sender;

            ButtonInfo buttonInfo = (ButtonInfo)button.Tag;
            if (!highlightedAreas.ContainsKey(buttonInfo.toHighlight))
            {
                this.highlightedAreas.Add(buttonInfo.toHighlight, Brushes.Yellow);
            }

            Rectangle toInvalidate = Rectangle.Truncate(buttonInfo.toHighlight);
            toInvalidate.Height += 1;
            toInvalidate.Width += 1;
            this.Invalidate(toInvalidate);

            foreach (ToolStripItem item in sharedContextMenu.Items)
            {
                if (item is ToolStripTextBox)
                {
                    ((ToolStripTextBox)item).Control.Parent.Controls.Remove(((ToolStripTextBox)item).Control);
                }
            }

            this.sharedContextMenu.Items.Clear();

            this.sharedContextMenu.Items.AddRange(ButtonAndMenuData.GetMenuItems(buttonInfo.associatedExpression).ToArray());

            this.sharedContextMenu.Show(this.PointToScreen(new Point(button.Right, button.Bottom)), ToolStripDropDownDirection.AboveRight);
        }

        private class ToExchange
        {
            public CodeExpression original;
            public CodeExpression newExpression;
            public ToolStripTextBox valueBox;

            public ToExchange(CodeExpression original, CodeExpression newExpression)
            {
                this.original = original;
                this.newExpression = newExpression;
            }

            public ToExchange(CodeExpression original, ToolStripTextBox textBox)
            {
                this.original = original;
                this.valueBox = textBox;
            }
        }

        private void OnSubExpressionButtonLeave(object sender, EventArgs e)
        {
            Button button = (Button)sender;

            if (button.Tag == null)
            {
                return;
            }

            ButtonInfo bInfo = (ButtonInfo)button.Tag;

            if (this.highlightedAreas.ContainsKey(bInfo.toHighlight))
            {
                this.highlightedAreas.Remove(bInfo.toHighlight);
            }

            Rectangle toInvalidate = Rectangle.Truncate(bInfo.toHighlight);
            toInvalidate.Height += 1;
            toInvalidate.Width += 1;
            this.Invalidate(toInvalidate);
        }

        private void OnContextMenuItemClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            if (item.Tag is ToExchange)
            {
                ToExchange exchange = (ToExchange)item.Tag;

                CodeBinaryOperatorExpression expression = ParentLink.GetParent(exchange.original);

                if (ParentLink.GetSide(exchange.original) == ParentSide.Left)
                {
                    expression.Left = exchange.newExpression;
                }
                else
                {
                    expression.Right = exchange.newExpression;
                }

                this.DisableButtons(exchange.original);

                this.highlightedAreas.Clear();
                this.Refresh();

                this.CheckEvaluateButtonEnable();
            }
        }

        private void CheckEvaluateButtonEnable()
        {
            if (RecursiveCheckAllPrimitive(currentExpressionValue))
            {
                this.evaluateButton.Enabled = true;
            }
            else
            {
                this.evaluateButton.Enabled = false;
            }
        }

        private bool RecursiveCheckAllPrimitive(CodeExpression expression)
        {
            if (expression is CodeBinaryOperatorExpression)
            {
                CodeBinaryOperatorExpression binaryOperator = (CodeBinaryOperatorExpression)expression;

                return RecursiveCheckAllPrimitive(binaryOperator.Left) && RecursiveCheckAllPrimitive(binaryOperator.Right);
            }
            else if (expression is CodePrimitiveExpression)
            {
                return true;
            }

            return false;
        }

        private void DisableButtons(CodeExpression expression)
        {
            if (expression is CodeBinaryOperatorExpression)
            {
                this.DisableButtons(((CodeBinaryOperatorExpression)expression).Left);
                this.DisableButtons(((CodeBinaryOperatorExpression)expression).Right);
            }

            Button button = null;
            if (ButtonAndMenuData.TryGetValue(expression, out button))
            {
                button.Visible = false;
            }
        }

        private void OnSetValueClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            ToExchange exchange = (ToExchange)item.Tag;

            ToolStripTextBox textBox = exchange.valueBox;

            object primitiveObject = null;

            try
            {
                primitiveObject = Convert.ChangeType(textBox.Text, EvaluatorGenerator.GetType(exchange.original, true));
            }
            catch (FormatException)
            {
                MessageBox.Show(string.Format("Could not convert '{0}' to object of type '{1}'.  Please enter a value into the textbox in the context menu.", textBox.Text, EvaluatorGenerator.GetType(exchange.original, false)));
                return;
            }

            CodePrimitiveExpression replacement = new CodePrimitiveExpression(primitiveObject);
            replacement.UserData.Add(typeof(ButtonAndMenuData), exchange.original.UserData[typeof(ButtonAndMenuData)]);
            EvaluatorGenerator.SetType(replacement, EvaluatorGenerator.GetType(exchange.original, true));
            EvaluatorGenerator.SetParameterIndex(replacement, EvaluatorGenerator.GetParameterIndex(exchange.original, true));

            CodeBinaryOperatorExpression expression = ParentLink.GetParent(exchange.original);

            if (ParentLink.GetSide(exchange.original) == ParentSide.Left)
            {
                expression.Left = replacement;
            }
            else
            {
                expression.Right = replacement;
            }

            ToolStripMenuItem menuItem = new ToolStripMenuItem(textBox.Text);
            menuItem.Tag = new ToExchange(exchange.original, replacement);
            menuItem.Click += OnContextMenuItemClick;
            List<ToolStripItem> menuItems = ButtonAndMenuData.GetMenuItems(exchange.original);
            menuItems.Insert(1, menuItem);

            if (menuItems.Count > 3 + 1 + 5)
            {
                menuItems.RemoveAt(6);
            }

            textBox.Clear();

            this.highlightedAreas.Clear();
            this.Refresh();
            this.CheckEvaluateButtonEnable();
        }

        private void OnEvaluateClick(object sender, EventArgs e)
        {
            EvaluatorParameters parameters = evaluator.CreateEmptyParameters();

            this.SetParameters(currentExpressionValue, parameters);

            this.resultLabel.Text = evaluator.Evaluate(parameters).ToString();

            Graphics graphics = resultLabel.CreateGraphics();
            SizeF resultSizeF = graphics.MeasureString(resultLabel.Text, resultLabel.Font);
            Size resultSize = Size.Truncate(resultSizeF);

            this.resultLabel.Width = resultSize.Width + 1;
            this.ResizePanel();
        }

        private void ResizePanel()
        {
            this.Width = (expressionRight + 10 < resultLabel.Right + 10 ? resultLabel.Right + 10 : expressionRight + 10);
        }

        private void SetParameters(CodeExpression root, EvaluatorParameters parameters)
        {
            if (root is CodeBinaryOperatorExpression)
            {
                CodeBinaryOperatorExpression bin = (CodeBinaryOperatorExpression)root;

                this.SetParameters(bin.Left, parameters);
                this.SetParameters(bin.Right, parameters);
            }
            else
            {
                if (root is CodePrimitiveExpression)
                {
                    CodePrimitiveExpression primitive = (CodePrimitiveExpression)root;

                    int parameterNumber = EvaluatorGenerator.GetParameterIndex(primitive, false);

                    if (parameterNumber >= 0)
                    {
                        parameters.SetParameter(parameterNumber, primitive.Value);
                    }
                }
                else
                {
                    throw new ApplicationException("Cannot evaluate a rule until all variables have been set.");
                }
            }
        }
    }
}