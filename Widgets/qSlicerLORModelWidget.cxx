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
::qSlicerLORModelWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget) : Superclass( newLORLogic, parentWidget ) , d_ptr( new qSlicerLORModelWidgetPrivate(*this) )
{
}


qSlicerLORModelWidget
::~qSlicerLORModelWidget()
{
}


qSlicerLORModelWidget* qSlicerLORModelWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  return new qSlicerLORModelWidget( newLORLogic );
}


void qSlicerLORModelWidget
::setup()
{
  Q_D(qSlicerLORModelWidget);

  d->setupUi(this);
  this->setMRMLScene( this->LORLogic->GetMRMLScene() );
  
  this->ConnectAllButtons();

  this->updateWidget();  
}


void qSlicerLORModelWidget
::ConnectAllButtons()
{
  Q_D(qSlicerLORModelWidget);
  
  connect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonToggled() ) );
  connect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonToggled() ) );
  connect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonToggled() ) );
  connect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonToggled() ) );
}

void qSlicerLORModelWidget
::DisconnectAllButtons()
{
  Q_D(qSlicerLORModelWidget);
  
  disconnect( d->ReferenceButton, SIGNAL( toggled( bool ) ), this, SLOT( onReferenceButtonToggled() ) );
  disconnect( d->PointButton, SIGNAL( toggled( bool ) ), this, SLOT( onPointButtonToggled() ) );
  disconnect( d->LineButton, SIGNAL( toggled( bool ) ), this, SLOT( onLineButtonToggled() ) );
  disconnect( d->PlaneButton, SIGNAL( toggled( bool ) ), this, SLOT( onPlaneButtonToggled() ) );
}


std::string qSlicerLORModelWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORModelWidget);
  
  return "vtkMRMLModelNode";
}


void qSlicerLORModelWidget
::enter()
{
}


void qSlicerLORModelWidget
::widgetActivated()
{
}


void qSlicerLORModelWidget
::widgetDeactivated()
{
  this->disconnectMarkupsObservers();
}


void qSlicerLORModelWidget
::disconnectMarkupsObservers( std::string checkString )
{
  Q_D(qSlicerLORModelWidget);

  this->qvtkDisconnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onReferenceFiducialDropped() ) );
  this->qvtkDisconnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onPointFiducialDropped() ) );
  this->qvtkDisconnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onLineFiducialDropped() ) );
  this->qvtkDisconnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onPlaneFiducialDropped() ) );

  vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );
  this->qvtkDisconnect( interactionNode, vtkMRMLInteractionNode::InteractionModeChangedEvent, this, SLOT( disconnectMarkupsObservers() ) );
  interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::ViewTransform );

  this->DisconnectAllButtons();

  if ( checkString.compare( LORConstants::REFERENCE_STRING ) != 0 )
  {
    d->ReferenceButton->setChecked( Qt::Unchecked );
  }
  if ( checkString.compare( LORConstants::POINT_STRING ) != 0 )
  {
    d->PointButton->setChecked( Qt::Unchecked );
  }
  if ( checkString.compare( LORConstants::LINE_STRING ) != 0 )
  {
    d->LineButton->setChecked( Qt::Unchecked );
  }
  if ( checkString.compare( LORConstants::PLANE_STRING ) != 0 )
  {
    d->PlaneButton->setChecked( Qt::Unchecked );
  }

  this->ConnectAllButtons();
}


// Note: Observe the markups PointModifiedEvent - this is when the markups value is changed to the correct coordinates

void qSlicerLORModelWidget
::onReferenceButtonToggled()
{
  Q_D(qSlicerLORModelWidget);

  // Observe the active markups fiducial node and start place mode
  vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );

  if ( interactionNode == NULL || ! d->ReferenceButton->isChecked() )
  {
    this->disconnectMarkupsObservers();
    return;
  }

  this->disconnectMarkupsObservers( LORConstants::REFERENCE_STRING );
  interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::Place );
  this->qvtkConnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onReferenceFiducialDropped() ) );
  this->qvtkConnect( interactionNode, vtkMRMLInteractionNode::InteractionModeChangedEvent, this, SLOT( disconnectMarkupsObservers() ) );
}



void qSlicerLORModelWidget
::onReferenceFiducialDropped()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::REFERENCE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelReference( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer(), this->LORNode );

  // Stop collecting
  this->LORNode->StopCollecting();

  this->disconnectMarkupsObservers();
}


void qSlicerLORModelWidget
::onPointButtonToggled()
{
  Q_D(qSlicerLORModelWidget);

  // Observe the active markups fiducial node and start place mode
  vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );

  if ( interactionNode == NULL || ! d->PointButton->isChecked() )
  {
    this->disconnectMarkupsObservers();
    return;
  }

  this->disconnectMarkupsObservers( LORConstants::POINT_STRING );
  interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::Place );
  this->qvtkConnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onPointFiducialDropped() ) );
  this->qvtkConnect( interactionNode, vtkMRMLInteractionNode::InteractionModeChangedEvent, this, SLOT( disconnectMarkupsObservers() ) );
}


void qSlicerLORModelWidget
::onPointFiducialDropped()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::POINT_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelPoint( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer(), this->LORNode );

  // Stop collecting
  this->LORNode->StopCollecting();

  this->disconnectMarkupsObservers();
}


void qSlicerLORModelWidget
::onLineButtonToggled()
{
  Q_D(qSlicerLORModelWidget);

  // Observe the active markups fiducial node and start place mode
  vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );

  if ( interactionNode == NULL || ! d->LineButton->isChecked() )
  {
    this->disconnectMarkupsObservers();
    return;
  }

  this->disconnectMarkupsObservers( LORConstants::LINE_STRING );
  interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::Place );
  this->qvtkConnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onLineFiducialDropped() ) );
  this->qvtkConnect( interactionNode, vtkMRMLInteractionNode::InteractionModeChangedEvent, this, SLOT( disconnectMarkupsObservers() ) );
}


void qSlicerLORModelWidget
::onLineFiducialDropped()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::LINE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelLine( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer(), this->LORNode );

  // Stop collecting
  this->LORNode->StopCollecting();

  this->disconnectMarkupsObservers();
}


void qSlicerLORModelWidget
::onPlaneButtonToggled()
{
  Q_D(qSlicerLORModelWidget);

  // Observe the active markups fiducial node and start place mode
  vtkMRMLInteractionNode* interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );

  if ( interactionNode == NULL || ! d->PlaneButton->isChecked() )
  {
    this->disconnectMarkupsObservers();
    return;
  }

  this->disconnectMarkupsObservers( LORConstants::PLANE_STRING );
  interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::Place );
  this->qvtkConnect( this->LORLogic->GetActiveMarkupsNode(), vtkMRMLMarkupsNode::PointModifiedEvent, this, SLOT( onPlaneFiducialDropped() ) );
  this->qvtkConnect( interactionNode, vtkMRMLInteractionNode::InteractionModeChangedEvent, this, SLOT( disconnectMarkupsObservers() ) );
}


void qSlicerLORModelWidget
::onPlaneFiducialDropped()
{
  Q_D(qSlicerLORModelWidget);

  // Start collecting
  this->LORNode->StartCollecting( LORConstants::PLANE_STRING );

  // Add the positions to the active position buffer (based on selected model)
  this->LORLogic->CreateModelPlane( d->ModelNodeComboBox->currentNode(), this->LORNode->GetActivePositionBuffer(), this->LORNode );

  // Stop collecting
  this->LORNode->StopCollecting();

  this->disconnectMarkupsObservers();
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
