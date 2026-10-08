#pragma once

#include "Types.h"

#include <QImage>
#include <QString>

namespace model
{
QImage grayToQImage(Slice2DType::Pointer slice);
QImage grayToQImage(LabelSlice2DType::Pointer slice); // 0/255 displayed directly as grayscale

QImage overlayToQImage(Slice2DType::Pointer gray, LabelSlice2DType::Pointer label, int channel);
QImage blendToQImage(Slice2DType::Pointer a, Slice2DType::Pointer b, double alpha);
QImage checkerToQImage(Slice2DType::Pointer a, Slice2DType::Pointer b, int pattern);

// Red = ourMask, green = gtSlice, yellow = overlap. Returns a null QImage and sets *errorOut
// if the three buffers don't have matching sizes — that mismatch is a real out-of-bounds read,
// not an itk::ExceptionObject, so it must be checked before the pixel loop, not caught after.
QImage overlaySegmentationVsGroundTruth(Slice2DType::Pointer gray,
                                         LabelSlice2DType::Pointer ourMask,
                                         LabelSlice2DType::Pointer gtSlice,
                                         QString * errorOut);
}
