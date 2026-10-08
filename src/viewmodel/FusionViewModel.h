#pragma once

#include "CaseViewModel.h"
#include "RegisterViewModel.h"

#include <QObject>
#include <QImage>

class FusionViewModel : public QObject
{
  Q_OBJECT
public:
  explicit FusionViewModel(CaseViewModel & caseViewModel, RegisterViewModel & registerViewModel,
                            QObject * parent = nullptr);

  enum class Mode
  {
    Blend,
    Checker
  };

public slots:
  void setMode(Mode mode);
  void setParam(int param);
  void setSliceZ(int z);
  void updateFusion();

signals:
  void sliceRangeChanged(int maxZ);
  void fusionImageChanged(QImage image);
  void placeholderMessage(QString message);

private:
  CaseViewModel & m_caseViewModel;
  RegisterViewModel & m_registerViewModel;
  Mode m_mode = Mode::Blend;
  int m_param = 50;
  int m_sliceZ = 0;
};
