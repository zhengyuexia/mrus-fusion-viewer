#pragma once

#include "Types.h"

#include <itkTransform.h>

#include <optional>

namespace model
{
struct RegistrationInput
{
  Volume3DType::Pointer mrImage;
  Volume3DType::Pointer usImage;
  Label3DType::Pointer mrLabel; // nullptr when hasLabels is false
  Label3DType::Pointer usLabel;
  bool hasLabels = false;
};

struct RegistrationResult
{
  itk::Transform<double, 3, 3>::Pointer transform;
  Volume3DType::Pointer alignedUsImage;      // US resampled onto the MR (fixed) grid
  Label3DType::Pointer alignedUsLabel;       // nullptr when the input had no labels
  unsigned int iterations = 0;
  double finalMetricValue = 0.0;
  double versor[3] = {0, 0, 0};
  double translationMm[3] = {0, 0, 0};
  std::optional<double> diceAfter;  // unset when hasLabels is false
  std::optional<double> diceBefore; // Identity-transform baseline, for comparison
};

// MR = fixed, US = moving. 3D rigid registration: VersorRigid3DTransform +
// MattesMutualInformationImageToImageMetricv4 + multi-resolution pyramid (3 levels).
// Initialization: label-centroid-guided when labels exist, geometry-centered otherwise.
RegistrationResult runRigidRegistration(const RegistrationInput & input);
}
