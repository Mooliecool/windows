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

//***************************************************************************************
// JSToken
//
// Items in this enumeration are arranged so to maximize performance in the parser.
// Values in switches are consecutive and operators are together to allow indexing in array
// and range check. Please, try to be consistent when adding new values or changing the
// Parser, also see the static functions in JSScanner that deal with JSTokens
//***************************************************************************************

namespace Microsoft.JScript{

    public enum JSToken : int{
      None = -1,
      EndOfFile,

      // main statement switch
      If,
      For,
      Do,
      While,
      Continue,
      Break,
      Return,
      Import,
      With,
      Switch,
      Throw,
      Try,
      Package,
      Internal,
      Abstract,
      Public,
      Static,
      Private,
      Protected,
      Final,
      Event,
      Var,
      Const,
      Class,

      // used by both statement and expression switches
      Function,
      LeftCurly,                      // {
      Semicolon,                      // ;

      // main expression switch
      Null,
      True,
      False,
      This,
      Identifier,
      StringLiteral,
      IntegerLiteral,
      NumericLiteral,

      LeftParen,                      // (
      LeftBracket,                    // [
      AccessField,                    // .

      // operators
      FirstOp,
      // unary ops
      LogicalNot = FirstOp,           // !
      BitwiseNot,                     // ~
      Delete,
      Void,
      Typeof,
      Increment,                      // ++
      Decrement,                      // --
      FirstBinaryOp,
      // binary ops
      Plus = FirstBinaryOp,           // +
      Minus,                          // -
      LogicalOr,                      // ||
      LogicalAnd,                     // &&
      BitwiseOr,                      // |
      BitwiseXor,                     // ^
      BitwiseAnd,                     // &
      Equal,                          // ==
      NotEqual,                       // !=
      StrictEqual,                    // ===
      StrictNotEqual,                 // !==
      GreaterThan,                    // >
      LessThan,                       // <
      LessThanEqual,                  // <=
      GreaterThanEqual,               // >=
      LeftShift,                      // <<
      RightShift,                     // >>
      UnsignedRightShift,             // >>>
      Multiply,                       // *
      Divide,                         // /
      Modulo,                         // %
      LastPPOperator = Modulo,
      Instanceof,
      In,
      Assign,                         // =
      PlusAssign,                     // +=
      MinusAssign,                    // -=
      MultiplyAssign,                 // *=
      DivideAssign,                   // /=
      BitwiseAndAssign,               // &=
      BitwiseOrAssign,                // |=
      BitwiseXorAssign,               // ^=
      ModuloAssign,                   // %=
      LeftShiftAssign,                // >>=
      RightShiftAssign,               // <<=
      UnsignedRightShiftAssign,       // <<<=
      LastAssign = UnsignedRightShiftAssign,
      LastBinaryOp = UnsignedRightShiftAssign,
      ConditionalIf,                  // ? // MUST FOLLOW LastBinaryOp
      Colon,                          // :
      Comma,                          // ,
      LastOp = Comma,

      // context specific keywords
      Case,
      Catch,
      Debugger,
      Default,
      Else,
      Export,
      Extends,
      Finally,
      Get,
      Implements,
      Interface,
      New,
      Set,
      Super,
      RightParen,                     // )
      RightCurly,                     // }
      RightBracket,                   // ]
      PreProcessorConstant,           // entity defined defined during preprocessing
      Comment,                        // for authoring
      UnterminatedComment,            // for authoring
      // js5, js8 and ECMA reserved words
      Assert,
      Boolean,
      Byte,
      Char,
      Decimal,
      Double,
      DoubleColon,                      // ::
      Enum,
      Ensure,
      Float,
      Goto,
      Int,
      Invariant,
      Long,
      Namespace,
      Native,
      Require,
      Sbyte,
      Short,
      Synchronized,
      Transient,
      Throws,
      ParamArray,                     // ...
      Volatile,
      Ushort,
      Uint,
      Ulong,
      Use,

      EndOfLine, // this is here only because of error recovery, in reality this token is never produced
      PreProcessDirective, 
    }
}