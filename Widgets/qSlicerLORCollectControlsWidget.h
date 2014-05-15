/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerLORCollectControlsWidget_h
#define __qSlicerLORCollectControlsWidget_h

// Qt includes
#include "qSlicerWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLORCollectControlsWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"


class qSlicerLORCollectControlsWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLORCollectControlsWidget : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLORCollectControlsWidget( QWidget *parent = 0 );
  qSlicerLORCollectControlsWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget *parent = 0 );
  virtual ~qSlicerLORCollectControlsWidget();

  static qSlicerLORCollectControlsWidget* New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic );

  void SetLORNode( vtkMRMLNode* newNode );
  virtual void SetAndObserveCollectNode( vtkMRMLNode* newCollectNode );

  virtual std::string GetCollectNodeType();
  virtual std::string GetCollectModeName();

public slots:
 
  virtual void widgetActivated();
  virtual void widgetDeactivated();
  
signals:

  void collectStarted( int collectionState );
  void collectStopped();

protected slots:

  virtual void updateWidget();
  

protected:
  QScopedPointer<qSlicerLORCollectControlsWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();

  void initialize();

  vtkMRMLLinearObjectRegistrationNode* LORNode;
  vtkSlicerLinearObjectRegistrationLogic* LORLogic;
  
  vtkMRMLNode* CollectNode;

  virtual void installShortcuts();
  virtual void uninstallShortcuts();

private:
  Q_DECLARE_PRIVATE(qSlicerLORCollectControlsWidget);
  Q_DISABLE_COPY(qSlicerLORCollectControlsWidget);
};

#endif
