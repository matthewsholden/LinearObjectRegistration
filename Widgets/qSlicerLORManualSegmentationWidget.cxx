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
::qSlicerLORManualSegmentationWidget( QWidget* parentWidget ) : Superclass( parentWidget ) , d_ptr( new qSlicerLORManualSegmentationWidgetPrivate(*this) )
{
}


qSlicerLORManualSegmentationWidget
::qSlicerLORManualSegmentationWidget( vtkSlicerLinearObjectRegistrationLogic* newLORLogic, QWidget* parentWidget ) : Superclass( newLORLogic, parentWidget ) , d_ptr( new qSlicerLORManualSegmentationWidgetPrivate(*this) )
{
}


qSlicerLORManualSegmentationWidget
::~qSlicerLORManualSegmentationWidget()
{
}


qSlicerLORManualSegmentationWidget* qSlicerLORManualSegmentationWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLORManualSegmentationWidget* newControlsWidget = new qSlicerLORManualSegmentationWidget( newLORLogic );
  newControlsWidget->setup();
  return newControlsWidget;
}


void qSlicerLORManualSegmentationWidget
::setup()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  d->setupUi(this);

  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonToggled() ) );
  
  this->updateWidget();
}


std::string qSlicerLORManualSegmentationWidget
::GetCollectNodeType()
{
  Q_D(qSlicerLORManualSegmentationWidget);
  
  return "vtkMRMLLinearTransformNode";
}


std::string qSlicerLORManualSegmentationWidget
::GetCollectModeName()
{
  Q_D(qSlicerLORManualSegmentationWidget);
  
  return LORConstants::MANUAL_SEGMENTATION_COLLECT_MODE;
}


void qSlicerLORManualSegmentationWidget
::enter()
{
}


void qSlicerLORManualSegmentationWidget
::widgetActivated()
{
  Q_D(qSlicerLORManualSegmentationWidget);
  this->updateWidget();
}


void qSlicerLORManualSegmentationWidget
::widgetDeactivated()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->LORNode != NULL )
  {
    this->LORNode->StopCollecting();
  }
  this->updateWidget();
}


void qSlicerLORManualSegmentationWidget
::SetAndObserveCollectNode( vtkMRMLNode* newCollectNode )
{
  Q_D(qSlicerLORManualSegmentationWidget);

  this->widgetDeactivated();

  this->Superclass::SetAndObserveCollectNode( newCollectNode );

  this->qvtkConnect( this->CollectNode, vtkMRMLLinearTransformNode::TransformModifiedEvent, this, SLOT( updateWidget() ) );

  this->widgetActivated();
}


void qSlicerLORManualSegmentationWidget
::onCollectButtonToggled()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->LORNode->GetCollectState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( this->CollectNode, LORConstants::UNKNOWNDOF_COLLECT_STATE );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
  this->updateWidget();
}



void qSlicerLORManualSegmentationWidget
::updateWidget()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  disconnect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonToggled() ) );

  if ( this->LORNode->GetCollectState().compare( LORConstants::UNKNOWNDOF_COLLECT_STATE ) == 0 )
  {
    d->CollectButton->setChecked( true );
    std::stringstream typeString;
    typeString << "Collect (";
    typeString << LORConstants::DOF_TO_STRING( this->LORNode->GetActivePositionBuffer()->GetDOF( this->LORNode->GetNoiseThreshold() ) );
    typeString << ")";
    d->CollectButton->setText( QString::fromStdString( typeString.str() ) );
  }
  else
  {
    d->CollectButton->setChecked( false );
    std::stringstream typeString;
    typeString << "Collect ()";
    d->CollectButton->setText( QString::fromStdString( typeString.str() ) );
  }

  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonToggled() ) );

}
