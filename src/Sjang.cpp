#include "Sjang.h"

using namespace std;
using namespace dsp;

Sjang::Sjang() {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

  configParam(PITCH_PARAM, 0.f, 6.f, 4.f, "pitch");
  configParam(RANDOM_T_PARAM, 0.f, 1.f, 0.f, "time randomness");
  configParam(RANDOM_A_PARAM, 0.f, 1.f, 0.f, "amp randomness");
  configParam(DESTRUCT_T_PARAM, 0.f, 1.f, 0.f, "destruct pitch");
  configParam(DESTRUCT_A_PARAM, 0.f, 1.f, 1.f, "destruct amp");
  configParam(SEGMENTS_PARAM, 3.f, 64.f, 16.f, "number of segments");
  configParam(RANDOM_T_ATT_PARAM, -1.f, 1.f, 0.f, "time randomness modulation");
  configParam(RANDOM_A_ATT_PARAM, -1.f, 1.f, 0.f, "amp randomness modulation");
  configButton(RESET_PARAM, "reset");
  configButton(RANDOMIZE_PARAM, "randomize");

  getParamQuantity(SEGMENTS_PARAM)->snapEnabled = true;
  getParamQuantity(PITCH_PARAM)->randomizeEnabled = false;

  configInput(V_OCT_INPUT, "V/oct");
  configInput(RANDOM_T_INPUT, "time randomness");
  configInput(RANDOM_A_INPUT, "amp randomness");
  configInput(DESTRUCT_T_INPUT, "destruct pitch");
  configInput(DESTRUCT_A_INPUT, "destruct shape");
  configInput(RESET_INPUT, "reset");
  configInput(RANDOMIZE_INPUT, "randomize");

  configOutput(L_OUTPUT, "left");
  configOutput(R_OUTPUT, "right");

  reset(false);
}

json_t* Sjang::dataToJson() {
  json_t* rootJ = json_object();
  json_object_set_new(rootJ, "pitchQuant", json_integer(pitchQuant));
  json_object_set_new(rootJ, "refWaveform", json_integer(refWaveform));
  return rootJ;
}

void Sjang::dataFromJson(json_t* rootJ) {
  json_t* pitchQuantJ = json_object_get(rootJ, "pitchQuant");
  if (pitchQuantJ)
    pitchQuant = (PitchQuant)json_integer_value(pitchQuantJ);
  json_t* refWaveformJ = json_object_get(rootJ, "refWaveform");
  if (refWaveformJ)
    refWaveform = (DynStochOscillator::RefWaveform)json_integer_value(refWaveformJ);
}

void Sjang::onSampleRateChange(const SampleRateChangeEvent& e) {
  Module::onSampleRateChange(e);
  for (int c = 0; c < 32; c++)
    osc[c].setSampleRate(APP->engine->getSampleRate());
}

void Sjang::onReset(const ResetEvent& e) {
  Module::onReset(e);
  refWaveform = DynStochOscillator::SINE;
  reset(false);
}

void Sjang::onRandomize(const RandomizeEvent& e) {
  Module::onRandomize(e);
  reset(true);
}

void Sjang::reset(bool randomize) {
  int segments = round(params[SEGMENTS_PARAM].getValue());
  for (int i = 0; i < 32; i++) {
    osc[i].setRefWaveform(refWaveform);
    osc[i].setSegments(segments);
    osc[i].reset(randomize);
  }
}

void Sjang::process(const ProcessArgs& args) {
  channels = max(inputs[V_OCT_INPUT].getChannels(), 1);
  outputs[L_OUTPUT].setChannels(channels);
  outputs[R_OUTPUT].setChannels(channels);

  for (int c = 0; c < channels; c++) {
    float pitch = params[PITCH_PARAM].getValue();
    if (pitchQuant == OCTAVES)
      pitch = round(pitch);
    else if (pitchQuant == SEMITONES)
      pitch = round(12.f * pitch) / 12.f;
    pitch += inputs[V_OCT_INPUT].getPolyVoltage(c);
    pitch = 16.35159783128741466737f * exp2f(pitch);

    int segments = round(params[SEGMENTS_PARAM].getValue());

    float randAmt_t = params[RANDOM_T_PARAM].getValue();
    float randAmt_a = params[RANDOM_A_PARAM].getValue();
    float destruct_t = params[DESTRUCT_T_PARAM].getValue();
    float destruct_a = params[DESTRUCT_A_PARAM].getValue();

    randAmt_t += .1f * inputs[RANDOM_T_INPUT].getPolyVoltage(c)
      * params[RANDOM_T_ATT_PARAM].getValue();
    randAmt_a += .1f * inputs[RANDOM_A_INPUT].getPolyVoltage(c)
      * params[RANDOM_A_ATT_PARAM].getValue();
    destruct_t += .1f * inputs[DESTRUCT_T_INPUT].getPolyVoltage(c);
    destruct_a += .1f * inputs[DESTRUCT_A_INPUT].getPolyVoltage(c);

    randAmt_t = (pow(100.f, randAmt_t) - 1.f) / 99.f;
    randAmt_a = (pow(100.f, randAmt_a) - 1.f) / 99.f;

    bool resetTr = resetTrigger[c].process(
      5.f * params[RESET_PARAM].getValue() + inputs[RESET_INPUT].getPolyVoltage(c),
      0.1f, 2.f);
    bool randomizeTr = randomizeTrigger[c].process(
      5.f * params[RANDOMIZE_PARAM].getValue() + inputs[RANDOMIZE_INPUT].getPolyVoltage(c),
      0.1f, 2.f);

    for (int i = c; i < ((outputs[R_OUTPUT].isConnected()) ? 32 : 16); i += 16) {
      if (resetTr) {
        osc[i].reset(false);
        osc[i].setSegments(segments);
      }
      if (randomizeTr) {
        osc[i].reset(true);
        osc[i].setSegments(segments);
      }

      osc[i].setFreq(pitch);
      if (APP->engine->getFrame() % 256 == 0)
        osc[i].setSegments(segments);
      osc[i].setRandAmt_t(randAmt_t);
      osc[i].setRandAmt_a(randAmt_a);
      osc[i].setDestruct_t(destruct_t);
      osc[i].setDestruct_a(destruct_a);
      osc[i].setRefWaveform(refWaveform);

      osc[i].process();
    }

    outputs[L_OUTPUT].setVoltage(5.f * osc[c].getWave(), c);
    outputs[R_OUTPUT].setVoltage(5.f * osc[c + 16].getWave(), c);
  }
}

Model* modelSjang = createModel<Sjang, SjangWidget>("Sjang");
