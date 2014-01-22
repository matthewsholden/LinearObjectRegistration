
#ifndef __qSlicerLinearObjectRegistrationModuleWidget_h
#define __qSlicerLinearObjectRegistrationModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerLinearObjectRegistrationModuleExport.h"

class qSlicerLinearObjectRegistrationModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_LINEAROBJECTREGISTRATION_EXPORT qSlicerLinearObjectRegistrationModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerLinearObjectRegistrationModuleWidget(QWidget *parent=0);
  virtual ~qSlicerLinearObjectRegistrationModuleWidget();

public slots:

  void OnFromMatchRequested( int matchIndex );
  void OnToMatchRequested( int matchIndex );

  void UpdateToMRMLNode();
  void UpdateFromMRMLNode();

protected:
  QScopedPointer<qSlicerLinearObjectRegistrationModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerLinearObjectRegistrationModuleWidget);
  Q_DISABLE_COPY(qSlicerLinearObjectRegistrationModuleWidget);
  
  void ConnectWidgets();
  void DisconnectWidgets();

  int FromMatchState;
  int ToMatchState;
  
};

#endif
