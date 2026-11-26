#pragma once

#include "IPlug_include_in_plug_hdr.h"

#include "../FMR_Library/FMR_Lib/FMR_DSP_float.h"
//#include "../FMR_Library/FMR_Lib/FMR_Filter.h"
#include "../FMR_Library/FMR_Pedal/IPedal.h"

#include "../FMR_Library/FMR_Pedal/PedalConst.h"

#include <atomic>
#include <memory>
#include <array>

enum BtCr_EParams
{
  BtCr_kBass = 0,
  BtCr_kMiddle,
  BtCr_kTreble,
  BtCr_kGain,
  BtCr_kType,
  BtCr_kVolume,
  BtCr_kFootSw,
  BtCr_kNumParams	
};

enum BtCr_EControlTags
{
  BtCr_kCtrlInputLED = 0,
  BtCr_kCtrlOutputLED,
  BtCr_kCtrlLED,
  BtCr_kNumLeds
};

template <typename T> class FMR_RateAdapter;
class ND_BritCrusher;

class BritCrusherDSP : public IPedal
{
public:
  struct Parameters
  {
    float bass = 0.5f;
    float treble = 0.5f;
    float middle = 0.5f;

    float gain = 0.3f;
    float type = 0.0f;
    float volume = 0.5f;

    int footSw = 1;
  };
  
  BritCrusherDSP();
  ~BritCrusherDSP();

  BritCrusherDSP(const BritCrusherDSP&) = delete;
  BritCrusherDSP& operator=(const BritCrusherDSP&) = delete;

  void Reset(double sampleRate, int blockSize);

  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, int nChans);

  int GetLatency() const;

  // IPedal (utilisé par le board : moteur seul, 48 kHz, en place)
  const char* Id() const override { return "BritCrusher"; }
  
  int NumParams() const override { return BtCr_kNumParams; }
  int NumLeds() const override { return BtCr_kNumLeds; }

  const char* ParamName(int k) const override;
  float ParamDefault(int k) const override;
  void SetParam(int k, float norm) override;
  void ResetEngine() override;
  void Process(iplug::sample** io, int nFrames, int nChans) override
  {
    ProcessEngine(io, io, nFrames, nChans, mParams);
  }

  void SetLed(int led, float color, float value, int decay);
  float GetLedColor(int led);
  float GetLedValue(int led);
  int GetLedDecay(int led);
  
  void setDisplayedLed(bool value) {displayedLed = value;}

private:
  void ProcessEngine(iplug::sample** inputs, iplug::sample** outputs, int nFrames, int nChans, const Parameters& params);
  void levelColorLed(int led, float level, float fullScale, float greenAt, float orangeAt);

  Parameters mParams;
  
  std::unique_ptr<ND_BritCrusher> mDrive;
  std::unique_ptr<FMR_RateAdapter<iplug::sample>> mRate;
  
  bool displayedLed = false;

  FilterFloat_t filterBass, filterMiddle, filterTreble;
  FilterFloat_t filterGain, filterVolume;
  float currentBass = 0.0f, currentMiddle = 0.0f, currentTreble = 0.0f;
  float currentGain = 0.0f, currentVolume = 0.0f;

  struct myLed
  {
      float color = 0.0f;
      float value = 0.0f;
      int decay = 0;
  };
  std::array<myLed, BtCr_kNumLeds> pedalLed;
};
