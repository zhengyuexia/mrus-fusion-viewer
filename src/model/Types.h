#pragma once

#include <itkImage.h>

namespace model
{
using PixelType = float;
using LabelPixelType = unsigned char;
using Volume3DType = itk::Image<PixelType, 3>;
using Label3DType = itk::Image<LabelPixelType, 3>;
using Slice2DType = itk::Image<PixelType, 2>;
using LabelSlice2DType = itk::Image<LabelPixelType, 2>;
}
