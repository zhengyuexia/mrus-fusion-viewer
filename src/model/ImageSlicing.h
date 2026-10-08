#pragma once

#include "Types.h"

namespace model
{
Slice2DType::Pointer sliceOf(Volume3DType::Pointer vol, int z);
LabelSlice2DType::Pointer labelSliceOf(Label3DType::Pointer vol, int z);
}
