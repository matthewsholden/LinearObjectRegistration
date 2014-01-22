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
::qSlicerLORManualDOFWidget(QWidget* parentWidget) : Superclass( parentWidget ) , d_ptr( new qSlicerLORManualDOFWidgetPrivate(*this) )
{
}


qSlicerLORManualDOFWidget
::~qSlicerLORManualDOFWidget()
{
}


qSlicerLORManualDOFWidget* qSlicerLORManualDOFWidget
::New()
{
  qSlicerLORManualDOFWidget* newLORManualDOFWidget = new qSlicerLORManualDOFWidget();
  newLORManualDOFWidget->LORNode = NULL;
  newLORManualDOFWidget->setup();
  return newLORManualDOFWidget;
}


void qSlicerLORManualDOFWidget
::setup()
{
  Q_D(qSlicerLORManualDOFWidget);

  d->setupUi(this);
  
  // Use the pressed signal (otherwise we can unpress buttons without clicking them)
  connect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
  connect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
  connect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
  connect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );

  this->updateWidget();  
}


void qSlicerLORManualDOFWidget
::enter()
{
}


void qSlicerLORManualDOFWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORManualDOFWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;
}


void qSlicerLORManualDOFWidget
::onReferenceButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( "Reference" );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onPointButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( "Point" );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onLineButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( "Line" );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}


void qSlicerLORManualDOFWidget
::onPlaneButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( "Plane" );
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

  disconnect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
  disconnect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
  disconnect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
  disconnect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );

  if ( this->LORNode->GetCollectionState().compare( "Reference" ) )
  {
    d->ReferenceButton->setChecked( true );
  }
  else
  {
    d->ReferenceButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( "Point" ) )
  {
    d->PointButton->setChecked( true );
  }
  else
  {
    d->PointButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( "Line" ) )
  {
    d->LineButton->setChecked( true );
  }
  else
  {
    d->LineButton->setChecked( false );
  }

  if ( this->LORNode->GetCollectionState().compare( "Plane" ) )
  {
    d->PlaneButton->setChecked( true );
  }
  else
  {
    d->PlaneButton->setChecked( false );
  }

  connect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
  connect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
  connect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
  connect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );

}
