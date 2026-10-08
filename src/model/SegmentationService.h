#pragma once

#include "Types.h"

#include <QString>

namespace model
{
LabelSlice2DType::Pointer runConnectedThreshold(Slice2DType::Pointer slice,
                                                 Slice2DType::IndexType seed,
                                                 int lower,
                                                 int upper);

// Aligns spacing/origin/direction of `b` onto `a` before computing Dice — two independent
// ExtractImageFilter calls on the same grid aren't guaranteed to match bit-for-bit, and
// itk::LabelOverlapMeasuresImageFilter throws if they don't. Wraps Update() in try/catch too.
// Returns NaN and sets *errorOut on failure instead of letting the exception escape.
double computeDice(LabelSlice2DType::Pointer a, LabelSlice2DType::Pointer b, QString * errorOut);
}
