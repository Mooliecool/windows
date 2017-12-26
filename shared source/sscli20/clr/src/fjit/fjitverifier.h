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
#ifndef _FJITVERIFIER_H_
#define _FJITVERIFIER_H_

// This file contains declarations for the classes and macros used in the verification of IL. These mechanisms are
// used in fjit.cpp. Validity checks are via VALIDITY_CHECK(condition) macro. Invalid IL is always rejected and 
// InvalidProgramException is thrown. Verification checks are done via VERIFICATION_CHECK(condition) macro. Unverifiable IL
// is rejected if verification is turned on, code that throws VerificationException is jitted instead of the actual
// body of the unverifiable function. The definitions of valid and verifiable IL are taken from Part. III of the ECMA
// specification.

// Object for storing IL offsets in a stack
#define DEFAULT_SPLITOFFSETS_STACK_SIZE 30
class SplitOffsetStack
{
private:
  int * stack;
  int stack_size;
  int num_offsets;
public:
  /* CONSTRUCTORS / DESTRUCTORS */
  SplitOffsetStack( int size = DEFAULT_SPLITOFFSETS_STACK_SIZE );
  ~SplitOffsetStack();

  /* MUTATORS */
  void pushOffset( unsigned int offset );
  int  popOffset( );
  // Put offset at given depth of the stack 
  void putAt( unsigned int offset, int index );
  void putInFront( unsigned int offset, int index );
  // Reset the state of the stack
  void reset();

  /* ACCESSORS */
  inline int isEmpty()        { return (num_offsets == 0); }
  inline unsigned int top()   { _ASSERTE(!isEmpty()); return (unsigned int)stack[num_offsets-1]; }
  inline unsigned int getItem( int i ) { _ASSERTE(i < num_offsets); return (unsigned int)stack[i]; }
  inline int getNumberItems() { return num_offsets; }

  /* DEBUGGING METHODS */
#if defined(_DEBUG)
  void dumpStack();
#endif
};

// Macros for easy jitting of verification and validation failures
#define FJIT_FAIL(JitErrorCode)     { _ASSERTE(JitErrorCode != FJIT_INTERNALERROR || !"Internal JIT Error"); return (JitErrorCode); }

// If FJIT_NO_VALIDATION is defined fjit will jit all code, the results are underfined if the IL is invalid
#if defined(FJIT_NO_VALIDATION)
#define VALIDITY_CHECK(x)        {}   
#define VERIFICATION_CHECK(x)    {}   
#else
// VALIDITY_CHECK is used to check for invalid IL. These checks are made regardless of the status of the verifier      
#define VALIDATION_FAIL(x)       { INDEBUG(printf("VALIDATION FAILS: " #x "\n");)  \
                                   INDEBUG(printf("At %s::%s at %x\n",szDebugClassName, szDebugMethodName, InstStart );)  \
                                   FJIT_FAIL (FJIT_BADCODE); }

#define VALIDITY_CHECK(x)        { if (!(x)) VALIDATION_FAIL(x) } 

// VERIFICATION_CHECK is used to check for un-verifiable IL. These checks are only made if the verifier is turned on   
#define VERIFICATION_CHECK(x)    {  if ( JitVerify && !(x) )                     \
                                    {                                            \
                                        return verificationFailure(INDEBUG(#x)); \
                                    }                                            \
                                 }
#endif

// Macros to verify the size of the stack
#define CHECK_STACK(cnt)         { VALIDITY_CHECK(opStack_len >= cnt);}
#define CHECK_STACK_SIZE(cnt)    { VALIDITY_CHECK(opStack_len == cnt);} 
#define CHECK_POP_STACK(cnt)     { VALIDITY_CHECK(opStack_len >= cnt); popOp(cnt);}
#define POP_STACK(cnt)           { popOp(cnt);}

// Macros used to match OpType 
#define OP_MATCH_1(x,a1)            ((int)x == (int)a1)
#define OP_MATCH_2(x,a1,a2)       ( ((int)x == (int)a1) || ((int)x == (int)a2) )
#define OP_MATCH_3(x,a1,a2,a3)    ( ((int)x == (int)a1) || ((int)x == (int)a2) || ((int)x == (int)a3))
#define OP_MATCH_4(x,a1,a2,a3,a4) ( ((int)x == (int)a1) || ((int)x == (int)a2) || ((int)x == (int)a3) || ((int)x == (int)a4))
#define OP_MATCH_5(x,a1,a2,a3,a4,a5) ( ((int)x == (int)a1) || ((int)x == (int)a2) || ((int)x == (int)a3) \
                                  || ((int)x == (int)a4) || ((int)x == (int)a5) )

