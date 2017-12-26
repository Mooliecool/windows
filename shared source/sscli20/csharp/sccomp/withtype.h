/* ==++==
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
// ===========================================================================
// File: withtype.h

    Defines structs that package an aggregate member together with
    generic type argument information.

    This file is included twice:
    1) Before symbol.h with WITHTYPE_INLINES _not_ defined
    2) After symbol.h with WITHTYPE_INLINES defined

// ===========================================================================
*/

/******************************************************************************
    SymWithType and its cousins. These package an aggregate member (field,
    prop, event, or meth) together with the particular instantiation of the
    aggregate (the AGGTYPESYM).

    The default constructor does nothing so these are not safe to use
    uninitialized. Note that when they are used as member of an EXPR they
    are automatically zero filled by newExpr.
******************************************************************************/
#ifndef WITHTYPE_INLINES
    struct SymWithType
    {
        SYM * sym;
        AGGTYPESYM * ats;

        SymWithType() { }
        SymWithType(SYM * sym, AGGTYPESYM * ats) { Set(sym, ats); }

        void Clear() { sym = NULL; ats = NULL; }
        void Set(SYM * sym, AGGTYPESYM * ats);

        SYM * Sym() const { return sym; }
        AGGTYPESYM * Type() const { return ats; }

        bool operator ==(const SymWithType & swt) const { return sym == swt.sym && ats == swt.ats; }
        bool operator !=(const SymWithType & swt) const { return sym != swt.sym || ats != swt.ats; }

        // The SymWithType is considered NULL iff the SYM is NULL.
        operator bool() const { return sym != NULL; }

        // These assert that the SYM is of the correct type.
        METHPROPSYM * MethProp() const ;
        METHSYM * Meth() const ;
        PROPSYM * Prop() const ;
        MEMBVARSYM * Field() const ;
        EVENTSYM * Event() const ;
    };
#else // !WITHTYPE_INLINES
    __forceinline void SymWithType::Set(SYM * sym, AGGTYPESYM * ats)
    {
        if (!sym)
            ats = NULL;
        ASSERT(!ats || sym->parent == ats->getAggregate());
        this->sym = sym;
        this->ats = ats;
    }
    __forceinline METHPROPSYM * SymWithType::MethProp() const { return this->sym->asMETHPROPSYM(); }
    __forceinline METHSYM * SymWithType::Meth() const { return this->sym->asMETHSYM(); }
    __forceinline PROPSYM * SymWithType::Prop() const { return this->sym->asPROPSYM(); }
    __forceinline MEMBVARSYM * SymWithType::Field() const { return this->sym->asMEMBVARSYM(); }
    __forceinline EVENTSYM * SymWithType::Event() const { return this->sym->asEVENTSYM(); }
#endif // !WITHTYPE_INLINES


/******************************************************************************
    In debug these types assert that the symbol is of the correct type.
    In non-debug they are just SymWithType.
******************************************************************************/
#ifndef DEBUG
    typedef SymWithType MethPropWithType;
    typedef SymWithType MethWithType;
    typedef SymWithType PropWithType;
    typedef SymWithType EventWithType;
    typedef SymWithType FieldWithType;
#elif !defined(WITHTYPE_INLINES) // && DEBUG
    struct MethPropWithType : public SymWithType
    {
        MethPropWithType() { }
        MethPropWithType(METHPROPSYM * mps, AGGTYPESYM * ats) { Set(mps, ats); }
        MethPropWithType(SymWithType & sym);
        void Set(METHPROPSYM * mps, AGGTYPESYM * ats);
    };

    struct MethWithType : public MethPropWithType
    {
        MethWithType() { }
        MethWithType(METHSYM * meth, AGGTYPESYM * ats) { Set(meth, ats); }
        MethWithType(SymWithType & sym);
        void Set(METHSYM * meth, AGGTYPESYM * ats);
    };

    struct PropWithType : public MethPropWithType
    {
        PropWithType() { }
        PropWithType(PROPSYM * prop, AGGTYPESYM * ats) { Set(prop, ats); }
        PropWithType(SymWithType & sym);
        void Set(PROPSYM * prop, AGGTYPESYM * ats);
    };

    struct EventWithType : public SymWithType
    {
        EventWithType() { }
        EventWithType(EVENTSYM * event, AGGTYPESYM * ats) { Set(event, ats); }
        EventWithType(SymWithType & sym);
        void Set(EVENTSYM * event, AGGTYPESYM * ats);
    };

    struct FieldWithType : public SymWithType
    {
        FieldWithType() { }
        FieldWithType(MEMBVARSYM * field, AGGTYPESYM * ats) { Set(field, ats); }
        FieldWithType(SymWithType & sym);
        void Set(MEMBVARSYM * field, AGGTYPESYM * ats);
    };
