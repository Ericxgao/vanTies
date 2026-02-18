#include "RatFuncOscillator.h"

using namespace std;

float RatFuncOscillator::phaseDistort1_1(float x, float c1) {
  float c2 = c1 * c1;
  float c3 = c2 * c1;

  return (2.f * (c3 - 2.f * c2 + c1) * x)
    / (x * (2.f * c3 - c1 * (x - 3.f)) + c2 * (2.f * x * x - 7.f * x + 1.f)
      + sqrtf(c1 * (4.f * c3 - 12.f * c2 + 13.f * c1 - 4.f))
      * (x - 1.f) * (c1 - x));
}

float RatFuncOscillator::phaseDistort2_1(float x, float c1) {
  float c2 = c1 * c1;
  float c3 = c2 * c1;

  return -(2.f * (c3 - 2.f * c2 + c1) * x)
    / (x * (c1 * (x - 3.f) - 2.f * c3) + c2 * (-2.f * x * x + 7.f * x - 1.f)
      + sqrtf(c1 * (4.f * c3 - 12.f * c2 + 13.f * c1 - 4.f))
      * (x - 1.f) * (c1 - x));
}

float RatFuncOscillator::primaryWaveFunction_1(float x) {
  x -= floorf(x);
  return min(max(
    (x * (2.f * x - 1.f) * (a - b) * (a - b))
    / (a * a * (2.f * SQRT2M1 * b * b - SQRT2M1 * b + x * (2.f * x - 1.f))
      + b * x * (-2.f * a * (2.f * SQRT2M1 * b + 2.f * x - SQRT2)
        + b * (2.f * SQRT2 * x - 1.f)
        - SQRT2M1 * x)),
    -1.f), 1.f);
}

float RatFuncOscillator::primaryWaveFunction(float x) {
  x -= floorf(x);
  float y = (x < .5f) ?
    primaryWaveFunction_1(x) :
    -primaryWaveFunction_1(1.f - x);
  // check for NaNs or infs
  return (abs(y) < 2.f) ? y : 0.f;
}

// we don't need the following two inverse functions for audio output,
// only for putting the reference points on the oscilloscope widget

float RatFuncOscillator::phaseDistortInv1_1(float x, float c1) {
  float c2 = c1 * c1;
  float c3 = c2 * c1;
  float x2 = x * x;
  float A = sqrtf(c1 * (4.f * c3 - 12.f * c2 + 13.f * c1 - 4.f));

  return (-2.f * c3 * x + 2.f * c3 + 7.f * c2 * x - 4.f * c2 + A * c1 * x
    + A * x - 3.f * c1 * x + 2.f * c1)
    / (2.f * (2.f * c2 + A - c1) * x)
    - sqrtf((c3 - 2.f * c2 + c1)
      * (4.f * c3 * x2 - 4.f * c3 * x + 2.f * c3 - 16.f * c2 * x2
        + 14.f * c2 * x - 4.f * c2 - 2.f * A * c1 * x2 - 3.f * A * x2
        + 2.f * A * c1 * x + 2.f * A * x + 11.f * c1 * x2 - 6.f * c1 * x
        + 2.f * c1 - 2.f * x2))
    / (SQRT2 * (2.f * c2 + A - c1) * x);
}

float RatFuncOscillator::phaseDistortInv2_1(float x, float c1) {
  float c2 = c1 * c1;
  float c3 = c2 * c1;
  float x2 = x * x;
  float A = sqrtf(c1 * (4.f * c3 - 12.f * c2 + 13.f * c1 - 4.f));

  return -(-2.f * c3 * x + 2.f * c3 + 7.f * c2 * x - 4.f * c2 - A * c1 * x
    - A * x - 3.f * c1 * x + 2.f * c1)
    / (2.f * (-2.f * c2 + A + c1) * x)
    + sqrtf((c3 - 2.f * c2 + c1)
      * (4.f * c3 * x2 - 4.f * c3 * x + 2.f * c3 - 16.f * c2 * x2
        + 14.f * c2 * x - 4.f * c2 + 2.f * A * c1 * x2 + 3.f * A * x2
        - 2.f * A * c1 * x - 2.f * A * x + 11.f * c1 * x2 - 6.f * c1 * x
        + 2.f * c1 - 2 * x2))
    / (SQRT2 * (-2.f * c2 + A + c1) * x);
}

float RatFuncOscillator::phaseDistort1(float x) {
  x -= floorf(x);
  return (c > .5f) ?
    phaseDistort1_1(x, c) :
    -phaseDistort2_1(1.f - x, 1.f - c);
}

float RatFuncOscillator::phaseDistort2(float x) {
  x -= floorf(x);
  return (c > .5f) ?
    phaseDistort2_1(x, c) :
    -phaseDistort1_1(1.f - x, 1.f - c);
}

float RatFuncOscillator::phaseDistortInv1(float x) {
  if (abs(c - .5f) < 1.e-6f)
    return x;

  float y;
  if (c > .5f) {
    x -= floorf(x);
    y = phaseDistortInv2_1(x, c);
  } else {
    x = -x;
    x -= floorf(x);
    y = -phaseDistortInv1_1(x, 1.f - c);
  }
  y -= floorf(y);
  return y;
}

float RatFuncOscillator::phaseDistortInv2(float x) {
  if (abs(c - .5f) < 1.e-6f)
    return x;

  float y;
  if (c > .5f) {
    x -= floorf(x);
    y = phaseDistortInv1_1(x, c);
  } else {
    x = -x;
    x -= floorf(x);
    y = -phaseDistortInv2_1(x, 1.f - c);
  }
  y -= floorf(y);
  return y;
}

// set the paramaters a, b, and c as values between 0. and 1.
void RatFuncOscillator::setParams(float a, float b, float c) {
  if (restrictParams) {
    // map the parameters in such a way that the worst aliasing is avoided
    // (lots of more or less educated guessing is going on here)

    // exclude values of c around 0 and 1
    float minimum = min(16.f * abs((float)dPh[0]), .5f);
    float maximum = 1.f - minimum;
    c = min(max(c, minimum), maximum);

    // d is the normalized frequency (dPh[0]) 
    // scaled with a factor that is 1 for c == .5
    // and grows bigger for c -> 0 or 1
    float d = abs((float)dPh[0] / (1.f - 2.f * abs(c - .5f)));

    // exclude values of a around 0 and 1
    minimum = min(32.f * d, .5f);
    maximum = max(min(1.f - 16.f * d, .99f), .5f);
    a = min(max(a, minimum), maximum);

    // range of a: (0, .5)
    a *= .5f;

    // exclude values of b around 0 and 1
    minimum = min(8.f * d / (.5f - a), .5f);
    maximum = max(1.f - 4.f * d / a, .5f);
    b = min(max(b, minimum), maximum);

    // range of b: (a, .5)
    b = a + (.5f - a) * b;
  } else {
    c = min(max(c, 0.f), 1.f);
    a = min(max(a, 0.f), 1.f);
    b = min(max(b, 0.f), 1.f);

    // range of a: (0, .5)
    a *= .5f;
    // range of b: (a, .5)
    b = a + (.5f - a) * b;
  }

  this->a = a;
  this->b = b;
  this->c = c;
}

void RatFuncOscillator::process() {
  if (abs(dPh[0]) > .5) {
    wave[0] = 0.f;
    wave[1] = 0.f;
    return;
  }

  wave[0] = waveFunction1(ph[0]);
  wave[1] = waveFunction2(ph[0]);

  incrementPhases();
}