// The following is derived from Table 2: Binary Numeric Operations of the IL Spec
#define BINARY_NUMERIC_RESULT(type1, type2, op, result)  { CHECK_STACK(2) \
            switch (type1.enum_()) {                 \
                case typeByRef:                      \
                    VALIDITY_CHECK((type2.enum_() == typeI4 && OP_MATCH_2(op, CEE_ADD, CEE_SUB )) || \
                                   (type2.enum_() == typeI  && OP_MATCH_2(op, CEE_ADD, CEE_SUB )) || \
                                   (type2.enum_() == typeByRef && OP_MATCH_1(op,CEE_SUB))); \
                    VERIFICATION_CHECK(false);       \
                    result = typeByRef;              \
                    break;                           \
                case typeI4:                         \
                    VALIDITY_CHECK((type2.enum_() == typeI4) || ( type2.enum_() == typeI ) || \
                                   (type2.enum_() == typeByRef && OP_MATCH_2(op, CEE_ADD, CEE_SUB ))); \
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef));\
                    VERIFICATION_CHECK((type1.enum_() != typeI4 || type1.cls() == NULL) && \
                                       (type2.enum_() != typeI4 || type2.cls() == NULL) );\
                    result = type2.enum_();          \
                    break;                           \
                case typeI8:                         \
                    VALIDITY_CHECK((type2.enum_() == typeI8) || \
                                   (type2.enum_() == typeByRef && typeI == typeI8 && OP_MATCH_2(op, CEE_ADD, CEE_SUB )) ||\
                                   (type2.enum_() == typeI4 && typeI == typeI8)) ;\
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef));\
                    result = type2.enum_();          \
                    break;                           \
                case typeR8:                         \
                    VALIDITY_CHECK((type2.enum_() == typeR8));  \
                    result = typeR8;                 \
                    break;                           \
              default:                               \
                    VALIDITY_CHECK(false)            \
		    }\
}

// The following is derived from Table 3: Unary Numeric Operations of the IL Spec
#define UNARY_NUMERIC_RESULT(type1, op, result)  { CHECK_STACK(1) \
              VALIDITY_CHECK( type1.enum_() == typeI4 || type1.enum_() == typeI8 || type1.enum_() == typeR8 ); \
              VERIFICATION_CHECK( type1.cls() == NULL );\
              result = type1.enum_();                \
}

// The following is derived from Table 4: Binary Comparison or Branch Operations of the IL Spec
#define MATCH_COMP_BRANCH(op) OP_MATCH_5(op, CEE_BEQ, CEE_BNE_UN, CEE_BEQ_S, CEE_BNE_UN_S, CEE_CEQ )
#define BINARY_COMP_BRANCH(type1, type2, op)  { CHECK_STACK(2) \
            switch (type1.enum_()) {                 \
                case typeI4:                         \
                    VALIDITY_CHECK( type2.enum_() == typeI4 || type2.enum_() == typeI && typeI == typeI4 || \
                                   (type2.enum_() == typeByRef && typeI == typeI4 && \
                                    MATCH_COMP_BRANCH(op))); \
                    VERIFICATION_CHECK((type1.enum_() != typeI4 || type1.cls() == NULL) && \
                                       (type2.enum_() != typeI4 || type2.cls() == NULL) );\
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef) );\
                    break;                           \
                case typeI8:                         \
                    VALIDITY_CHECK( type2.enum_() == typeI8 || type2.enum_() == typeI && typeI == typeI8 || \
                                   (type2.enum_() == typeByRef && typeI == typeI8 && \
                                    MATCH_COMP_BRANCH(op))); \
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef) );\
                    break;                           \
                case typeR8:                         \
                    VALIDITY_CHECK((type2.enum_() == typeR8));  \
                    break;                           \
                case typeByRef:                      \
                    VALIDITY_CHECK((type2.enum_() == typeI4 && typeI == typeI4 && \
                                    MATCH_COMP_BRANCH(op)) || \
                                   (type2.enum_() == typeI8 && typeI == typeI8 && \
                                    MATCH_COMP_BRANCH(op)) || \
                                   (type2.enum_() == typeByRef )); \
                    VERIFICATION_CHECK(type2.enum_() == typeByRef );\
                    break;                           \
                case typeMethod:                     \
                case typeRef:                        \
                    VALIDITY_CHECK( ( type2.enum_() == typeRef ||  type2.enum_() == typeMethod ) && \
                                    (MATCH_COMP_BRANCH(op) || OP_MATCH_1(op, CEE_CGT_UN)) );\
                    break;                           \
              default:                               \
                       VALIDITY_CHECK(false)         \
		    }                                \
}


