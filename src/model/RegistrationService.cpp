#include "RegistrationService.h"

#include <itkVersorRigid3DTransform.h>
#include <itkImageMomentsCalculator.h>
#include <itkCenteredTransformInitializer.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkResampleImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkIdentityTransform.h>
#include <itkLabelOverlapMeasuresImageFilter.h>

namespace model
{
RegistrationResult runRigidRegistration(const RegistrationInput & input)
{
  using TransformType = itk::VersorRigid3DTransform<double>;
  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using MetricType = itk::MattesMutualInformationImageToImageMetricv4<Volume3DType, Volume3DType>;
  using RegistrationType = itk::ImageRegistrationMethodv4<Volume3DType, Volume3DType, TransformType>;

  auto metric = MetricType::New();
  metric->SetNumberOfHistogramBins(50);
  auto optimizer = OptimizerType::New();
  auto registration = RegistrationType::New();
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);

  auto transform = TransformType::New();
  transform->SetIdentity();

  if (input.hasLabels && input.mrLabel && input.usLabel)
  {
    using MomentsCalculatorType = itk::ImageMomentsCalculator<Label3DType>;
    auto fixedMoments = MomentsCalculatorType::New();
    fixedMoments->SetImage(input.mrLabel);
    fixedMoments->Compute();
    auto movingMoments = MomentsCalculatorType::New();
    movingMoments->SetImage(input.usLabel);
    movingMoments->Compute();
    auto fixedCentroid = fixedMoments->GetCenterOfGravity();
    auto movingCentroid = movingMoments->GetCenterOfGravity();

    TransformType::InputPointType center;
    center[0] = fixedCentroid[0];
    center[1] = fixedCentroid[1];
    center[2] = fixedCentroid[2];
    transform->SetCenter(center);
    TransformType::OutputVectorType initialTranslation;
    initialTranslation[0] = movingCentroid[0] - fixedCentroid[0];
    initialTranslation[1] = movingCentroid[1] - fixedCentroid[1];
    initialTranslation[2] = movingCentroid[2] - fixedCentroid[2];
    transform->SetTranslation(initialTranslation);
  }
  else
  {
    // No segmentation labels (e.g. BITE brain data) -> fall back to geometric center alignment.
    // Cruder than label-centroid init, but it's the only option without labels to compute a centroid from.
    using InitializerType = itk::CenteredTransformInitializer<TransformType, Volume3DType, Volume3DType>;
    auto initializer = InitializerType::New();
    initializer->SetTransform(transform);
    initializer->SetFixedImage(input.mrImage);
    initializer->SetMovingImage(input.usImage);
    initializer->GeometryOn();
    initializer->InitializeTransform();
  }

  registration->SetInitialTransform(transform);
  registration->InPlaceOn();
  registration->SetFixedImage(input.mrImage);
  registration->SetMovingImage(input.usImage);
  registration->SetMetricSamplingStrategy(RegistrationType::MetricSamplingStrategyEnum::NONE);

  OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
  constexpr double translationScale = 1.0 / 1000.0;
  scales[0] = 1.0;
  scales[1] = 1.0;
  scales[2] = 1.0;
  scales[3] = translationScale;
  scales[4] = translationScale;
  scales[5] = translationScale;
  optimizer->SetScales(scales);
  optimizer->SetLearningRate(0.2);
  optimizer->SetMinimumStepLength(0.0001);
  optimizer->SetRelaxationFactor(0.5);
  optimizer->SetNumberOfIterations(300);

  constexpr unsigned int numberOfLevels = 3;
  RegistrationType::ShrinkFactorsArrayType shrink;
  shrink.SetSize(numberOfLevels);
  shrink[0] = 4;
  shrink[1] = 2;
  shrink[2] = 1;
  RegistrationType::SmoothingSigmasArrayType sigmas;
  sigmas.SetSize(numberOfLevels);
  sigmas[0] = 2;
  sigmas[1] = 1;
  sigmas[2] = 0;
  registration->SetNumberOfLevels(numberOfLevels);
  registration->SetSmoothingSigmasPerLevel(sigmas);
  registration->SetShrinkFactorsPerLevel(shrink);

  registration->Update(); // may throw — caller decides how to surface it

  RegistrationResult result;
  result.transform = transform.GetPointer();
  result.iterations = optimizer->GetCurrentIteration();
  result.finalMetricValue = optimizer->GetValue();

  TransformType::ParametersType params = transform->GetParameters();
  for (int i = 0; i < 3; ++i)
  {
    result.versor[i] = params[i];
    result.translationMm[i] = params[3 + i];
  }

  using ResampleType = itk::ResampleImageFilter<Volume3DType, Volume3DType>;
  auto resampler = ResampleType::New();
  resampler->SetTransform(transform);
  resampler->SetInput(input.usImage);
  resampler->SetSize(input.mrImage->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(input.mrImage->GetOrigin());
  resampler->SetOutputSpacing(input.mrImage->GetSpacing());
  resampler->SetOutputDirection(input.mrImage->GetDirection());
  resampler->SetDefaultPixelValue(0);
  resampler->Update();
  result.alignedUsImage = resampler->GetOutput();

  if (input.hasLabels && input.mrLabel && input.usLabel)
  {
    using LabelResampleType = itk::ResampleImageFilter<Label3DType, Label3DType>;
    auto labelResampler = LabelResampleType::New();
    labelResampler->SetTransform(transform);
    labelResampler->SetInput(input.usLabel);
    labelResampler->SetSize(input.mrLabel->GetLargestPossibleRegion().GetSize());
    labelResampler->SetOutputOrigin(input.mrLabel->GetOrigin());
    labelResampler->SetOutputSpacing(input.mrLabel->GetSpacing());
    labelResampler->SetOutputDirection(input.mrLabel->GetDirection());
    labelResampler->SetDefaultPixelValue(0);
    using NNInterpType = itk::NearestNeighborInterpolateImageFunction<Label3DType, double>;
    auto nn = NNInterpType::New();
    labelResampler->SetInterpolator(nn);
    labelResampler->Update();
    result.alignedUsLabel = labelResampler->GetOutput();

    using OverlapType = itk::LabelOverlapMeasuresImageFilter<Label3DType>;
    auto overlapAfter = OverlapType::New();
    overlapAfter->SetSourceImage(input.mrLabel);
    overlapAfter->SetTargetImage(result.alignedUsLabel);
    overlapAfter->Update();
    result.diceAfter = overlapAfter->GetDiceCoefficient();

    auto identity = itk::IdentityTransform<double, 3>::New();
    auto baselineResampler = LabelResampleType::New();
    baselineResampler->SetTransform(identity);
    baselineResampler->SetInput(input.usLabel);
    baselineResampler->SetSize(input.mrLabel->GetLargestPossibleRegion().GetSize());
    baselineResampler->SetOutputOrigin(input.mrLabel->GetOrigin());
    baselineResampler->SetOutputSpacing(input.mrLabel->GetSpacing());
    baselineResampler->SetOutputDirection(input.mrLabel->GetDirection());
    baselineResampler->SetDefaultPixelValue(0);
    baselineResampler->SetInterpolator(nn);
    baselineResampler->Update();

    auto overlapBefore = OverlapType::New();
    overlapBefore->SetSourceImage(input.mrLabel);
    overlapBefore->SetTargetImage(baselineResampler->GetOutput());
    overlapBefore->Update();
    result.diceBefore = overlapBefore->GetDiceCoefficient();
  }
  else
  {
    result.alignedUsLabel = nullptr;
  }

  return result;
}
}
