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

namespace Microsoft.JScript{

  using Microsoft.JScript.Vsa;
  using System;
  using System.Runtime.InteropServices;
  using System.Runtime.Serialization;
  using System.Resources;
  using System.Security.Permissions;
  using System.Globalization;
  using Microsoft.Vsa;
  using System.Text;

  //-------------------------------------------------------------------------------------------------------
  // JScriptException
  //
  //  An error in JScript goes to a CLR host/program in the form of a JScriptException. However a 
  //  JScriptException is not always thrown. In fact a JScriptException is also a IVsaError and thus it can be
  //  passed to the host through IVsaSite.OnCompilerError(IVsaError error).
  //  When a JScriptException is not a wrapper for some other object (usually either a COM+ exception or 
  //  any value thrown in a JScript throw statement) it takes a JSError value.
  //  The JSError enum is defined in JSError.cs. When introducing a new type of error perform
  //  the following four steps:
  //  1- Add the error in the JSError enum (JSError.cs)
  //  2- Update GetErrorType() to return a value from the ErrorType enum (in ErrorConstructor) for the 
  //     error you just defined. GetErrorType() is a map form JSError to a JScript object in the Error
  //     hierarchy
  //  3- Update Microsoft.JScript.txt with the US English error message
  //  4- Update Severity.
  //-------------------------------------------------------------------------------------------------------
  [Serializable] public class JScriptException : ApplicationException, IVsaFullErrorInfo{
    internal Object value;
    [NonSerialized] internal Context context;
    internal bool isError;
    internal static readonly String ContextStringDelimiter = ";;";
    private int code; // This is same as base.HResult. We have this so that the debuugger can get the
                      // error code without doing a func-eval ( to evaluate the HResult property )
    
    // We don't expect this to be called usually, but add it in case of serialization.  
    public JScriptException()
      :this(JSError.NoError){
    }
    
    public JScriptException(string m)
      :this(m, null){
    }
    
    public JScriptException(string m, Exception e) 
      :this(m, e, null){
    }
    
    public JScriptException(JSError errorNumber)
      :this(errorNumber, null){
    }
    
    internal JScriptException(JSError errorNumber, Context context){
      this.value = Missing.Value;
      this.context = context;
      this.code = this.HResult = unchecked((int)(0x800A0000 + (int)errorNumber));
    }

    internal JScriptException(Object value, Context context){
      this.value = value;
      this.context = context;
      this.code = this.HResult = unchecked((int)(0x800A0000 + (int)JSError.UncaughtException));
    }

    internal JScriptException(Exception e, Context context) : this(null, e, context){
    }
    
    internal JScriptException(string m, Exception e, Context context) : base(m, e){
      this.value = e;
      this.context = context;
      if (e is StackOverflowException){
        this.code = this.HResult = unchecked((int)(0x800A0000 + (int)JSError.OutOfStack));
        this.value = Missing.Value;
      }else if (e is OutOfMemoryException){
        this.code = this.HResult = unchecked((int)(0x800A0000 + (int)JSError.OutOfMemory));
        this.value = Missing.Value;
      }
      else {
        int hr = System.Runtime.InteropServices.Marshal.GetHRForException(e);
        if ((hr & 0xffff0000) == 0x800A0000 && System.Enum.IsDefined(typeof(JSError), hr & 0xffff)){
          this.code = this.HResult = hr;
          this.value = Missing.Value;
        }else
          this.code = this.HResult = unchecked((int)(0x800A0000 + (int)JSError.UncaughtException));
      }
    }

    protected JScriptException(SerializationInfo info, StreamingContext context)
      : base(info, context){
      this.code = this.HResult = info.GetInt32("Code");
      this.value = Missing.Value;
      this.isError = info.GetBoolean("IsError");
    }

    public String SourceMoniker{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.context != null)
          return this.context.document.documentName;
        else
          return "no source";  
      }
    }

    public int StartColumn{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        return this.Column;
      }
    }

    public int Column{
      get{
        if (this.context != null)
          return this.context.StartColumn + this.context.document.startCol + 1;
        else
          return 0;
      }
    }

    // We have this method simply to have the same link demand on it as the interface
    // method has and avoid a FXCOP warning. The Description property itself cannot have
    // a link demand on it since it is used by the JScript Error object's Description property
    string IVsaError.Description{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        return this.Description;
      }
    }

    public string Description{
      get{
        return this.Message;
      }
    }

    public int EndLine{
      get{
        if (this.context != null)
          return this.context.EndLine + this.context.document.startLine - this.context.document.lastLineInSource;
        else
          return 0;
      }
    }

    public int EndColumn{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.context != null)
          return this.context.EndColumn + this.context.document.startCol + 1;
        else
          return 0;
      }
    }

    // We have this method simply to have the same link demand on it as the interface
    // method has and avoid a FXCOP warning. The Number property itself cannot have
    // a link demand on it since it is used by the JScript Error object's Number property
    int IVsaError.Number{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        return this.Number;
      }
    }

    public int Number{
      get{
        return this.ErrorNumber;
      }
    }

    public int ErrorNumber{ 
      get{
        return this.HResult;
      }
    }

    internal ErrorType GetErrorType(){
      int ec = this.HResult;
      if ((ec & 0xFFFF0000) != 0x800A0000) return ErrorType.OtherError;
      switch ((JSError)(ec & 0xFFFF)){
        case JSError.AbstractWithBody : return ErrorType.SyntaxError;
        case JSError.AmbiguousConstructorCall : return ErrorType.ReferenceError;
        case JSError.AmbiguousMatch : return ErrorType.ReferenceError;
        case JSError.ArrayLengthConstructIncorrect : return ErrorType.RangeError;
        case JSError.ArrayLengthAssignIncorrect : return ErrorType.RangeError;
        case JSError.AssemblyAttributesMustBeGlobal : return ErrorType.SyntaxError;
        case JSError.AssignmentToReadOnly : return ErrorType.ReferenceError;
        case JSError.BadBreak : return ErrorType.SyntaxError;
        case JSError.BadContinue : return ErrorType.SyntaxError;
        case JSError.BadHexDigit : return ErrorType.SyntaxError;
        case JSError.BadLabel : return ErrorType.SyntaxError;
        case JSError.BadReturn : return ErrorType.SyntaxError;
        case JSError.BadSwitch : return ErrorType.SyntaxError;
        case JSError.BadFunctionDeclaration : return ErrorType.SyntaxError;
        case JSError.BadPropertyDeclaration : return ErrorType.SyntaxError;
        case JSError.BadVariableDeclaration : return ErrorType.SyntaxError;
        case JSError.BooleanExpected : return ErrorType.TypeError;
        case JSError.CannotInstantiateAbstractClass : return ErrorType.TypeError; 
        case JSError.CannotNestPositionDirective : return ErrorType.SyntaxError;
        case JSError.CannotReturnValueFromVoidFunction : return ErrorType.TypeError;
        case JSError.CantAssignThis : return ErrorType.ReferenceError;
        case JSError.CcInvalidElif : return ErrorType.SyntaxError;
        case JSError.CcInvalidElse : return ErrorType.SyntaxError;
        case JSError.CcInvalidEnd : return ErrorType.SyntaxError;
        case JSError.CcOff : return ErrorType.SyntaxError;
        case JSError.CircularDefinition : return ErrorType.SyntaxError;
        case JSError.ClashWithProperty : return ErrorType.SyntaxError;
        case JSError.ClassNotAllowed : return ErrorType.SyntaxError;
        case JSError.ConstructorMayNotHaveReturnType : return ErrorType.SyntaxError;
        case JSError.DateExpected : return ErrorType.TypeError;
        case JSError.DifferentReturnTypeFromBase : return ErrorType.TypeError;
        case JSError.DoesNotHaveAnAddress : return ErrorType.ReferenceError;
        case JSError.DupDefault : return ErrorType.SyntaxError;
        case JSError.DuplicateMethod : return ErrorType.TypeError;
        case JSError.DuplicateNamedParameter : return ErrorType.ReferenceError;
        case JSError.EnumeratorExpected : return ErrorType.TypeError;
        case JSError.ErrEOF : return ErrorType.SyntaxError;
        case JSError.ExpectedAssembly: return ErrorType.SyntaxError;
        case JSError.ExpressionExpected : return ErrorType.SyntaxError;
        case JSError.FractionOutOfRange : return ErrorType.RangeError;
        case JSError.FunctionExpected : return ErrorType.TypeError;
        case JSError.IllegalAssignment : return ErrorType.ReferenceError;
        case JSError.IllegalChar : return ErrorType.SyntaxError;
        case JSError.IllegalEval : return ErrorType.EvalError;
        case JSError.ImpossibleConversion : return ErrorType.TypeError;
        case JSError.InstanceNotAccessibleFromStatic : return ErrorType.ReferenceError;
        case JSError.InvalidBaseTypeForEnum : return ErrorType.TypeError;
        case JSError.InvalidCall : return ErrorType.TypeError; 
        case JSError.InvalidCustomAttribute : return ErrorType.TypeError;
        case JSError.InvalidCustomAttributeArgument : return ErrorType.TypeError;
        case JSError.InvalidCustomAttributeClassOrCtor : return ErrorType.TypeError;
        case JSError.InvalidDebugDirective: return ErrorType.SyntaxError;
        case JSError.InvalidElse : return ErrorType.SyntaxError;
        case JSError.InvalidPositionDirective: return ErrorType.SyntaxError;
        case JSError.InvalidPrototype : return ErrorType.TypeError;
        case JSError.ItemNotAllowedOnExpandoClass : return ErrorType.SyntaxError;
        case JSError.KeywordUsedAsIdentifier : return ErrorType.SyntaxError;
        case JSError.MemberInitializerCannotContainFuncExpr : return ErrorType.SyntaxError;
        case JSError.MissingConstructForAttributes : return ErrorType.SyntaxError;
        case JSError.MissingNameParameter : return ErrorType.ReferenceError;
        case JSError.MoreNamedParametersThanArguments : return ErrorType.ReferenceError;
        case JSError.MustBeEOL : return ErrorType.SyntaxError;
        case JSError.MustProvideNameForNamedParameter : return ErrorType.ReferenceError;
        case JSError.IncorrectNumberOfIndices : return ErrorType.ReferenceError;
        case JSError.NeedArrayObject : return ErrorType.TypeError;
        case JSError.NeedCompileTimeConstant : return ErrorType.ReferenceError; 
        case JSError.NeedInterface : return ErrorType.TypeError;
        case JSError.NeedInstance : return ErrorType.ReferenceError;
        case JSError.NeedType : return ErrorType.TypeError;
        case JSError.NestedInstanceTypeCannotBeExtendedByStatic : return ErrorType.ReferenceError;
        case JSError.NoAt : return ErrorType.SyntaxError;
        case JSError.NoCatch : return ErrorType.SyntaxError;
        case JSError.NoCcEnd : return ErrorType.SyntaxError;
        case JSError.NoColon : return ErrorType.SyntaxError;
        case JSError.NoComma : return ErrorType.SyntaxError;
        case JSError.NoCommaOrTypeDefinitionError : return ErrorType.SyntaxError;
        case JSError.NoCommentEnd : return ErrorType.SyntaxError;
        case JSError.NoConstructor : return ErrorType.TypeError;
        case JSError.NoEqual : return ErrorType.SyntaxError;
        case JSError.NoIdentifier : return ErrorType.SyntaxError;
        case JSError.NoLabel : return ErrorType.SyntaxError;
        case JSError.NoLeftParen : return ErrorType.SyntaxError;
        case JSError.NoLeftCurly : return ErrorType.SyntaxError;
        case JSError.NoMemberIdentifier : return ErrorType.SyntaxError;
        case JSError.NonStaticWithTypeName : return ErrorType.ReferenceError;
        case JSError.NoRightBracket : return ErrorType.SyntaxError;
        case JSError.NoRightBracketOrComma : return ErrorType.SyntaxError;
        case JSError.NoRightCurly : return ErrorType.SyntaxError;
        case JSError.NoRightParen : return ErrorType.SyntaxError;
        case JSError.NoRightParenOrComma : return ErrorType.SyntaxError;
        case JSError.NoSemicolon : return ErrorType.SyntaxError;
        case JSError.NoSuchMember : return ErrorType.ReferenceError;
        case JSError.NoSuchStaticMember : return ErrorType.ReferenceError;
        case JSError.NotIndexable : return ErrorType.TypeError;
        case JSError.NotAccessible : return ErrorType.ReferenceError;
        case JSError.NotAnExpandoFunction : return ErrorType.ReferenceError;
        case JSError.NotCollection : return ErrorType.TypeError;
        case JSError.NotConst : return ErrorType.SyntaxError;
        case JSError.NotInsideClass : return ErrorType.SyntaxError;
        case JSError.NoWhile : return ErrorType.SyntaxError;
        case JSError.NumberExpected : return ErrorType.TypeError;
        case JSError.ObjectExpected : return ErrorType.TypeError;
        case JSError.OLENoPropOrMethod : return ErrorType.TypeError;
        case JSError.OnlyClassesAllowed : return ErrorType.SyntaxError;
        case JSError.OnlyClassesAndPackagesAllowed : return ErrorType.SyntaxError;
        case JSError.PackageExpected : return ErrorType.SyntaxError; 
        case JSError.ParamListNotLast : return ErrorType.SyntaxError;
        case JSError.PrecisionOutOfRange : return ErrorType.RangeError;
        case JSError.PropertyLevelAttributesMustBeOnGetter : return ErrorType.ReferenceError;
        case JSError.RegExpExpected : return ErrorType.TypeError;
        case JSError.RegExpSyntax : return ErrorType.SyntaxError;
        case JSError.ShouldBeAbstract : return ErrorType.SyntaxError;
        case JSError.StaticMissingInStaticInit : return ErrorType.SyntaxError;
        case JSError.StaticRequiresTypeName : return ErrorType.ReferenceError;
        case JSError.StringExpected : return ErrorType.TypeError;
        case JSError.SuperClassConstructorNotAccessible : return ErrorType.ReferenceError;
        case JSError.SyntaxError : return ErrorType.SyntaxError;
        case JSError.TooFewParameters : return ErrorType.TypeError;
        case JSError.TooManyTokensSkipped : return ErrorType.SyntaxError;
        case JSError.TypeCannotBeExtended : return ErrorType.ReferenceError;
        case JSError.TypeMismatch : return ErrorType.TypeError;
        case JSError.UndeclaredVariable : return ErrorType.ReferenceError;
        case JSError.UndefinedIdentifier : return ErrorType.ReferenceError;
        case JSError.UnexpectedSemicolon : return ErrorType.SyntaxError;
        case JSError.UnreachableCatch : return ErrorType.SyntaxError;
        case JSError.UnterminatedString : return ErrorType.SyntaxError;
        case JSError.URIEncodeError : return ErrorType.URIError;
        case JSError.URIDecodeError : return ErrorType.URIError;
        case JSError.VBArrayExpected : return ErrorType.TypeError;
        case JSError.WriteOnlyProperty : return ErrorType.ReferenceError;
        case JSError.WrongDirective : return ErrorType.SyntaxError;
        case JSError.BadModifierInInterface : return ErrorType.SyntaxError;
        case JSError.VarIllegalInInterface : return ErrorType.SyntaxError;
        case JSError.InterfaceIllegalInInterface : return ErrorType.SyntaxError;
        case JSError.NoVarInEnum : return ErrorType.SyntaxError;
        case JSError.EnumNotAllowed: return ErrorType.SyntaxError;
        case JSError.PackageInWrongContext: return ErrorType.SyntaxError;
        case JSError.CcInvalidInDebugger: return ErrorType.SyntaxError;
        case JSError.TypeNameTooLong: return ErrorType.SyntaxError;
      }
      return ErrorType.OtherError;
    }

    [SecurityPermission(SecurityAction.Demand, SerializationFormatter=true)]
    public override void GetObjectData(SerializationInfo info, StreamingContext context) {
      if (info == null) throw new ArgumentNullException("info");
      base.GetObjectData(info, context);
      info.AddValue("IsError", this.isError);
      info.AddValue("Code", this.code);
    }

    public int Line{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.context != null)
          return this.context.StartLine + this.context.document.startLine - this.context.document.lastLineInSource;
        else
          return 0;
      }
    }

    public String LineText{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.context != null)
          return this.context.source_string;
        else
          return "";
      }
    }

    internal static String Localize(String key, CultureInfo culture){
      return JScriptException.Localize(key, null, culture);
    }

    internal static String Localize(String key, String context, CultureInfo culture){
      try{
        ResourceManager rm = new ResourceManager("Microsoft.JScript", typeof(JScriptException).Module.Assembly);
        String localizedString = rm.GetString(key, culture);
        if (localizedString != null){
          // locate context/no-context string delimiter
          int splitAt = localizedString.IndexOf(ContextStringDelimiter);
          if (splitAt == -1){
            // there is no context-specific string
            return localizedString;
          }else if (context == null){
            // splitAt is one character past the end of the no-context string
            return localizedString.Substring(0, splitAt);
          }else{
            // splitAt is two characters before the beginning of the context string
            return String.Format(culture, localizedString.Substring(splitAt+2), context);
          }
        }
      }catch(MissingManifestResourceException){
      }
      return key;
    }

    public override String Message{
      get{
        if (this.value is Exception){
          Exception e = (Exception)this.value;
          String result = e.Message;
          if (result != null && result.Length > 0)
            return result;
          else{
            return e.ToString();
          }
        }
        String code = (this.HResult & 0xFFFF).ToString(CultureInfo.InvariantCulture);
        CultureInfo culture = null;
        if (this.context != null && this.context.document != null){
          VsaEngine engine = this.context.document.engine;
          if (engine != null)
            culture = engine.ErrorCultureInfo;
        }
        if (this.value is ErrorObject){
          String result = ((ErrorObject)this.value).Message;
          if (result != null && result.Length > 0)
            return result;
          else
            return Localize("No description available", culture) + ": " + code;
        }else if (this.value is String){
          switch (((JSError)(this.HResult & 0xFFFF))){
            case JSError.CannotBeAbstract :
            case JSError.Deprecated:
            case JSError.DifferentReturnTypeFromBase : 
            case JSError.DuplicateName : 
            case JSError.HidesAbstractInBase:
            case JSError.HidesParentMember :
            case JSError.InvalidCustomAttributeTarget :
            case JSError.ImplicitlyReferencedAssemblyNotFound :
            case JSError.MustImplementMethod : 
            case JSError.NoSuchMember : 
            case JSError.NoSuchType :
            case JSError.NoSuchStaticMember :
            case JSError.NotIndexable :
            case JSError.TypeCannotBeExtended :
            case JSError.TypeMismatch :
            case JSError.InvalidResource:
            case JSError.IncompatibleAssemblyReference:
            case JSError.InvalidAssemblyKeyFile:
            case JSError.TypeNameTooLong:
              return Localize(code, (String)this.value, culture);
            default : return (String)this.value;
          }
        }
        // special case some errors with contextual information
        if (this.context != null){
          switch (((JSError)(this.HResult & 0xFFFF))){
            case JSError.AmbiguousBindingBecauseOfWith :
            case JSError.AmbiguousBindingBecauseOfEval :
            case JSError.AssignmentToReadOnly :
            case JSError.DuplicateName : 
            case JSError.InstanceNotAccessibleFromStatic :
            case JSError.KeywordUsedAsIdentifier :
            case JSError.NeedInstance :
            case JSError.NonStaticWithTypeName :
            case JSError.NotAccessible : 
            case JSError.NotDeletable :
            case JSError.NotMeantToBeCalledDirectly :
            case JSError.ObjectExpected :
            case JSError.StaticRequiresTypeName :
            case JSError.UndeclaredVariable : 
            case JSError.UndefinedIdentifier :
            case JSError.VariableLeftUninitialized :
            case JSError.VariableMightBeUnitialized :
              return Localize(code, this.context.GetCode(), culture);
            }
          }
        return Localize(((int)(this.HResult & 0xFFFF)).ToString(CultureInfo.InvariantCulture), culture);
      }
    }   

    public int Severity{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        //guide: 0 == there will be a run-time error if this code executes
        //       1 == the programmer probably did not intend to do this
        //       2 == this can lead to problems in the future.
        //       3 == this can lead to performance problems
        //       4 == this is just not right.
        int ec = this.HResult;
        if ((ec & 0xFFFF0000) != 0x800A0000) return 0;
        if (!this.isError){
          switch ((JSError)(ec & 0xFFFF)){
            case JSError.AmbiguousBindingBecauseOfWith : return 4;
            case JSError.AmbiguousBindingBecauseOfEval : return 4;
            case JSError.ArrayMayBeCopied : return 1;
            case JSError.AssignmentToReadOnly : return 1;
            case JSError.BadOctalLiteral : return 1;
            case JSError.BadWayToLeaveFinally : return 3;
            case JSError.BaseClassIsExpandoAlready : return 1;
            case JSError.Deprecated : return 2;
            case JSError.DifferentReturnTypeFromBase : return 1;
            case JSError.DuplicateName : return 1;
            case JSError.DupVisibility : return 1;
            case JSError.GetAndSetAreInconsistent : return 1;
            case JSError.HidesParentMember : return 1; 
            case JSError.IncompatibleVisibility : return 1;
            case JSError.KeywordUsedAsIdentifier : return 2;
            case JSError.NewNotSpecifiedInMethodDeclaration : return 1;
            case JSError.NotDeletable : return 1;
            case JSError.NotMeantToBeCalledDirectly : return 1;
            case JSError.OctalLiteralsAreDeprecated : return 2;
            case JSError.PossibleBadConversion : return 1;
            case JSError.PossibleBadConversionFromString : return 4;
            case JSError.ShouldBeAbstract : return 1;
            case JSError.StringConcatIsSlow : return 3;
            case JSError.SuspectAssignment : return 1;
            case JSError.SuspectLoopCondition : return 1;
            case JSError.SuspectSemicolon : return 1;
            case JSError.TooFewParameters : return 1;
            case JSError.TooManyParameters : return 1;
            case JSError.UndeclaredVariable : return 3;
            case JSError.UselessAssignment : return 1;
            case JSError.UselessExpression : return 1;
            case JSError.VariableLeftUninitialized : return 3;
            case JSError.VariableMightBeUnitialized : return 3;
            case JSError.IncompatibleAssemblyReference : return 1;
          }
        }
        return 0;
      }
    }

    public IVsaItem SourceItem{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.context != null)
          return this.context.document.sourceItem;
        else
          throw new NoContextException();
        
      }
    }
    
    public override String StackTrace{
      get{
        if (this.context == null)
          return this.Message+Environment.NewLine+base.StackTrace;
        StringBuilder result = new StringBuilder();
        Context c = this.context;

        // Append the "filename: line number" error string
        String fileName = c.document.documentName;
        if (fileName != null && fileName.Length > 0)
          result.Append(fileName+": ");
        CultureInfo culture = null;
        if (this.context != null && this.context.document != null){
          VsaEngine engine = this.context.document.engine;
          if (engine != null)
            culture = engine.ErrorCultureInfo;
        }
        result.Append(Localize("Line", culture));
        result.Append(' ');
        result.Append(c.StartLine);
        // report the error
        result.Append(" - ");
        result.Append(Localize("Error", culture));
        result.Append(": ");
        result.Append(this.Message);
        result.Append(Environment.NewLine);
         
        if (c.document.engine != null){   
          // Append the stack trace
          Stack callContextStack = c.document.engine.Globals.CallContextStack;
          for (int i = 0, n = callContextStack.Size(); i < n; i++){
            CallContext cctx = (CallContext)callContextStack.Peek(i);
            result.Append("    ");
            result.Append(Localize("at call to", culture));
            result.Append(cctx.FunctionName());
            result.Append(' ');
            result.Append(Localize("in line", culture));
            result.Append(": ");
            result.Append(cctx.sourceContext.EndLine);
          }
        }
        return result.ToString();
      }
    }
      
  }

  [Serializable]
  public class NoContextException : ApplicationException{
    public NoContextException() : base(JScriptException.Localize("No Source Context available", CultureInfo.CurrentUICulture)){
    }
    
    public NoContextException(string m) : base(m) {
    }
    
    public NoContextException(string m, Exception e) : base(m, e) {
    }
    
    protected NoContextException(SerializationInfo s, StreamingContext c) : base(s, c) {
    }
  } 

}