// The following is derived from Table 5: Integer Operations of the IL Spec
#define INTEGER_OPERATIONS(type1, type2, op)  {      \
            if ( OP_MATCH_1( op, CEE_NOT ) )         \
               CHECK_STACK(1)                        \
            else                                     \
               CHECK_STACK(2)                        \
            switch (type1.enum_()) {                 \
                case typeI4:                         \
                    VALIDITY_CHECK( type2.enum_() == typeI4 || type2.enum_() == typeI ); \
                    VERIFICATION_CHECK( type1.cls() == NULL && type2.cls() == NULL ); \
                    break;                           \
                case typeI8:                         \
                    VALIDITY_CHECK( type2.enum_() == typeI8 );\
                    break;                           \
              default:                               \
                    VALIDITY_CHECK(false)            \
		    }                                \
}
/*

         */


// The following is derived from Table 6: Shift Operations of the IL Spec
// 'type1' refers to the amount to shift by and 'type2' refers to the value
#define SHIFT_OPERATIONS(type1, type2, op)  {        \
            CHECK_STACK(2)                           \
            switch (type1.enum_()) {                 \
                case typeI4:                         \
                    VALIDITY_CHECK( type2.enum_() == typeI4 || type2.enum_() == typeI8 || type2.enum_() == typeI ); \
                    VERIFICATION_CHECK( type1.cls() == NULL && type2.cls() == NULL ); \
                    break;                           \
                case typeI8:  /* Actually TypeI*/    \
                    VALIDITY_CHECK( type1.enum_() == typeI );\
                    VALIDITY_CHECK( type2.enum_() == typeI4 || type2.enum_() == typeI8 || type2.enum_() == typeI ); \
                    break;                           \
              default:                               \
                    VALIDITY_CHECK(false)            \
		    }                                \
}

// The following is derived from Table 7: Binary Numeric Operations of the IL Spec
#define BINARY_OVERFLOW_RESULT(type1, type2, op, result)  { CHECK_STACK(2) \
            switch (type1.enum_()) {                 \
                case typeI4:                         \
                    VALIDITY_CHECK((type2.enum_() == typeI4) || ( type2.enum_() == typeI ) || \
                                   (type2.enum_() == typeByRef && OP_MATCH_1(op, CEE_ADD_OVF_UN ))); \
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef));\
                    VERIFICATION_CHECK((type1.enum_() != typeI4 || type1.cls() == NULL) && \
                                       (type2.enum_() != typeI4 || type2.cls() == NULL) );\
                    result = type2.enum_();          \
                    break;                           \
                case typeI8:                         \
                    VALIDITY_CHECK((type2.enum_() == typeI8) || \
                                   (type2.enum_() == typeByRef && typeI == typeI8 && OP_MATCH_1(op, CEE_ADD_OVF_UN )) ||\
                                   (type2.enum_() == typeI4 && typeI == typeI8)) ;\
                    VERIFICATION_CHECK(!(type2.enum_() == typeByRef));\
                    result = type2.enum_();          \
                    break;                           \
                case typeByRef:                      \
                    VALIDITY_CHECK((type2.enum_() == typeI4 && OP_MATCH_2(op, CEE_ADD_OVF_UN, CEE_SUB_OVF_UN )) || \
                                   (type2.enum_() == typeI  && OP_MATCH_2(op, CEE_ADD_OVF_UN, CEE_SUB_OVF_UN )) || \
                                   (type2.enum_() == typeByRef && OP_MATCH_1(op,CEE_SUB_OVF_UN))); \
                    VERIFICATION_CHECK(false);       \
                    result = typeByRef;              \
                    break;                           \
              default:                               \
                    VALIDITY_CHECK(false)            \
		    }                                \
}

