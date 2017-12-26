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

#ifndef __DECODEMD_H__
#define __DECODEMD_H__

// --------------------------------------------------------
// This is used to decode a bitstream encoding

class Decoder
{
public:
    Decoder();
    Decoder(BYTE* bytes);
    void Init(BYTE* bytes);
    unsigned Next();
    signed NextSigned();
    BYTE *End();
    
    // --------------------------------------------------------
    // This structures contains the state of the FSM

    struct Decode
    {
        const BYTE* decoded;    //the already decoded values
        unsigned  next;   //what to do when no more decoded values
    };

private:
    // --------------------------------------------------------
    // This is used to access nibbles from a byte stream.

    class Nibbles
    {
        friend class Decoder;
    public:
        void SetContents(BYTE* bytes);
        BYTE Next();
        BYTE Read();
        unsigned Bits(unsigned number);
    private:
        PTR_BYTE data;
        BYTE nibbles[2];
        unsigned next;
    };

    Decode state;
    Nibbles data;
};

// --------------------------------------------------------
// This is used to encode a bitstream encoding

class Encoder
{
public:
    Encoder(BYTE *buffer);
    void ContainsNegatives(BOOL b);
    void EncodeSigned(signed value);
    void Encode(unsigned value);
    void Encode(signed value, BOOL isSigned);
    void Add(unsigned value, unsigned length);
    void Add64(unsigned __int64 value, unsigned length);
    void Done();
    unsigned Contents(BYTE** contents);
    unsigned Length();
private:
    BYTE* buffer;
    BYTE encoding;
    unsigned unusedBits;
    BOOL done;
    BOOL signedNumbers;
    unsigned index;
};

#endif // __DECODEMD_H__
