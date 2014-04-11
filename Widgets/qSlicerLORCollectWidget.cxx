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
#include "qSlicerLORCollectWidget.h"

#include <QtGui>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORCollectWidgetPrivate
  : public Ui_qSlicerLORCollectWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORCollectWidget);
protected:
  qSlicerLORCollectWidget* const q_ptr;

public:
  qSlicerLORCollectWidgetPrivate( qSlicerLORCollectWidget& object);
  ~qSlicerLORCollectWidgetPrivate();
  virtual void setupUi(qSlicerLORCollectWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORCollectWidgetPrivate
::qSlicerLORCollectWidgetPrivate( qSlicerLORCollectWidget& object) : q_ptr(&object)
{
}

qSlicerLORCollectWidgetPrivate
::~qSlicerLORCollectWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORCollectWidgetPrivate
::setupUi(qSlicerLORCollectWidget* widget)
{
  this->Ui_qSlicerLORCollectWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORCollectWidget methods

//-----------------------------------------------------------------------------
qSlicerLORCollectWidget
::qSlicerLORCollectWidget( QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORCollectWidgetPrivate(*this) )
{
  this->setup();
}


qSlicerLORCollectWidget
::qSlicerLORCollectWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORCollectWidgetPrivate(*this) )
{
  this->LORLogic = newLORLogic; // So the scene can be set
  this->setup();
  this->LORLogic = newLORLogic;
}


qSlicerLORCollectWidget
::~qSlicerLORCollectWidget()
{
}


qSlicerLORCollectWidget* qSlicerLORCollectWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  return new qSlicerLORCollectWidget( newLORLogic );
}


void qSlicerLORCollectWidget
::setup()
{
  Q_D(qSlicerLORCollectWidget);

  d->setupUi(this);
  this->setMRMLScene( this->LORLogic->GetMRMLScene() );

  this->LORLogic = NULL;
  this->LORNode = NULL;
  this->ControlsWidget = NULL;
  
  // Observe the collect node combo box
  connect( d->CollectNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( collectNodeChanged() ) );

  this->updateWidget();  
}


void qSlicerLORCollectWidget
::enter()
{
}

void qSlicerLORCollectWidget
::show()
{
  Q_D(qSlicerLORCollectWidget);

  if ( this->isHidden() )
  {
    this->Superclass::show();
    this->widgetActivated();
  }
  if ( this->GetControlsWidget()->GetCollectNodeType().compare( "" ) == 0 )
  {
    d->CollectNodeComboBox->hide();
  }
}


void qSlicerLORCollectWidget
::hide()
{
  Q_D(qSlicerLORCollectWidget);

  if ( ! this->isHidden() )
  {
    this->widgetDeactivated();
    this->Superclass::hide();
  }
}

void qSlicerLORCollectWidget
::widgetActivated()
{
  // Activate the controls widget
  if ( this->ControlsWidget != NULL )
  {
    this->ControlsWidget->widgetActivated();
  }
}


void qSlicerLORCollectWidget
::widgetDeactivated()
{
  // Deactivate the controls widget
  if ( this->ControlsWidget != NULL )
  {
    this->ControlsWidget->widgetDeactivated();
  }
}


qSlicerLORCollectControlsWidget* qSlicerLORCollectWidget
::GetControlsWidget()
{
  return this->ControlsWidget;
}


void qSlicerLORCollectWidget
::SetControlsWidget( qSlicerLORCollectControlsWidget* newControlsWidget )
{
  Q_D(qSlicerLORCollectWidget);

  // Add the widget
  d->CollectControlsLayout->removeWidget( this->ControlsWidget );
  d->CollectControlsLayout->addWidget( newControlsWidget );

  this->ControlsWidget = newControlsWidget;

  // Propagate information down to the controls widget
  newControlsWidget->SetLORNode( this->LORNode );

  // Propagate information up from the controls widget
  QStringList nodeTypes;
  nodeTypes << QString::fromStdString( this->ControlsWidget->GetCollectNodeType() );
  d->CollectNodeComboBox->setNodeTypes( nodeTypes );
}



void qSlicerLORCollectWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORCollectWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;

  if ( this->ControlsWidget != NULL )
  {
    this->ControlsWidget->SetLORNode( newLORNode );
  }
}


void qSlicerLORCollectWidget
::SetCollectNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORCollectWidget);

  vtkMRMLNode* newCollectNode = vtkMRMLNode::SafeDownCast( newNode );
  if ( newCollectNode == NULL )
  {
    return;
  }

  // This will cause the collectNodeChanged slot to be called automatically (if there is a change in node)
  d->CollectNodeComboBox->setCurrentNode( newCollectNode );
}


void qSlicerLORCollectWidget
::collectNodeChanged()
{
  Q_D(qSlicerLORCollectWidget);

  vtkMRMLNode* currentNode = d->CollectNodeComboBox->currentNode();

  this->ControlsWidget->SetAndObserveCollectNode( currentNode );

  if ( this->LORNode != NULL )
  {
    this->LORNode->SetCollectTypeNode( currentNode );
  }
}


void qSlicerLORCollectWidget
::updateWidget()
{
  Q_D(qSlicerLORCollectWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  // Nothing to do
}
