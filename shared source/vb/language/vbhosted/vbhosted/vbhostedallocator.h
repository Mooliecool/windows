#pragma once

#include "VBContext.h"

class VBHostedAllocator
#if FV_DEADBEEF
    : public Deadbeef<VBHostedAllocator> // Must be last base class!
#endif
{
public:
    VBHostedAllocator();
    virtual ~VBHostedAllocator();

    NorlsAllocator* GetNorlsAllocator();
    IUnknown* AddToReleaseList(IUnknown* punk);

private:
    List<IUnknown*> m_COMReleaseList;
    NorlsAllocator m_Allocator;
};
