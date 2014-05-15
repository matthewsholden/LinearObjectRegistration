
#ifndef __qSlicerLinearObjectRegistrationModuleWidget_h
#define __qSlicerLinearObjectRegistrationModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerLORCollectWidget.h"
#include "qSlicerLORCollectControlsWidget.h"
#include "qSlicerLORManualDOFWidget.h"
#include "qSlicerLORManualSegmentationWidget.h"
#include "qSlicerLORAutomaticWidget.h"
#include "qSlicerLORModelWidget.h"

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

  void UpdateMatchCandidates();
  void OnMatchButtonClicked();

  void UpdateToMRMLNode();
  void UpdateFromMRMLNode();

  void PostProcessCollectionWidgets();

protected:
  QScopedPointer<qSlicerLinearObjectRegistrationModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  virtual void enter();
  virtual void exit();

  void EnableAllWidgets( bool enable );

private:
  Q_DECLARE_PRIVATE(qSlicerLinearObjectRegistrationModuleWidget);
  Q_DISABLE_COPY(qSlicerLinearObjectRegistrationModuleWidget);
  
  void ConnectWidgets();
  void DisconnectWidgets();

  void AddCollectWidget( qSlicerLORCollectControlsWidget* newControlsWidget );
  void UpdateCollectWidgets( vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode );

  std::map< int, qSlicerLORCollectWidget* > CollectWidgets;  
};

#endif
