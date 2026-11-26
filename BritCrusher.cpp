
#include "BritCrusher.h"

#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include "BritCrusherGUI.h"
#include "../FMR_Library/FMR_NeuralDrive/FMR_BritCrusher.h"
#include "../FMR_Library/FMR_Lib/FMR_ConvPot.h"

#include <algorithm>

// utiliser struct Parameters et BritCrusherDSP::SetParam pour ne pas dupliquer les infos ?
BritCrusher::BritCrusher(const InstanceInfo& info) : iplug::Plugin(info, MakeConfig(BtCr_kNumParams, kNumPresets))
{
  GetParam(BtCr_kBass)->InitDouble("Bass", 0.5, 0.001, 0.999, 0.001);
  GetParam(BtCr_kMiddle)->InitDouble("Treble", 0.5, 0.001, 0.999, 0.001);
  GetParam(BtCr_kTreble)->InitDouble("Treble", 0.5, 0.001, 0.999, 0.001);

  GetParam(BtCr_kGain)->InitDouble("Gain", 0.3, 0.1, 0.999, 0.001);
  GetParam(BtCr_kType)->InitInt("Type", 0, 0, 2);
  GetParam(BtCr_kVolume)->InitInt("Volume", 2048, 0, 4095);

  GetParam(BtCr_kFootSw)->InitInt("Foot Switch", 1, 0, 1);
  
#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]()
  {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics)
  {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    BritCrusherGUI::Build(pGraphics, 0.f, 0.f, 0, true);
	mDSP.setDisplayedLed(true);
  };
#endif
}

void BritCrusher::OnIdle()
{
#if IPLUG_EDITOR
  if (!GetUI()) return;
  
  for(int cptLed = 0; cptLed < BtCr_kNumLeds ; cptLed++ )
  {
    if ( auto* led = dynamic_cast<ILEDControl*>( GetUI()->GetControlWithTag( kNumStartLED + cptLed) ) )
	{
      float value = GetBypassed() ? 0.0f : mDSP.GetLedValue(cptLed);
      led->SetValue(value);
      if (value != 0.0f)
      {
          led->TriggerWithDecay(mDSP.GetLedDecay(cptLed));
      }
      led->SetHue(mDSP.GetLedColor(cptLed));
      led->SetDirty(true);
    }
  }

#endif
}

#if IPLUG_DSP

void BritCrusher::OnReset()
{
  mDSP.Reset(GetSampleRate(), std::max(GetBlockSize(), 64));
  SetLatency(mDSP.GetLatency());
}

void BritCrusher::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = std::min(NOutChansConnected(),ND_BRIT_CRUSHER_NB_CHANNELS);

  // Conversion unique : BritCrusherDSP::SetParam (partagée avec le board).
  // lecture de tous les knobs et conversion (mise à l'échelle)
  for (int k = 0; k < mDSP.NumParams(); k++)
    mDSP.SetParam(k, static_cast<float>(GetParam(k)->GetNormalized()));

  mDSP.ProcessBlock(inputs, outputs, nFrames, nChans);  
}

#endif
