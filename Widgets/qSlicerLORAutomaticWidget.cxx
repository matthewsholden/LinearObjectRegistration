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
#include "qSlicerLORAutomaticWidget.h"

#include <QtGui>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLORAutomaticWidgetPrivate
  : public Ui_qSlicerLORAutomaticWidget
{
  Q_DECLARE_PUBLIC(qSlicerLORAutomaticWidget);
protected:
  qSlicerLORAutomaticWidget* const q_ptr;

public:
  qSlicerLORAutomaticWidgetPrivate( qSlicerLORAutomaticWidget& object);
  ~qSlicerLORAutomaticWidgetPrivate();
  virtual void setupUi(qSlicerLORAutomaticWidget*);
};

// --------------------------------------------------------------------------
qSlicerLORAutomaticWidgetPrivate
::qSlicerLORAutomaticWidgetPrivate( qSlicerLORAutomaticWidget& object) : q_ptr(&object)
{
}

qSlicerLORAutomaticWidgetPrivate
::~qSlicerLORAutomaticWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLORAutomaticWidgetPrivate
::setupUi(qSlicerLORAutomaticWidget* widget)
{
  this->Ui_qSlicerLORAutomaticWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLORAutomaticWidget methods

//-----------------------------------------------------------------------------
qSlicerLORAutomaticWidget
::qSlicerLORAutomaticWidget( QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORAutomaticWidgetPrivate(*this) )
{
}


qSlicerLORAutomaticWidget
::qSlicerLORAutomaticWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget ) : Superclass( newLORLogic, parentWidget ) , d_ptr( new qSlicerLORAutomaticWidgetPrivate(*this) )
{
}


qSlicerLORAutomaticWidget
::~qSlicerLORAutomaticWidget()
{
}


qSlicerLORAutomaticWidget* qSlicerLORAutomaticWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORAutomaticWidget* newControlsWidget = new qSlicerLORAutomaticWidget( newLORLogic );
  newControlsWidget->setup();
  return newControlsWidget;
}


void qSlicerLORAutomaticWidget
::setup()
{
  Q_D(qSlicerLORAutomaticWidget);

  d->setupUi(this);
  
  this->updateWidget();  
}


std::string qSlicerLORAutomaticWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORAutomaticWidget);
  
  return "vtkMRMLLinearTransformNode";
}


std::string qSlicerLORAutomaticWidget
::GetCollectModeName()
{
  Q_D(qSlicerLORAutomaticWidget);
  
  return LORConstants::AUTOMATIC_COLLECT_MODE;
}


void qSlicerLORAutomaticWidget
::enter()
{
}


void qSlicerLORAutomaticWidget
::widgetActivated()
{
  Q_D(qSlicerLORAutomaticWidget);

  if ( this->LORNode != NULL && this->LORNode->GetCollectState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( this->CollectNode, LORConstants::UNKNOWNDOF_COLLECT_STATE );
  }
  this->updateWidget();
}


void qSlicerLORAutomaticWidget
::widgetDeactivated()
{
  Q_D(qSlicerLORAutomaticWidget);

  if ( this->LORNode != NULL && this->LORNode->GetCollectState().compare( LORConstants::UNKNOWNDOF_COLLECT_STATE ) == 0 )
  {
    this->LORNode->StopCollecting();
  }
  this->updateWidget();
}


void qSlicerLORAutomaticWidget
::SetAndObserveCollectNode( vtkMRMLNode* newCollectNode )
{
  Q_D(qSlicerLORAutomaticWidget);

  this->widgetDeactivated();

  this->Superclass::SetAndObserveCollectNode( newCollectNode );

  this->qvtkConnect( this->CollectNode, vtkMRMLLinearTransformNode::TransformModifiedEvent, this, SLOT( updateWidget() ) );

  this->widgetActivated();
}



void qSlicerLORAutomaticWidget
::updateWidget()
{
  Q_D(qSlicerLORAutomaticWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  d->CollectProgressBar->setValue( int( 100 * this->LORNode->GetActivePositionBuffer()->Size() / this->LORNode->GetMinimumCollectionPositions() ) );

  std::stringstream typeString;
  typeString << "(";
  typeString << LORConstants::DOF_TO_STRING( this->LORNode->GetActivePositionBuffer()->GetDOF( this->LORNode->GetNoiseThreshold() ) );
  typeString << ")";
  d->LinearObjectTypeLabel->setText( QString::fromStdString( typeString.str() ) );
}
