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
// ===========================================================================
// File: fileiter.h
//
// Defines various iterators for C# files
// ===========================================================================

#ifndef __fileiter_h__
#define __fileiter_h__

/*
 * Iterators for OUTFILESYMs
 */
class OutFileIteratorBase
{
public:

    virtual OUTFILESYM *Reset(COMPILER * compiler)
    {
        current = compiler->GetFirstOutFile();
        AdvanceToValid();
        return Current();
    }

    virtual OUTFILESYM *Next()
    {
        current = current->nextOutfile();
        AdvanceToValid();
        return Current();
    }

            OUTFILESYM *Current() { return current; }

protected:
    void AdvanceToValid()
    {
        if (current && !IsValid()) {
            Next();
        }
    }

    virtual bool IsValid() = 0;

    OutFileIteratorBase() { current = 0; }

    OUTFILESYM *current;
};

class OutFileIterator : public OutFileIteratorBase
{
protected:
    virtual bool IsValid() { return true; }
};

class SourceOutFileIterator : public OutFileIteratorBase
{
protected:
    bool IsValid()
    {
        return (!current->isResource && current->name && *current->name->text);
    }
};

/*
 * Iterators for INFILESYMs
 */

class IInfileIterator
{
public:
    virtual INFILESYM *Next() = 0;
    virtual INFILESYM *Current() = 0;
};

class InFileIteratorBase : public IInfileIterator
{
public:

    INFILESYM *Reset(OUTFILESYM *outfile)
    {
        current = outfile->firstInfile();
        AdvanceToValid();
        return Current();
    }

    INFILESYM *Next()
    {
        current = current->nextInfile();
        AdvanceToValid();
        return Current();
    }

    INFILESYM *Current()
    {
        return current;
    }

protected:
    void AdvanceToValid()
    {
        if (current && !IsValid()) {
            Next();
        }
    }

    virtual bool IsValid() = 0;

    InFileIteratorBase() { current = 0; }

    INFILESYM *current;
};

class InFileIterator : public InFileIteratorBase
{
protected:
    bool IsValid() { return true; }
};

/*
 * Combined Out/In iterator
 */
class CombinedFileIterator : public IInfileIterator
{
public:

    CombinedFileIterator(OutFileIteratorBase *   outIterator,
                         InFileIteratorBase *    inIterator) :
        outIterator(outIterator),
        inIterator(inIterator)
    {}

    INFILESYM *Reset(COMPILER *compiler)
    {
        outIterator->Reset(compiler);
        inIterator->Reset(outIterator->Current());
        return Current();
    }

    INFILESYM *Next()
    {
        inIterator->Next();
        if (!inIterator->Current()) {
            outIterator->Next();
            if (outIterator->Current()) {
                inIterator->Reset(outIterator->Current());
            }
        }
        return Current();
    }

    INFILESYM *Current()
    {
        return inIterator->Current();
    }

private:

    OutFileIteratorBase *   outIterator;
    InFileIteratorBase *    inIterator;

};

class SourceFileIterator : public CombinedFileIterator
{
public:
    SourceFileIterator() :
        CombinedFileIterator(&outIterator, &inIterator)
    {}

private:
    SourceOutFileIterator outIterator;
    InFileIterator inIterator;
};

class AllInFileIterator : public CombinedFileIterator
{
public:
    AllInFileIterator() :
        CombinedFileIterator(&outIterator, &inIterator)
    {}

private:
    OutFileIterator outIterator;
    InFileIterator inIterator;
};


// Iterator for AGGSYMs
class AggIterator
{
public:
    AGGSYM *Reset(INFILESYM *infile)
    {
        ASSERT(infile->rootDeclaration);
        aggCur = GetFirstInListNsDecl(infile->rootDeclaration->firstChild);
        return aggCur;
    }

    AGGSYM *Next()
    {
        aggCur = GetNext(aggCur);
        return aggCur;
    }

    AGGSYM *Current()
    {
        return aggCur;
    }

protected:
    AGGSYM * aggCur;

    static AGGSYM * GetFirstInListNsDecl(SYM * sym)
    {
        // Only use this for child lists in an NSDECL.
        ASSERT(!sym || sym->parent->isNSDECLSYM());

        AGGSYM * agg;

        for ( ; sym; sym = sym->nextChild) {
            switch (sym->getKind()) {
            case SK_AGGDECLSYM:
                if (sym->asAGGDECLSYM()->IsFirst())
                    return sym->asAGGDECLSYM()->Agg();
                break;
            case SK_NSDECLSYM:
                agg = GetFirstInListNsDecl(sym->asNSDECLSYM()->firstChild);
                if (agg)
                    return agg;
                break;
            case SK_GLOBALATTRSYM:
                break;
            default:
                ASSERT(!"Bad SK");
                break;
            }
        }

        return NULL;
    }

    static AGGSYM * GetFirstInListAgg(SYM * sym)
    {
        // Only use this for child lists in an AGG.
        ASSERT(!sym || sym->parent->isAGGSYM());

        for ( ; sym; sym = sym->nextChild) {
            if (sym->isAGGSYM())
                return sym->asAGGSYM();
        }

        return NULL;
    }

    static AGGSYM * GetNext(AGGSYM * agg)
    {
        if (!agg)
            return NULL;

        // Children first. We process nested AGGs with the outer AGG.
        AGGSYM * aggNext = GetFirstInListAgg(agg->firstChild);
        if (aggNext)
            return aggNext;

        // Check siblings. If none found move up a level. Once agg's parent is a NS
        // the processing is different (following this loop).
        for ( ; agg->Parent()->isAGGSYM(); agg = agg->Parent()->asAGGSYM()) {
            aggNext = GetFirstInListAgg(agg->nextChild);
            if (aggNext)
                return aggNext;
        }

        // Agg's parent is a NS. Switch to searching DECLs.
        ASSERT(agg && agg->Parent()->isNSSYM());
        for (DECLSYM * decl = agg->DeclFirst(); decl; decl = decl->DeclPar()) {
            aggNext = GetFirstInListNsDecl(decl->nextChild);
            if (aggNext)
                return aggNext;
        }

        return NULL;
    }
};

#endif // __fileiter_h__

