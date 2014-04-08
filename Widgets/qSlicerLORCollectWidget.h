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

#ifndef __qSlicerLORCollectWidget_h
#define __qSlicerLORCollectWidget_h

// Qt includes
#include "qSlicerWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLORCollectWidget.h"

#include "qSlicerLORCollectControlsWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"


class qSlicerLORCollectWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLORCollectWidget : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLORCollectWidget(QWidget *parent=0);
  virtual ~qSlicerLORCollectWidget();

  static qSlicerLORCollectWidget* New( vtkSlicerLinearObjectRegistrationLogic* LORLogic );

  void SetControlsWidget( qSlicerLORCollectControlsWidget* newControlsWidget );
  qSlicerLORCollectControlsWidget* GetControlsWidget();

  void SetLORNode( vtkMRMLNode* newNode );


public slots:

  void show();
  void hide();
  
  virtual void widgetActivated();
  virtual void widgetDeactivated();
  
signals:

  void collectStarted( int collectionState );
  void collectStopped();

protected slots:

  virtual void updateWidget();
  

protected:
  QScopedPointer<qSlicerLORCollectWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();


private:
  Q_DECLARE_PRIVATE(qSlicerLORCollectWidget);
  Q_DISABLE_COPY(qSlicerLORCollectWidget);

  vtkMRMLLinearObjectRegistrationNode* LORNode;
  vtkSlicerLinearObjectRegistrationLogic* LORLogic;
  
  qSlicerLORCollectControlsWidget* ControlsWidget;
};

#endif
