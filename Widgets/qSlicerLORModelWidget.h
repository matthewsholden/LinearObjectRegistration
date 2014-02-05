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

#ifndef __qSlicerLORModelWidget_h
#define __qSlicerLORModelWidget_h

// Qt includes
#include "qSlicerWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLORModelWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"


class qSlicerLORModelWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLORModelWidget : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLORModelWidget(QWidget *parent=0);
  virtual ~qSlicerLORModelWidget();

  static qSlicerLORModelWidget* New( vtkSlicerLinearObjectRegistrationLogic* LORLogic );

  void SetLORNode( vtkMRMLNode* newNode );

protected slots:

  void onReferenceButtonClicked();
  void onPointButtonClicked();
  void onLineButtonClicked();
  void onPlaneButtonClicked();

  void updateWidget();

protected:
  QScopedPointer<qSlicerLORModelWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerLORModelWidget);
  Q_DISABLE_COPY(qSlicerLORModelWidget);

  vtkMRMLLinearObjectRegistrationNode* LORNode;
  vtkSlicerLinearObjectRegistrationLogic* LORLogic;
};

#endif
