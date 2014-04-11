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

#ifndef __qSlicerLORFiducialWidget_h
#define __qSlicerLORFiducialWidget_h

// Qt includes
#include "qSlicerWidget.h"
#include "qSlicerLORCollectControlsWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLORFiducialWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"

#include "vtkMRMLInteractionNode.h"


class qSlicerLORFiducialWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLORFiducialWidget : public qSlicerLORCollectControlsWidget
{
  Q_OBJECT
public:
  typedef qSlicerLORCollectControlsWidget Superclass;
  qSlicerLORFiducialWidget( QWidget *parent = 0 );
  qSlicerLORFiducialWidget( vtkSlicerLinearObjectRegistrationLogic* LORLogic, QWidget *parent = 0 );
  virtual ~qSlicerLORFiducialWidget();

  static qSlicerLORFiducialWidget* New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic );

  virtual std::string GetCollectNodeType();
  virtual std::string GetCollectModeName();

public slots:

  virtual void widgetActivated();
  virtual void widgetDeactivated();

protected slots:

  void onConvertFiducialsButtonClicked();

  void updateWidget();

protected:
  QScopedPointer<qSlicerLORFiducialWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerLORFiducialWidget);
  Q_DISABLE_COPY(qSlicerLORFiducialWidget);

  void ConnectAllButtons();
  void DisconnectAllButtons();
};

#endif