// The following is derived from Table 8: Conversion Operations of the IL Spec
#define CONVERSION_OPERATIONS(type1, type2)  { CHECK_STACK(1) \
             VALIDITY_CHECK( type1.enum_() != typeRef &&  type1.enum_() != typeByRef || \
                             type1.enum_() == typeByRef && (type2 == CORINFO_TYPE_ULONG ||type2 == CORINFO_TYPE_LONG) || \
                             type1.enum_() == typeByRef && typeI == typeI4 && \
                              (type2 == CORINFO_TYPE_INT || type2 == CORINFO_TYPE_UINT ) || \
                             type1.enum_() == typeRef && (type2 == CORINFO_TYPE_ULONG ||type2 == CORINFO_TYPE_LONG) || \
                             type1.enum_() == typeRef && typeI == typeI4 && \
                              (type2 == CORINFO_TYPE_INT || type2 == CORINFO_TYPE_UINT ) ); \
             VERIFICATION_CHECK( type1.enum_() != typeByRef && type1.enum_() != typeRef && type1.enum_() != typeMethod );\
             VERIFICATION_CHECK((type1.enum_() != typeI4 || type1.cls() == NULL) );\
}

#define  invertBranch( op )                          \
{                                                    \
  switch ( op )                                      \
    {                                                \
      case CEE_CondBelow: op = CEE_CondAboveEq;break;\
      case CEE_CondAboveEq: op = CEE_CondBelow;break;\
      case CEE_CondEq: op = CEE_CondNotEq;break;     \
      case CEE_CondNotEq: op = CEE_CondEq;break;     \
      case CEE_CondBelowEq: op = CEE_CondAbove;break;\
      case CEE_CondAbove: op = CEE_CondBelowEq;break;\
      case CEE_CondLt: op = CEE_CondGtEq;break;      \
      case CEE_CondGtEq: op = CEE_CondLt;break;      \
      case CEE_CondLtEq: op = CEE_CondGt;break;      \
      case CEE_CondGt: op = CEE_CondLtEq;break;      \
      case CEE_CondAlways: op = CEE_CondAlways;break;\
      default:                                       \
           FJIT_FAIL(FJIT_INTERNALERROR);            \
    }                                                \
}

inline void makeClauseEmpty( CORINFO_EH_CLAUSE * clause )
{
  clause->HandlerOffset = 0;  
  clause->HandlerLength = 0;
  clause->TryOffset     = 0;
  clause->TryLength     = 0;                 
}

inline int sameClauses( CORINFO_EH_CLAUSE * clause1, CORINFO_EH_CLAUSE * clause2 )
{
   return ( clause1->HandlerOffset == clause2->HandlerOffset  &&  
            clause1->HandlerLength == clause2->HandlerLength  &&
            clause1->TryOffset     == clause2->TryOffset  &&
            clause1->TryLength     == clause2->TryLength &&
            clause1->FilterOffset  == clause2->FilterOffset );             
}
 
inline int isClauseEmpty( CORINFO_EH_CLAUSE * clause )
{
  return (  clause->HandlerOffset == 0 &&  
            clause->HandlerLength == 0 &&
            clause->TryOffset     == 0 &&
            clause->TryLength     == 0 );
}


