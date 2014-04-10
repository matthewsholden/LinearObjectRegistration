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
#include "qSlicerLORCollectControlsWidget.h"

#include <QtGui>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORCollectControlsWidgetPrivate
  : public Ui_qSlicerLORCollectControlsWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORCollectControlsWidget);
protected:
  qSlicerLORCollectControlsWidget* const q_ptr;

public:
  qSlicerLORCollectControlsWidgetPrivate( qSlicerLORCollectControlsWidget& object);
  ~qSlicerLORCollectControlsWidgetPrivate();
  virtual void setupUi(qSlicerLORCollectControlsWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORCollectControlsWidgetPrivate
::qSlicerLORCollectControlsWidgetPrivate( qSlicerLORCollectControlsWidget& object) : q_ptr(&object)
{
}

qSlicerLORCollectControlsWidgetPrivate
::~qSlicerLORCollectControlsWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORCollectControlsWidgetPrivate
::setupUi(qSlicerLORCollectControlsWidget* widget)
{
  this->Ui_qSlicerLORCollectControlsWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORCollectControlsWidget methods

//-----------------------------------------------------------------------------
qSlicerLORCollectControlsWidget
::qSlicerLORCollectControlsWidget( QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORCollectControlsWidgetPrivate(*this) )
{
  this->setup();
}


qSlicerLORCollectControlsWidget
::qSlicerLORCollectControlsWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORCollectControlsWidgetPrivate(*this) )
{
  this->LORLogic = newLORLogic; // So the scene can be set
  this->initialize();
  this->LORLogic = newLORLogic;
}


qSlicerLORCollectControlsWidget
::~qSlicerLORCollectControlsWidget()
{
}


qSlicerLORCollectControlsWidget* qSlicerLORCollectControlsWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORCollectControlsWidget* newControlsWidget = new qSlicerLORCollectControlsWidget( newLORLogic );
  newControlsWidget->setup();
  return newControlsWidget;
}


void qSlicerLORCollectControlsWidget
::initialize()
{
  Q_D(qSlicerLORCollectControlsWidget);

  this->setMRMLScene( this->LORLogic->GetMRMLScene() );

  this->LORLogic = NULL;
  this->LORNode = NULL;
  this->CollectNode = NULL;
}


void qSlicerLORCollectControlsWidget
::setup()
{
  Q_D(qSlicerLORCollectControlsWidget);

  d->setupUi(this);

  this->updateWidget();  
}


void qSlicerLORCollectControlsWidget
::enter()
{
}


void qSlicerLORCollectControlsWidget
::show()
{
  if ( this->isHidden() )
  {
    this->Superclass::show();
    this->widgetActivated();
  }
}


void qSlicerLORCollectControlsWidget
::hide()
{
  if ( ! this->isHidden() )
  {
    this->widgetDeactivated();
    this->Superclass::hide();
  }
}


void qSlicerLORCollectControlsWidget
::widgetActivated()
{
}


void qSlicerLORCollectControlsWidget
::widgetDeactivated()
{
}



void qSlicerLORCollectControlsWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORCollectControlsWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;
}


void qSlicerLORCollectControlsWidget
::SetAndObserveCollectNode( vtkMRMLNode* newCollectNode )
{
  Q_D(qSlicerLORCollectControlsWidget);
  
  this->qvtkDisconnectAll();
  this->CollectNode = newCollectNode;
  this->qvtkConnect( this->CollectNode, vtkCommand::ModifiedEvent, this, SLOT( updateWidget() ) );
}


std::string qSlicerLORCollectControlsWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORCollectControlsWidget);
  
  return "";
}


std::string qSlicerLORCollectControlsWidget
::GetCollectModeName()
{
  Q_D(qSlicerLORCollectControlsWidget);
  
  return "";
}


void qSlicerLORCollectControlsWidget
::updateWidget()
{
  Q_D(qSlicerLORCollectControlsWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  // Nothing to do
}
