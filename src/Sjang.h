// Sjang
// rational funcion oscillator module
// for VCV Rack
// Author: Matthias Sars
// http://www.matthiassars.eu
// https://github.com/matthiassars/vanTies

#pragma once
#include <iostream>
#include <cmath>
#include "rack.hpp"
#include "vanTies.h"
#include "dsp/DynStochOscillator.h"

struct Sjang : Module {
  enum ParamId {
    PITCH_PARAM,
    RANDOM_T_PARAM,
    RANDOM_A_PARAM,
    DESTRUCT_T_PARAM,
    DESTRUCT_A_PARAM,
    SEGMENTS_PARAM,
    RANDOM_T_ATT_PARAM,
    RANDOM_A_ATT_PARAM,
    RESET_PARAM,
    RANDOMIZE_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    V_OCT_INPUT,
    RANDOM_T_INPUT,
    RANDOM_A_INPUT,
    DESTRUCT_T_INPUT,
    DESTRUCT_A_INPUT,
    RESET_INPUT,
    RANDOMIZE_INPUT,
    INPUTS_LEN
  };
  enum OutputId {
    L_OUTPUT,
    R_OUTPUT,
    OUTPUTS_LEN
  };
  enum LightId {
    LIGHTS_LEN
  };

  Sjang();

  json_t* dataToJson() override;
  void dataFromJson(json_t* rootJ) override;
  void onSampleRateChange(const SampleRateChangeEvent& e) override;
  void onReset(const ResetEvent& e) override;
  void onRandomize(const RandomizeEvent& e) override;
  void reset(bool randomize);
  void process(const ProcessArgs& args) override;

  // 32 of them: 16 polyphony channels, each of them stereo
  DynStochOscillator osc[32];

  enum PitchQuant {
    CONTINUOUS,
    SEMITONES,
    OCTAVES
  };

  int channels = 0;
  PitchQuant pitchQuant = CONTINUOUS;
  DynStochOscillator::RefWaveform refWaveform = DynStochOscillator::SINE;

  dsp::SchmittTrigger resetTrigger[16];
  dsp::SchmittTrigger randomizeTrigger[16];
};

struct SjangScopeWidget : Widget {
  Sjang* module;

  void drawWaveform(NVGcontext* ctx, DynStochOscillator& osc);

  void drawLayer(const DrawArgs& args, int layer) override;
};

struct SjangWidget : ModuleWidget {
  SjangWidget(Sjang* module);

  void appendContextMenu(Menu* menu) override;
};

