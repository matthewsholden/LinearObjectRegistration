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
#include "qSlicerLORManualSegmentationWidget.h"

#include <QtGui>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORManualSegmentationWidgetPrivate
  : public Ui_qSlicerLORManualSegmentationWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORManualSegmentationWidget);
protected:
  qSlicerLORManualSegmentationWidget* const q_ptr;

public:
  qSlicerLORManualSegmentationWidgetPrivate( qSlicerLORManualSegmentationWidget& object);
  ~qSlicerLORManualSegmentationWidgetPrivate();
  virtual void setupUi(qSlicerLORManualSegmentationWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORManualSegmentationWidgetPrivate
::qSlicerLORManualSegmentationWidgetPrivate( qSlicerLORManualSegmentationWidget& object) : q_ptr(&object)
{
}

qSlicerLORManualSegmentationWidgetPrivate
::~qSlicerLORManualSegmentationWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORManualSegmentationWidgetPrivate
::setupUi(qSlicerLORManualSegmentationWidget* widget)
{
  this->Ui_qSlicerLORManualSegmentationWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORManualSegmentationWidget methods

//-----------------------------------------------------------------------------
qSlicerLORManualSegmentationWidget
::qSlicerLORManualSegmentationWidget(QWidget* parentWidget) : Superclass( parentWidget ) , d_ptr( new qSlicerLORManualSegmentationWidgetPrivate(*this) )
{
}


qSlicerLORManualSegmentationWidget
::~qSlicerLORManualSegmentationWidget()
{
}


qSlicerLORManualSegmentationWidget* qSlicerLORManualSegmentationWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORManualSegmentationWidget* newLORManualSegmentationWidget = new qSlicerLORManualSegmentationWidget();
  newLORManualSegmentationWidget->LORLogic = newLORLogic;
  newLORManualSegmentationWidget->setup();
  return newLORManualSegmentationWidget;
}


void qSlicerLORManualSegmentationWidget
::setup()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  d->setupUi(this);
  this->setMRMLScene( this->LORLogic->GetMRMLScene() );

  this->CollectType = "";

  connect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onTransformNodeChanged() ) );
  
  // Use the pressed signal (otherwise we can unpress buttons without clicking them)
  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );
  
  this->updateWidget();  
}


void qSlicerLORManualSegmentationWidget
::enter()
{
}


vtkMRMLNode* qSlicerLORManualSegmentationWidget
::GetCurrentNode()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  return d->TransformNodeComboBox->currentNode();
}


void qSlicerLORManualSegmentationWidget
::SetCurrentNode( vtkMRMLNode* currentNode )
{
  Q_D(qSlicerLORManualSegmentationWidget);

  vtkMRMLLinearTransformNode* currentTransformNode = vtkMRMLLinearTransformNode::SafeDownCast( currentNode );
  d->TransformNodeComboBox->setCurrentNode( currentTransformNode );
}


void qSlicerLORManualSegmentationWidget
::setCollect( std::string collectType )
{
  Q_D(qSlicerLORManualSegmentationWidget);

  this->CollectType = collectType;

  if ( collectType.compare( "" ) == 0 )
  {
    this->LORLogic->FinalizeActivePositionBuffer();
    // All buttons should be unchecked (since they may become unchecked by another button)
    connect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onTransformNodeChanged() ) );

    d->CollectButton->setChecked( false );

    connect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onTransformNodeChanged() ) );
  }
  else
  {
    this->LORLogic->InitializeActivePositionBuffer( collectType );
    // Individual buttons will be responsible for checking themselves (since they can only become checked on button press)
  }

}



void qSlicerLORManualSegmentationWidget
::onTransformNodeChanged()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  emit transformNodeChanged();

  this->LORLogic->FinalizeActivePositionBuffer();
  this->LORLogic->ObserveTransformNode( d->TransformNodeComboBox->currentNode() );

  this->updateWidget();
}


void qSlicerLORManualSegmentationWidget
::onCollectButtonClicked()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->CollectType.compare( "" ) == 0 && this->GetCurrentNode() != NULL )
  {
    this->setCollect( "Collect" );
    d->CollectButton->setChecked( true );
  }
  else
  {
    this->setCollect( "" );
  }
}



void qSlicerLORManualSegmentationWidget
::updateWidget()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  disconnect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );

  if ( this->CollectType.compare( "Collect" ) == 0 && this->GetCurrentNode() != NULL )
  {
    d->CollectButton->setChecked( true );
  }
  else
  {
    d->CollectButton->setChecked( false );
  }

  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );

}
