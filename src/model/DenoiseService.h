#pragma once

#include "Types.h"

namespace model
{
struct DenoiseResult
{
  Slice2DType::Pointer denoised;
  double meanAbsDiff = 0.0;
  double sigmaAbsDiff = 0.0;
  double maxAbsDiff = 0.0;
};

// itk::CurvatureFlowImageFilter — PDE-based edge-preserving denoising.
DenoiseResult runCurvatureFlow(Slice2DType::Pointer input, unsigned int iterations, double timeStep);
}
