#include "DenoiseService.h"

#include <itkCurvatureFlowImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkAbsImageFilter.h>
#include <itkStatisticsImageFilter.h>

namespace model
{
DenoiseResult runCurvatureFlow(Slice2DType::Pointer input, unsigned int iterations, double timeStep)
{
  using FilterType = itk::CurvatureFlowImageFilter<Slice2DType, Slice2DType>;
  auto filter = FilterType::New();
  filter->SetInput(input);
  filter->SetNumberOfIterations(iterations);
  filter->SetTimeStep(timeStep);
  filter->Update(); // may throw itk::ExceptionObject — caller decides how to surface it

  DenoiseResult result;
  result.denoised = filter->GetOutput();

  using SubtractType = itk::SubtractImageFilter<Slice2DType, Slice2DType, Slice2DType>;
  auto sub = SubtractType::New();
  sub->SetInput1(input);
  sub->SetInput2(result.denoised);
  using AbsType = itk::AbsImageFilter<Slice2DType, Slice2DType>;
  auto absF = AbsType::New();
  absF->SetInput(sub->GetOutput());
  using StatsType = itk::StatisticsImageFilter<Slice2DType>;
  auto stats = StatsType::New();
  stats->SetInput(absF->GetOutput());
  stats->Update();

  result.meanAbsDiff = stats->GetMean();
  result.sigmaAbsDiff = stats->GetSigma();
  result.maxAbsDiff = stats->GetMaximum();
  return result;
}
}
