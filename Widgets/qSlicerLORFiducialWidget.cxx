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
#include "qSlicerLORFiducialWidget.h"

#include <QtGui>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORFiducialWidgetPrivate
  : public Ui_qSlicerLORFiducialWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORFiducialWidget);
protected:
  qSlicerLORFiducialWidget* const q_ptr;

public:
  qSlicerLORFiducialWidgetPrivate( qSlicerLORFiducialWidget& object);
  ~qSlicerLORFiducialWidgetPrivate();
  virtual void setupUi(qSlicerLORFiducialWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORFiducialWidgetPrivate
::qSlicerLORFiducialWidgetPrivate( qSlicerLORFiducialWidget& object) : q_ptr(&object)
{
}

qSlicerLORFiducialWidgetPrivate
::~qSlicerLORFiducialWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORFiducialWidgetPrivate
::setupUi(qSlicerLORFiducialWidget* widget)
{
  this->Ui_qSlicerLORFiducialWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORFiducialWidget methods

//-----------------------------------------------------------------------------
qSlicerLORFiducialWidget
::qSlicerLORFiducialWidget( QWidget* parentWidget) : Superclass( parentWidget ) , d_ptr( new qSlicerLORFiducialWidgetPrivate(*this) )
{
}


qSlicerLORFiducialWidget
::qSlicerLORFiducialWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget) : Superclass( newLORLogic, parentWidget ) , d_ptr( new qSlicerLORFiducialWidgetPrivate(*this) )
{
}


qSlicerLORFiducialWidget
::~qSlicerLORFiducialWidget()
{
}


qSlicerLORFiducialWidget* qSlicerLORFiducialWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORFiducialWidget* newControlsWidget = new qSlicerLORFiducialWidget( newLORLogic );
  newControlsWidget->setup();
  return newControlsWidget;
}


void qSlicerLORFiducialWidget
::setup()
{
  Q_D(qSlicerLORFiducialWidget);

  d->setupUi(this);
  
  this->ConnectAllButtons();

  this->updateWidget();  
}


void qSlicerLORFiducialWidget
::ConnectAllButtons()
{
  Q_D(qSlicerLORFiducialWidget);
  
  connect( d->ConvertFiducialsButton, SIGNAL( clicked() ), this, SLOT( onConvertFiducialsButtonClicked() ) );
}

void qSlicerLORFiducialWidget
::DisconnectAllButtons()
{
  Q_D(qSlicerLORFiducialWidget);
  
  disconnect( d->ConvertFiducialsButton, SIGNAL( clicked() ), this, SLOT( onConvertFiducialsButtonClicked() ) );
}


std::string qSlicerLORFiducialWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORFiducialWidget);
  
  return "vtkMRMLMarkupsFiducialNode";
}


std::string qSlicerLORFiducialWidget
::GetCollectModeName()
{
  Q_D(qSlicerLORFiducialWidget);
  
  return LORConstants::FIDUCIAL_COLLECT_MODE;
}


void qSlicerLORFiducialWidget
::enter()
{
}


void qSlicerLORFiducialWidget
::widgetActivated()
{
}


void qSlicerLORFiducialWidget
::widgetDeactivated()
{
}


void qSlicerLORFiducialWidget
::onConvertFiducialsButtonClicked()
{
  Q_D(qSlicerLORFiducialWidget);

  // Start collecting
  this->LORNode->StartCollecting( this->CollectNode, LORConstants::UNKNOWNDOF_COLLECT_STATE );
  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->AddFiducialsToPositionBuffer( this->LORNode->GetActivePositionBuffer(), this->CollectNode );
  // Stop collecting
  this->LORNode->StopCollecting();
}


void qSlicerLORFiducialWidget
::updateWidget()
{
  Q_D(qSlicerLORFiducialWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  // Nothing to do

}
