#include "ImageConversion.h"

#include <itkRescaleIntensityImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkCheckerBoardImageFilter.h>

#include <algorithm>
#include <vector>

namespace model
{
namespace
{
using OutType = itk::Image<unsigned char, 2>;

OutType::Pointer rescaleTo8Bit(Slice2DType::Pointer slice)
{
  using RescaleType = itk::RescaleIntensityImageFilter<Slice2DType, OutType>;
  auto rescaler = RescaleType::New();
  rescaler->SetInput(slice);
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  rescaler->Update();
  return rescaler->GetOutput();
}
}

QImage grayToQImage(Slice2DType::Pointer slice)
{
  auto out = rescaleTo8Bit(slice);
  auto size = out->GetLargestPossibleRegion().GetSize();
  int w = static_cast<int>(size[0]);
  int h = static_cast<int>(size[1]);
  QImage img(out->GetBufferPointer(), w, h, w, QImage::Format_Grayscale8);
  return img.copy(); // deep copy, detach from the ITK object's lifetime
}

QImage grayToQImage(LabelSlice2DType::Pointer slice)
{
  auto size = slice->GetLargestPossibleRegion().GetSize();
  int w = static_cast<int>(size[0]);
  int h = static_cast<int>(size[1]);
  QImage img(slice->GetBufferPointer(), w, h, w, QImage::Format_Grayscale8);
  return img.copy();
}

QImage overlayToQImage(Slice2DType::Pointer gray, LabelSlice2DType::Pointer label, int channel)
{
  auto grayU8 = rescaleTo8Bit(gray);
  auto size = grayU8->GetLargestPossibleRegion().GetSize();
  int w = static_cast<int>(size[0]);
  int h = static_cast<int>(size[1]);

  std::vector<unsigned char> buf(static_cast<size_t>(w) * h * 3);
  const unsigned char * g = grayU8->GetBufferPointer();
  const unsigned char * l = label ? label->GetBufferPointer() : nullptr;
  for (int i = 0; i < w * h; ++i)
  {
    unsigned char gv = g[i];
    bool hit = l && l[i] > 0;
    for (int c = 0; c < 3; ++c)
    {
      unsigned char v = gv;
      if (hit && c == channel)
      {
        v = static_cast<unsigned char>(std::min(255, gv + 120));
      }
      else if (hit)
      {
        v = gv / 2;
      }
      buf[static_cast<size_t>(i) * 3 + c] = v;
    }
  }
  QImage img(buf.data(), w, h, w * 3, QImage::Format_RGB888);
  return img.copy();
}

QImage blendToQImage(Slice2DType::Pointer a, Slice2DType::Pointer b, double alpha)
{
  using MultiplyType = itk::MultiplyImageFilter<Slice2DType, Slice2DType, Slice2DType>;
  auto sa = MultiplyType::New();
  sa->SetInput(a);
  sa->SetConstant(alpha);
  auto sb = MultiplyType::New();
  sb->SetInput(b);
  sb->SetConstant(1.0 - alpha);
  using AddType = itk::AddImageFilter<Slice2DType, Slice2DType, Slice2DType>;
  auto add = AddType::New();
  add->SetInput1(sa->GetOutput());
  add->SetInput2(sb->GetOutput());
  add->Update();
  return grayToQImage(add->GetOutput());
}

QImage checkerToQImage(Slice2DType::Pointer a, Slice2DType::Pointer b, int pattern)
{
  using CheckerType = itk::CheckerBoardImageFilter<Slice2DType>;
  auto checker = CheckerType::New();
  checker->SetInput1(a);
  checker->SetInput2(b);
  CheckerType::PatternArrayType p;
  p[0] = pattern;
  p[1] = pattern;
  checker->SetCheckerPattern(p);
  checker->Update();
  return grayToQImage(checker->GetOutput());
}

QImage overlaySegmentationVsGroundTruth(Slice2DType::Pointer gray,
                                         LabelSlice2DType::Pointer ourMask,
                                         LabelSlice2DType::Pointer gtSlice,
                                         QString * errorOut)
{
  auto grayU8 = rescaleTo8Bit(gray);
  auto size = grayU8->GetLargestPossibleRegion().GetSize();
  auto maskSize = ourMask->GetLargestPossibleRegion().GetSize();
  auto gtSize = gtSlice->GetLargestPossibleRegion().GetSize();
  if (size != maskSize || size != gtSize)
  {
    // The three buffers must be byte-for-byte the same size to composite by linear index;
    // otherwise this is a genuine out-of-bounds read, not something try/catch can save us from.
    if (errorOut)
    {
      *errorOut = "内部错误: 灰度图/分割结果/真实标注三者尺寸不一致,已跳过叠加显示";
    }
    return QImage();
  }

  int w = static_cast<int>(size[0]);
  int h = static_cast<int>(size[1]);
  std::vector<unsigned char> buf(static_cast<size_t>(w) * h * 3);
  const unsigned char * g = grayU8->GetBufferPointer();
  const unsigned char * our = ourMask->GetBufferPointer();
  const unsigned char * gt = gtSlice->GetBufferPointer();
  for (int i = 0; i < w * h; ++i)
  {
    unsigned char gv = g[i];
    bool ourHit = our[i] > 0;
    bool gtHit = gt[i] > 0;
    unsigned char r = gv, gch = gv, b = gv;
    if (ourHit || gtHit)
    {
      b = gv / 2;
      r = ourHit ? static_cast<unsigned char>(std::min(255, gv + 120)) : gv / 2;
      gch = gtHit ? static_cast<unsigned char>(std::min(255, gv + 120)) : gv / 2;
    }
    buf[static_cast<size_t>(i) * 3 + 0] = r;
    buf[static_cast<size_t>(i) * 3 + 1] = gch;
    buf[static_cast<size_t>(i) * 3 + 2] = b;
  }
  QImage img(buf.data(), w, h, w * 3, QImage::Format_RGB888);
  return img.copy();
}
}