#else // DEBUG && WITHTYPE_INLINES
    inline void MethPropWithType::Set(METHPROPSYM * mps, AGGTYPESYM * ats) { SymWithType::Set(mps, ats); }
    inline MethPropWithType::MethPropWithType(SymWithType & swt) { Set(swt.sym->asMETHPROPSYM(), swt.ats); }

    inline void MethWithType::Set(METHSYM * meth, AGGTYPESYM * ats) { SymWithType::Set(meth, ats); }
    inline MethWithType::MethWithType(SymWithType & swt) { Set(swt.sym->asMETHSYM(), swt.ats); }

    inline void PropWithType::Set(PROPSYM * prop, AGGTYPESYM * ats) { SymWithType::Set(prop, ats); }
    inline PropWithType::PropWithType(SymWithType & swt) { Set(swt.sym->asPROPSYM(), swt.ats); }

    inline void EventWithType::Set(EVENTSYM * event, AGGTYPESYM * ats) { SymWithType::Set(event, ats); }
    inline EventWithType::EventWithType(SymWithType & swt) { Set(swt.sym->asEVENTSYM(), swt.ats); }

    inline void FieldWithType::Set(MEMBVARSYM * field, AGGTYPESYM * ats) { SymWithType::Set(field, ats); }
    inline FieldWithType::FieldWithType(SymWithType & swt) { Set(swt.sym->asMEMBVARSYM(), swt.ats); }
#endif // DEBUG && WITHTYPE_INLINES


/******************************************************************************
    MethPropWithInst and MethWithInst. These extend MethPropWithType with
    the method type arguments. Properties will never have type args, but
    methods and properties share a lot of code so it's convenient to allow
    both here.

    The default constructor does nothing so these are not safe to use
    uninitialized. Note that when they are used as member of an EXPR they
    are automatically zero filled by newExpr.
******************************************************************************/
#ifndef WITHTYPE_INLINES
    struct MethPropWithInst : public MethPropWithType
    {
        TypeArray * typeArgs;

        MethPropWithInst() { }
        MethPropWithInst(METHPROPSYM * mps, AGGTYPESYM * ats, TypeArray * typeArgs = NULL) { Set(mps, ats, typeArgs); }

        void Clear() { sym = NULL; ats = NULL; typeArgs = NULL; }
        void Set(METHPROPSYM * sym, AGGTYPESYM * ats, TypeArray * typeArgs);

        TypeArray * TypeArgs() const { return typeArgs; }

        bool operator ==(const MethPropWithInst & mpwi) const { return sym == mpwi.sym && ats == mpwi.ats && typeArgs == mpwi.typeArgs; }
        bool operator !=(const MethPropWithInst & mpwi) const { return sym != mpwi.sym || ats != mpwi.ats || typeArgs != mpwi.typeArgs; }
    };
#else // !WITHTYPE_INLINES
    __forceinline void MethPropWithInst::Set(METHPROPSYM * mps, AGGTYPESYM * ats, TypeArray * typeArgs)
    {
        if (!mps) {
            ats = NULL;
            typeArgs = NULL;
        }
        ASSERT(!ats || mps && mps->getClass() == ats->getAggregate());
        ASSERT(!typeArgs || !typeArgs->size || mps && mps->isMETHSYM());
        ASSERT(!typeArgs || !mps->isMETHSYM() || mps->asMETHSYM()->typeVars->size == typeArgs->size);
        this->sym = mps;
        this->ats = ats;
        this->typeArgs = typeArgs;
    }
#endif // !WITHTYPE_INLINES


/******************************************************************************
    In debug this type asserts that the symbol is a METHSYM.
    In retail it is just MethPropWithInst.
******************************************************************************/
#ifndef DEBUG
    typedef MethPropWithInst MethWithInst;
#elif !defined(WITHTYPE_INLINES) // && DEBUG
    struct MethWithInst : public MethPropWithInst
    {
        MethWithInst() { }
        MethWithInst(METHSYM * meth, AGGTYPESYM * ats, TypeArray * typeArgs = NULL) { Set(meth, ats, typeArgs); }
        MethWithInst(MethPropWithInst & mpwi);
        void Set(METHSYM * meth, AGGTYPESYM * ats, TypeArray * typeArgs);
    };
#else // DEBUG && WITHTYPE_INLINES
    inline void MethWithInst::Set(METHSYM * meth, AGGTYPESYM * ats, TypeArray * typeArgs) { MethPropWithInst::Set(meth, ats, typeArgs); }
    inline MethWithInst::MethWithInst(MethPropWithInst & mpwi) { Set(mpwi.sym->asMETHSYM(), mpwi.ats, mpwi.typeArgs); }
#endif // DEBUG && WITHTYPE_INLINES

#undef WITHTYPE_INLINES
