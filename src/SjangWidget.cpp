#include "Sjang.h"

void SjangScopeWidget::drawWaveform(NVGcontext* ctx, DynStochOscillator& osc) {
  float x = 0.f;
  float y = osc.breakpoints.front().a;
  y = (.5f - .5f * y) * box.size.y;
  nvgBeginPath(ctx);
  nvgMoveTo(ctx, x, y);
  for (auto bp : osc.breakpoints) {
    x = bp.t;
    if (x > 2.f) {
      y = (bp.a * (bp.t - 2.f) + bp.a * (2.f - bp.t)) / (bp.t - bp.t);
      y = (.5f - .5f * y) * box.size.y;
      nvgLineTo(ctx, box.size.x, y);
      break;
    }
    x *= .5f * box.size.x;
    y = bp.a;
    y = (.5f - .5f * y) * box.size.y;
    nvgLineTo(ctx, x, y);
  }
  nvgStroke(ctx);
}

void SjangScopeWidget::drawLayer(const DrawArgs& args, int layer) {
  if (!module)
    return;

  if (layer == 1) {
    nvgStrokeWidth(args.vg, 1.f);
    nvgLineCap(args.vg, NVG_ROUND);
    nvgLineJoin(args.vg, NVG_ROUND);

    for (int c = module->channels - 1; c >= 0; c--) {
      if (module->outputs[Sjang::R_OUTPUT].isConnected()) {
        nvgStrokeColor(args.vg, nvgRGBf(1.f, .5f, .5f));
        drawWaveform(args.vg, module->osc[c + 16]);
      }

      nvgStrokeColor(args.vg, nvgRGBf(1.f, 1.f, .75f));
      drawWaveform(args.vg, module->osc[c]);
    }
  }

  Widget::drawLayer(args, layer);
}

SjangWidget::SjangWidget(Sjang* module) {
  setModule(module);
  setPanel(createPanel(asset::plugin(pluginInstance, "res/Sjang.svg")));

  addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
  addChild(createWidget<ScrewBlack>(Vec(
    box.size.x - 2 * RACK_GRID_WIDTH, 0)));
  addChild(createWidget<ScrewBlack>(Vec(
    RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
  addChild(createWidget<ScrewBlack>(Vec(
    box.size.x - 2 * RACK_GRID_WIDTH,
    RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

  addParam(createParamCentered<RoundLargeBlackKnob>(
    mm2px(Vec(20.32, 35)), module, Sjang::PITCH_PARAM));
  addParam(createParamCentered<RoundSmallBlackKnob>(
    mm2px(Vec(6.773333333333333, 40)), module, Sjang::DESTRUCT_T_PARAM));
  addParam(createParamCentered<RoundSmallBlackKnob>(
    mm2px(Vec(33.86666666666667, 40)), module, Sjang::DESTRUCT_A_PARAM));
  addParam(createParamCentered<RoundLargeBlackKnob>(
    mm2px(Vec(10.16, 60)), module, Sjang::RANDOM_T_PARAM));
  addParam(createParamCentered<RoundLargeBlackKnob>(
    mm2px(Vec(30.48, 60)), module, Sjang::RANDOM_A_PARAM));
  addParam(createParamCentered<Trimpot>(
    mm2px(Vec(6.773333333333333, 80)), module, Sjang::RANDOM_T_ATT_PARAM));
  addParam(createParamCentered<Trimpot>(
    mm2px(Vec(20.32, 80)), module, Sjang::SEGMENTS_PARAM));
  addParam(createParamCentered<Trimpot>(
    mm2px(Vec(33.86666666666667, 80)), module, Sjang::RANDOM_A_ATT_PARAM));

  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(5.08, 92)), module, Sjang::DESTRUCT_T_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(15.24, 92)), module, Sjang::RANDOM_T_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(25.4, 92)), module, Sjang::RANDOM_A_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(35.56, 92)), module, Sjang::DESTRUCT_A_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(10.16, 103)), module, Sjang::RESET_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(20.32, 103)), module, Sjang::RANDOMIZE_INPUT));
  addInput(createInputCentered<DarkPJ301MPort>(
    mm2px(Vec(30.48, 103)), module, Sjang::V_OCT_INPUT));
  addParam(createParamCentered<VCVButton>(
    mm2px(Vec(5.08, 114)), module, Sjang::RESET_PARAM));
  addParam(createParamCentered<VCVButton>(
    mm2px(Vec(15.24, 114)), module, Sjang::RANDOMIZE_PARAM));
  addOutput(createOutputCentered<DarkPJ301MPort>(
    mm2px(Vec(25.4, 114)), module, Sjang::L_OUTPUT));
  addOutput(createOutputCentered<DarkPJ301MPort>(
    mm2px(Vec(35.56, 114)), module, Sjang::R_OUTPUT));

  SjangScopeWidget* scopeWidget =
    createWidget<SjangScopeWidget>(mm2px(Vec(1, 14)));
  scopeWidget->setSize(mm2px(Vec(38.64, 11)));
  scopeWidget->module = module;
  addChild(scopeWidget);
}

void SjangWidget::appendContextMenu(Menu* menu) {
  Sjang* module = getModule<Sjang>();

  menu->addChild(new MenuSeparator);

  menu->addChild(createIndexPtrSubmenuItem(
    "Quantize pitch knob",
    { "Continuous",
      "Semitones",
      "Octaves" },
    &module->pitchQuant));

  menu->addChild(createIndexPtrSubmenuItem(
    "Reference wave form",
    { "Sine",
      "Zero" },
    &module->refWaveform));
}
