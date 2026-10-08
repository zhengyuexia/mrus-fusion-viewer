#pragma once

#include "CaseViewModel.h"
#include "../model/Types.h"

#include <itkTransform.h>

#include <QObject>
#include <QImage>
#include <QString>

class RegisterViewModel : public QObject
{
  Q_OBJECT
public:
  explicit RegisterViewModel(CaseViewModel & caseViewModel, QObject * parent = nullptr);

  model::Volume3DType::Pointer alignedUsImage() const { return m_alignedUsImage; }
  model::Label3DType::Pointer alignedUsLabel() const { return m_alignedUsLabel; }
  bool hasResult() const { return m_alignedUsImage.IsNotNull(); }

public slots:
  void setSliceZ(int z);
  void runRegistration();

signals:
  void statusTextChanged(QString text);
  void sliceRangeChanged(int maxZ, int currentZ);
  void fixedSliceChanged(QImage image);
  void alignedSliceChanged(QImage image);
  void resultReady(); // tells dependents (e.g. FusionViewModel) a new aligned volume exists

private:
  void onCaseLoaded();
  void refreshSliceViews();

  CaseViewModel & m_caseViewModel;
  int m_sliceZ = 0;
  itk::Transform<double, 3, 3>::Pointer m_transform;
  model::Volume3DType::Pointer m_alignedUsImage;
  model::Label3DType::Pointer m_alignedUsLabel;
};
