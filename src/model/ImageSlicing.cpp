#include "ImageSlicing.h"

#include <itkExtractImageFilter.h>

namespace model
{
Slice2DType::Pointer sliceOf(Volume3DType::Pointer vol, int z)
{
  using ExtractFilterType = itk::ExtractImageFilter<Volume3DType, Slice2DType>;
  auto region = vol->GetLargestPossibleRegion();
  auto size = region.GetSize();
  size[2] = 0;
  auto index = region.GetIndex();
  index[2] = z;
  Volume3DType::RegionType extractRegion;
  extractRegion.SetSize(size);
  extractRegion.SetIndex(index);

  auto filter = ExtractFilterType::New();
  filter->SetExtractionRegion(extractRegion);
  filter->SetInput(vol);
  filter->SetDirectionCollapseToSubmatrix();
  filter->Update();
  return filter->GetOutput();
}

LabelSlice2DType::Pointer labelSliceOf(Label3DType::Pointer vol, int z)
{
  using ExtractFilterType = itk::ExtractImageFilter<Label3DType, LabelSlice2DType>;
  auto region = vol->GetLargestPossibleRegion();
  auto size = region.GetSize();
  size[2] = 0;
  auto index = region.GetIndex();
  index[2] = z;
  Label3DType::RegionType extractRegion;
  extractRegion.SetSize(size);
  extractRegion.SetIndex(index);

  auto filter = ExtractFilterType::New();
  filter->SetExtractionRegion(extractRegion);
  filter->SetInput(vol);
  filter->SetDirectionCollapseToSubmatrix();
  filter->Update();
  return filter->GetOutput();
}
}
