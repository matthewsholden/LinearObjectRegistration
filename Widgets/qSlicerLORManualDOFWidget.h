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

#ifndef __qSlicerLORManualDOFWidget_h
#define __qSlicerLORManualDOFWidget_h

// Qt includes
#include "qSlicerWidget.h"
#include "qSlicerLORCollectControlsWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLORManualDOFWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"


class qSlicerLORManualDOFWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLORManualDOFWidget : public qSlicerLORCollectControlsWidget
{
  Q_OBJECT
public:
  
  typedef qSlicerLORCollectControlsWidget Superclass;
  qSlicerLORManualDOFWidget( QWidget *parent = 0 );
  qSlicerLORManualDOFWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget *parent = 0 );
  virtual ~qSlicerLORManualDOFWidget();

  static qSlicerLORManualDOFWidget* New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic );

  virtual std::string GetCollectNodeType();
  virtual std::string GetCollectModeName();

public slots:

  virtual void widgetActivated();
  virtual void widgetDeactivated();

protected slots:

  void onReferenceButtonToggled();
  void onPointButtonToggled();
  void onLineButtonToggled();
  void onPlaneButtonToggled();

  void updateWidget();

protected:
  QScopedPointer<qSlicerLORManualDOFWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerLORManualDOFWidget);
  Q_DISABLE_COPY(qSlicerLORManualDOFWidget);

  void ConnectAllButtons();
  void DisconnectAllButtons();
};

#endif
