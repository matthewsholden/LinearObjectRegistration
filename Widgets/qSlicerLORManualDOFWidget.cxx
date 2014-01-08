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


const int LINEAROBJECT_NAME_COLUMN = 0;
const int LINEAROBJECT_TYPE_COLUMN = 1;
const int LINEAROBJECT_COLUMNS = 2;


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
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORManualDOFWidget* newLORManualDOFWidget = new qSlicerLORManualDOFWidget();
  newLORManualDOFWidget->LORLogic = newLORLogic;
  newLORManualDOFWidget->setup();
  return newLORManualDOFWidget;
}


void qSlicerLORManualDOFWidget
::setup()
{
  Q_D(qSlicerLORManualDOFWidget);

  d->setupUi(this);
  this->setMRMLScene( this->LORLogic->GetMRMLScene() );

  this->CollectType = "";

  connect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onTransformNodeChanged() ) );
  
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


vtkMRMLNode* qSlicerLORManualDOFWidget
::GetCurrentNode()
{
  Q_D(qSlicerLORManualDOFWidget);

  return d->TransformNodeComboBox->currentNode();
}


void qSlicerLORManualDOFWidget
::SetCurrentNode( vtkMRMLNode* currentNode )
{
  Q_D(qSlicerLORManualDOFWidget);

  vtkMRMLLinearTransformNode* currentTransformNode = vtkMRMLLinearTransformNode::SafeDownCast( currentNode );
  d->TransformNodeComboBox->setCurrentNode( currentTransformNode );
}


void qSlicerLORManualDOFWidget
::setCollect( std::string collectType )
{
  Q_D(qSlicerLORManualDOFWidget);

  this->CollectType = collectType;

  if ( collectType.compare( "" ) == 0 )
  {
    this->LORLogic->FinalizeActivePositionBuffer();
    // All buttons should be unchecked (since they may become unchecked by another button)
    disconnect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
    disconnect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
    disconnect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
    disconnect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );

    d->ReferenceButton->setChecked( false );
    d->PointButton->setChecked( false );
    d->LineButton->setChecked( false );
    d->PlaneButton->setChecked( false );

    connect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
    connect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
    connect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
    connect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );
  }
  else
  {
    this->LORLogic->InitializeActivePositionBuffer( collectType );
    // Individual buttons will be responsible for checking themselves (since they can only become checked on button press)
  }

}



void qSlicerLORManualDOFWidget
::onTransformNodeChanged()
{
  Q_D(qSlicerLORManualDOFWidget);


  this->LORLogic->FinalizeActivePositionBuffer();
  this->LORLogic->ObserveTransformNode( d->TransformNodeComboBox->currentNode() );

  this->updateWidget();
}


void qSlicerLORManualDOFWidget
::onReferenceButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->CollectType.compare( "" ) == 0 && this->GetCurrentNode() != NULL )
  {
    this->setCollect( "Reference" );
    d->ReferenceButton->setChecked( true );
  }
  else
  {
    this->setCollect( "" );
  }
}


void qSlicerLORManualDOFWidget
::onPointButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->CollectType.compare( "" ) == 0 && this->GetCurrentNode() != NULL )
  {
    this->setCollect( "Point" );
    d->PointButton->setChecked( true );
  }
  else
  {
    this->setCollect( "" );
  }
}


void qSlicerLORManualDOFWidget
::onLineButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->CollectType.compare( "" ) == 0 && this->GetCurrentNode() != NULL )
  {
    this->setCollect( "Line" );
    d->LineButton->setChecked( true );
  }
  else
  {
    this->setCollect( "" );
  }
}


void qSlicerLORManualDOFWidget
::onPlaneButtonClicked()
{
  Q_D(qSlicerLORManualDOFWidget);

  if ( this->CollectType.compare( "" ) == 0 && this->GetCurrentNode() != NULL )
  {
    this->setCollect( "Plane" );
    d->PlaneButton->setChecked( true );
  }
  else
  {
    this->setCollect( "" );
  }
}



void qSlicerLORManualDOFWidget
::updateWidget()
{
  Q_D(qSlicerLORManualDOFWidget);

  disconnect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonClicked() ) );
  disconnect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonClicked() ) );
  disconnect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonClicked() ) );
  disconnect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonClicked() ) );

  if ( this->CollectType == "Reference" && this->GetCurrentNode() != NULL )
  {
    d->ReferenceButton->setChecked( true );
  }
  else
  {
    d->ReferenceButton->setChecked( false );
  }

  if ( this->CollectType == "Point" && this->GetCurrentNode() != NULL )
  {
    d->PointButton->setChecked( true );
  }
  else
  {
    d->PointButton->setChecked( false );
  }

  if ( this->CollectType == "Line" && this->GetCurrentNode() != NULL )
  {
    d->LineButton->setChecked( true );
  }
  else
  {
    d->LineButton->setChecked( false );
  }

  if ( this->CollectType == "Plane" && this->GetCurrentNode() != NULL )
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
