#include "SegmentationService.h"

#include <itkConnectedThresholdImageFilter.h>
#include <itkLabelOverlapMeasuresImageFilter.h>

#include <limits>

namespace model
{
LabelSlice2DType::Pointer runConnectedThreshold(Slice2DType::Pointer slice,
                                                 Slice2DType::IndexType seed,
                                                 int lower,
                                                 int upper)
{
  using ConnectedFilterType = itk::ConnectedThresholdImageFilter<Slice2DType, LabelSlice2DType>;
  auto segmenter = ConnectedFilterType::New();
  segmenter->SetInput(slice);
  segmenter->SetLower(lower);
  segmenter->SetUpper(upper);
  segmenter->SetReplaceValue(255);
  segmenter->SetSeed(seed);
  segmenter->Update(); // may throw — caller decides how to surface it
  return segmenter->GetOutput();
}

double computeDice(LabelSlice2DType::Pointer a, LabelSlice2DType::Pointer b, QString * errorOut)
{
  b->SetSpacing(a->GetSpacing());
  b->SetOrigin(a->GetOrigin());
  b->SetDirection(a->GetDirection());

  using OverlapType = itk::LabelOverlapMeasuresImageFilter<LabelSlice2DType>;
  auto overlap = OverlapType::New();
  overlap->SetSourceImage(a);
  overlap->SetTargetImage(b);
  try
  {
    overlap->Update();
    return overlap->GetDiceCoefficient();
  }
  catch (const itk::ExceptionObject & err)
  {
    if (errorOut)
    {
      *errorOut = QString("Dice 计算出错: ") + err.GetDescription();
    }
    return std::numeric_limits<double>::quiet_NaN();
  }
}
}
