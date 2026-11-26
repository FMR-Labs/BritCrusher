#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "BritCrusherDSP.h"

using namespace iplug;
using namespace igraphics;

const int kNumPresets = 1;

class BritCrusher final : public Plugin
{
public:
  BritCrusher(const InstanceInfo& info);
  ~BritCrusher() override = default;

  void OnIdle() override;

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;

private:
  BritCrusherDSP mDSP;
#endif
};