inline int canAssign( ICorJitInfo* info, CORINFO_METHOD_HANDLE context, OpType a, OpType b, bool noCasts = false )  // a - child,  b - parent
{
  // Simple case 
  if ( a == b)
    return true;

  // Any byref is assignment compatible with a readonly byref, provided their targets are equivalent
  if (a.isByRef() && b.isReadOnly())
    return (a.getTarget() == b.getTarget());

  // Check for null
  if ( a.isNull() && b.isRef() || a.isRef() && b.isNull() )
      return true;

  // Check if this are two objects that inherit from one another
  if ( a.isRef() && b.isRef()  && a.cls() != 0 && b.cls() != 0 && !noCasts )
  {
      if (info->canCast(a.cls(), b.cls()))
	return true;
      return false;
  }
    
  // Check if it is necessary to normalize the types 
  if ( ( a.isValClass() && !a.isRefAny() ) ^ ( b.isValClass() && !b.isRefAny() )&& !a.isMethod() && !b.isMethod() )
  {
    OpType t1 = b, t2 = a; 
    if ( a.isValClass() && !a.isRefAny() )
    {
          CorInfoType eeType = info->asCorInfoType(a.cls());
          t2 = OpType(eeType);
          bool isPrimValClass = (info->getClassAttribs(a.cls(), context) & CORINFO_FLG_VALUECLASS) ? true : false;
          // Filter out primitive value types for which we don't want to track type information
          isPrimValClass = isPrimValClass && ( CORINFO_TYPE_UNDEF == info->getTypeForPrimitiveValueClass( a.cls() ) ) ? true : false;
          if ( !t2.isPrimitive() || t2.isRef() || t2.isByRef() || (t2.enum_() == typeI4 && isPrimValClass) )
	    t2.setHandle( a.cls() ); 
    }
    if ( b.isValClass() && !b.isRefAny() )
    {
         CorInfoType eeType = info->asCorInfoType(b.cls());
         t1 = OpType(eeType);
         bool isPrimValClass = (info->getClassAttribs(b.cls(), context) & CORINFO_FLG_VALUECLASS) ? true : false;
         // Filter out primitive value types for which we don't want to track type information
	 isPrimValClass = isPrimValClass && ( CORINFO_TYPE_UNDEF == info->getTypeForPrimitiveValueClass( b.cls() ) ) ? true : false;
         if ( !t1.isPrimitive() || t1.isRef() || t1.isByRef() || (t1.enum_() == typeI4 && isPrimValClass) )
	    t1.setHandle( b.cls() );
         t1.toFPNormalizedType();
    }
 
    // Simple case 
   if ( t1 == t2)
      return true;

   // Check if for null (we allow any value to be assigned to null but this will immediately cause
   // an access violation)
   if ( t2.isRef() && t1.isNull() || t1.isRef() && t2.isNull() )
     return true;

   // Check if this are two objects that inherit from one another
   if ( t1.isRef() && t2.isRef() && !noCasts )
   {
      return info->canCast(t2.cls(), t1.cls());
   }

   if ( t1.isPrimitive() && t2.isPrimitive() )
     switch ( t1.enum_() ) {
        case typeU1: if ( t2.enum_() == typeI1 ) return true;  break;
        case typeU2: if ( t2.enum_() == typeI2 ) return true;  break;
        case typeI1: if ( t2.enum_() == typeU1 ) return true;  break;
        case typeI2: if ( t2.enum_() == typeU2 ) return true;  break;
        default: break;
   }
 
   INDEBUG(printf( "TypeA [%d, %d] TypeB [%d, %d] T1 [ %d, %d] T2[%d, %d] \n", a.enum_(), a.cls(), b.enum_(), b.cls(), 
		   t1.enum_(), t1.cls(), t2.enum_(), t2.cls() );)
  }
  
  // This is necessary to match the behavior of V1 .NET Framework which doesn't differentiate between 
  // signed/unsigned elements
  if ( a.isByRef() && b.isByRef() && a.isTargetPrimitive() && b.isTargetPrimitive() )
    switch ( a.targetAsEnum() ) {
        case typeU1: if ( b.targetAsEnum() == typeI1 ) return true;  break;
        case typeU2: if ( b.targetAsEnum() == typeI2 ) return true;  break;
        case typeI1: if ( b.targetAsEnum() == typeU1 ) return true;  break;
        case typeI2: if ( b.targetAsEnum() == typeU2 ) return true;  break;
        default: break;
    }
       
  
  INDEBUG(printf( "TypeA [%d, %d] TypeB [%d, %d] \n", a.enum_(), a.cls(), b.enum_(), b.cls());)

  // The types must be different
  return false;
}

#endif //_FJITVERIFIER_H_
