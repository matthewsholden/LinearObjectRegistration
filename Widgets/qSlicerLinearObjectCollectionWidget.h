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

#ifndef __qSlicerLinearObjectCollectionWidget_h
#define __qSlicerLinearObjectCollectionWidget_h

// Qt includes
#include "qSlicerWidget.h"

// FooBar Widgets includes
#include "qSlicerLinearObjectRegistrationModuleWidgetsExport.h"
#include "ui_qSlicerLinearObjectCollectionWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"


class qSlicerLinearObjectCollectionWidgetPrivate;

/// \ingroup Slicer_QtModules_CreateModels
class Q_SLICER_MODULE_LINEAROBJECTREGISTRATION_WIDGETS_EXPORT 
qSlicerLinearObjectCollectionWidget : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLinearObjectCollectionWidget(QWidget *parent=0);
  virtual ~qSlicerLinearObjectCollectionWidget();

  static qSlicerLinearObjectCollectionWidget* New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic );

  vtkSlicerLinearObjectRegistrationLogic* LORLogic;

  vtkMRMLNode* GetCurrentNode();
  void SetCurrentNode( vtkMRMLNode* currentNode );

protected slots:

  void onCollectionNodeChanged(); // User selects a different node using the combo box

  void onCollectionNodeModified(); // Some attribute of the currently selected node has been modified

  void SetCurrentActive();

  void onCollectionTableContextMenu(const QPoint& position);

  void onLinearObjectEdited( int row, int column );

  void updateWidget();

signals:

  void collectionNodeModified();

  void matchRequested( int index );

protected:
  QScopedPointer<qSlicerLinearObjectCollectionWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerLinearObjectCollectionWidget);
  Q_DISABLE_COPY(qSlicerLinearObjectCollectionWidget);

};

#endif
