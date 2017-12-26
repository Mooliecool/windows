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
// File: stage.h
//
// Contains a list of all the stages in the compiler
// ===========================================================================

#ifndef DEFINE_STAGE
#error Must define DEFINE_STAGE macro before including stage.h
#endif

DEFINE_STAGE(BEGIN)             // catchall
DEFINE_STAGE(PARSE)             // CSourceData::ParseTopLevel
DEFINE_STAGE(DECLARE)           // CLSDREC::declareXXX
DEFINE_STAGE(IMPORT)            // IMPORTER
DEFINE_STAGE(DEFINE)            // CLSDREC::defineXXX
DEFINE_STAGE(PREPARE)           // CLSDREC::prepareXXX
DEFINE_STAGE(EMIT)              // EMITTER & CLSDREC::emitXXX
DEFINE_STAGE(INTERIORPARSE)     // CSourceData::ParseInteriorNode
DEFINE_STAGE(COMPILE)           // CLSDREC::compileXXX
DEFINE_STAGE(BIND)              // FUNCBREC::bindXXX
DEFINE_STAGE(SCAN)              // FUNCBREC::postBindCompile
DEFINE_STAGE(TRANSFORM)         // FUNCBREC::rewriteXXX
DEFINE_STAGE(CODEGEN)           // ILGEN
DEFINE_STAGE(EMITIL)            // ILGEN::copyCode
