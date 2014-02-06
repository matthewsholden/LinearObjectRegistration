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

// FooBar Widgets includes
#include "qSlicerLORModelWidget.h"

#include <QtGui>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORModelWidgetPrivate
  : public Ui_qSlicerLORModelWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORModelWidget);
protected:
  qSlicerLORModelWidget* const q_ptr;

public:
  qSlicerLORModelWidgetPrivate( qSlicerLORModelWidget& object);
  ~qSlicerLORModelWidgetPrivate();
  virtual void setupUi(qSlicerLORModelWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORModelWidgetPrivate
::qSlicerLORModelWidgetPrivate( qSlicerLORModelWidget& object) : q_ptr(&object)
{
}

qSlicerLORModelWidgetPrivate
::~qSlicerLORModelWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORModelWidgetPrivate
::setupUi(qSlicerLORModelWidget* widget)
{
  this->Ui_qSlicerLORModelWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORModelWidget methods

//-----------------------------------------------------------------------------
qSlicerLORModelWidget
::qSlicerLORModelWidget(QWidget* parentWidget) : Superclass( parentWidget ) , d_ptr( new qSlicerLORModelWidgetPrivate(*this) )
{
}


qSlicerLORModelWidget
::~qSlicerLORModelWidget()
{
}


qSlicerLORModelWidget* qSlicerLORModelWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORModelWidget* newLORModelWidget = new qSlicerLORModelWidget();
  newLORModelWidget->LORLogic = newLORLogic;
  newLORModelWidget->LORNode = NULL;
  newLORModelWidget->setup();
  return newLORModelWidget;
}


void qSlicerLORModelWidget
::setup()
{
  Q_D(qSlicerLORModelWidget);

  d->setupUi(this);

  d->ModelNodeComboBox->setMRMLScene( this->LORLogic->GetMRMLScene() );
  
  // Use the pressed signal (otherwise we can unpress buttons without clicking them)
  connect( d->ReferenceButton, SIGNAL( clicked() ), this, SLOT( onReferenceButtonClicked() ) );
  connect( d->PointButton, SIGNAL( clicked() ), this, SLOT( onPointButtonClicked() ) );
  connect( d->LineButton, SIGNAL( clicked() ), this, SLOT( onLineButtonClicked() ) );
  connect( d->PlaneButton, SIGNAL( clicked() ), this, SLOT( onPlaneButtonClicked() ) );

  this->updateWidget();  
}


void qSlicerLORModelWidget
::enter()
{
}


void qSlicerLORModelWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORModelWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;
}


void qSlicerLORModelWidget
::onReferenceButtonClicked()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::REFERENCE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelReference( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer() );

  // Stop collecting
  this->LORNode->StopCollecting();
}


void qSlicerLORModelWidget
::onPointButtonClicked()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::POINT_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelPoint( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer() );

  // Stop collecting
  this->LORNode->StopCollecting();
}


void qSlicerLORModelWidget
::onLineButtonClicked()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::LINE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelLine( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer() );

  // Stop collecting
  this->LORNode->StopCollecting();
}


void qSlicerLORModelWidget
::onPlaneButtonClicked()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::PLANE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelPlane( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer() );

  // Stop collecting
  this->LORNode->StopCollecting();
}



void qSlicerLORModelWidget
::updateWidget()
{
  Q_D(qSlicerLORModelWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  // Nothing to do

}