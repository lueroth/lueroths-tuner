//------------------------------------------------------------------------
// Copyright(c) 2026 lueroth.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace yin {
//------------------------------------------------------------------------
static const Steinberg::FUID kgenTestProcessorUID (0x3086F695, 0x728B5E0F, 0x9A4CAA04, 0xC3E10EF7);
static const Steinberg::FUID kgenTestControllerUID (0x377B9405, 0xEC11518F, 0xB450D6C5, 0xBAC77402);

    enum MyParameterIDs {
        kCentsOffset = 67
    };

#define genTestVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace yin
