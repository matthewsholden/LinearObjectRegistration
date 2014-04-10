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
#include "qSlicerLORManualDOFWidget.h"

#include <QtGui>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORManualDOFWidgetPrivate
  : public Ui_qSlicerLORManualDOFWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORManualDOFWidget);
protected:
  qSlicerLORManualDOFWidget* const q_ptr;

public:
  qSlicerLORManualDOFWidgetPrivate( qSlicerLORManualDOFWidget& object);
  ~qSlicerLORManualDOFWidgetPrivate();
  virtual void setupUi(qSlicerLORManualDOFWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORManualDOFWidgetPrivate
::qSlicerLORManualDOFWidgetPrivate( qSlicerLORManualDOFWidget& object) : q_ptr(&object)
{
}

qSlicerLORManualDOFWidgetPrivate
::~qSlicerLORManualDOFWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORManualDOFWidgetPrivate
::setupUi(qSlicerLORManualDOFWidget* widget)
{
  this->Ui_qSlicerLORManualDOFWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORManualDOFWidget methods

//-----------------------------------------------------------------------------
qSlicerLORManualDOFWidget
::qSlicerLORManualDOFWidget( QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORManualDOFWidgetPrivate(*this) )
{
}


qSlicerLORManualDOFWidget
::qSlicerLORManualDOFWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget ) : Superclass( newLORLogic, parentWidget ) , d_ptr( new qSlicerLORManualDOFWidgetPrivate(*this) )
{
}


qSlicerLORManualDOFWidget
::~qSlicerLORManualDOFWidget()
{
}


qSlicerLORManualDOFWidget* qSlicerLORManualDOFWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORManualDOFWidget* newControlsWidget = new qSlicerLORManualDOFWidget( newLORLogic );
  newControlsWidget->setup();
  return newControlsWidget;
}


void qSlicerLORManualDOFWidget
::setup()
{
  Q_D(qSlicerLORManualDOFWidget);

  d->setupUi(this);
  
  this->ConnectAllButtons();

  this->updateWidget();  
}


void qSlicerLORManualDOFWidget
::ConnectAllButtons()
{
  Q_D(qSlicerLORManualDOFWidget);
  
  connect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonToggled() ) );
  connect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonToggled() ) );
  connect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonToggled() ) );
  connect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonToggled() ) );
}


void qSlicerLORManualDOFWidget
::DisconnectAllButtons()
{
  Q_D(qSlicerLORManualDOFWidget);
  
  disconnect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonToggled() ) );
  disconnect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonToggled() ) );
  disconnect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonToggled() ) );
  disconnect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonToggled() ) );
}


std::string qSlicerLORManualDOFWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORManualDOFWidget);
  
  return "vtkMRMLLinearTransformNode";
}


std::string qSlicerLORManualDOFWidget
::GetCollectModeName()
{
  Q_D(qSlicerLORManualDOFWidget);
  
  return "ManualDOF";
}


void qSlicerLORManualDOFWidget
::enter()
{
}


void qSlicerLORManualDOFWidget
::widgetActivated()
{
  Q_D(qSlicerLORManualDOFWidget);
}


void qSlicerLORManualDOFWidget
::widgetDeactivated()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode != NULL )
  {
    this->LORNode->StopCollecting();
  }
}



void qSlicerLORManualDOFWidget
::onReferenceButtonToggled()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( LORConstants::REFERENCE_STRING );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onPointButtonToggled()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( LORConstants::POINT_STRING );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onLineButtonToggled()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( LORConstants::LINE_STRING );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onPlaneButtonToggled()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( LORConstants::PLANE_STRING );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}



void qSlicerLORManualDOFWidget
::updateWidget()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  this->DisconnectAllButtons();

  if ( this->LORNode->GetCollectionState().compare( LORConstants::REFERENCE_STRING ) )
  {
    d->ReferenceButton->setChecked( true );
  }
  else
  {
    d->ReferenceButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( LORConstants::POINT_STRING ) )
  {
    d->PointButton->setChecked( true );
  }
  else
  {
    d->PointButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( LORConstants::LINE_STRING ) )
  {
    d->LineButton->setChecked( true );
  }
  else
  {
    d->LineButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( LORConstants::PLANE_STRING ) )
  {
    d->PlaneButton->setChecked( true );
  }
  else
  {
    d->PlaneButton->setChecked( false );
  }

  this->ConnectAllButtons();
}
