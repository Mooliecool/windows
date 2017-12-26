//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Standard macros for declaring C++ enums.
//
//  The purpose of these macros is to reduce global namespace pollution with enum value names
//  and provide type-safe operations on enum values.
//
//  Example of simple enum declaration using the macros:
//
//    DECLARE_ENUM(CompilationState)
//        NoState,      // We have no compiler-generated state for this item.
//        Declared,     // Symbols have been created.
//        Bound,        // Symbols have been linked together.
//        TypesEmitted, // Type metadata has been emitted.
//        Compiled,     // Method bodies have been compiled, code is on disk.
//        MAX
//    END_ENUM(CompilationState)
//
//    CompilationStateEnum currentState = CompilationState::Bound;
//
//  Example of flags enum declaration using the macros:
//
//    DECLARE_ENUM(Specifiers)
//        FLAG_VALUE(Private),
//        FLAG_VALUE(Protected),
//        FLAG_VALUE(Friend),
//        ProtectedFriend = Protected | Friend,
//        FLAG_VALUE(Public),
//        Accesses = Private | Protected | Friend | ProtectedFriend | Public
//    END_ENUM(Specifiers)
//
//    DECLARE_ENUM_OPERATORS(Specifiers)
//
//    SpecifiersEnum accessModifiers = Specifiers::Public | Specifiers::Private;
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Starts enum declaration. Use END_ENUM to finish the declaration.
#define DECLARE_ENUM(name)                                                \
    struct name                                                           \
    {                                                                     \
    private:                                                              \
        static const int __startLine = __LINE__ + 1;                      \
                                                                          \
        name() {}                                                         \
                                                                          \
    public:                                                               \
        enum _Enum                                                        \
        {

// Declares an unique bit-flags enum value.
#define FLAG_VALUE(name) name = (1 << (__LINE__ - __startLine))

// In debug builds enum operators are strongly-typed and operate on enums.
// In retail builds enum values are 'unsigned int' and operators are not needed.
#if DEBUG

// Finishes enum declaration started with DECLARE_ENUM.
#define END_ENUM(name)                                                    \
        };                                                                \
    };                                                                    \
    typedef name::_Enum name##Enum;

// Declares type-safe bitwise operations on enums.
#define DECLARE_ENUM_OPERATORS(name)                                      \
    inline                                                                \
    name::_Enum operator |(_In_ name::_Enum e1, _In_ name::_Enum e2)      \
    {                                                                     \
       return name::_Enum((unsigned int)e1 | (unsigned int)e2);           \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum operator &(_In_ name::_Enum e1, _In_ name::_Enum e2)      \
    {                                                                     \
       return name::_Enum((unsigned int)e1 & (unsigned int)e2);           \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum operator ~(_In_ name::_Enum e)                            \
    {                                                                     \
       return name::_Enum(~(int)e);                                       \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum& operator |=(_In_ name::_Enum& lhs, _In_ name::_Enum rhs) \
    {                                                                     \
       lhs = lhs | rhs;                                                   \
       return lhs;                                                        \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum& operator &=(_In_ name::_Enum& lhs, _In_ name::_Enum rhs) \
    {                                                                     \
       lhs = lhs & rhs;                                                   \
       return lhs;                                                        \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum operator <<(_In_ name::_Enum e, _In_ name::_Enum shift)   \
    {                                                                     \
       return name::_Enum((int)e << (int)shift);                          \
    }                                                                     \
                                                                          \
    inline                                                                \
    name::_Enum operator >>(_In_ name::_Enum e, _In_ name::_Enum shift)   \
    {                                                                     \
        return name::_Enum((int)e >> (int)shift);                         \
    }

#else // !DEBUG

// Finishes enum declaration started with DECLARE_ENUM.
#define END_ENUM(name)                                                    \
        };                                                                \
    };                                                                    \
    typedef unsigned int name##Enum;

// Declares type-safe bitwise operations on enums.
#define DECLARE_ENUM_OPERATORS(name)    ;

#endif // DEBUG                                                           

#define HASFLAG(FlagsToSearchThrough, FlagToSearchFor)                    \
       ( (((unsigned int)(FlagsToSearchThrough)) & ((unsigned int)(FlagToSearchFor))) != 0)
