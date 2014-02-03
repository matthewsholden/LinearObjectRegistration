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
::New()
{
  qSlicerLORManualSegmentationWidget* newLORManualSegmentationWidget = new qSlicerLORManualSegmentationWidget();
  newLORManualSegmentationWidget->LORNode = NULL;
  newLORManualSegmentationWidget->setup();
  return newLORManualSegmentationWidget;
}


void qSlicerLORManualSegmentationWidget
::setup()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  d->setupUi(this);
  
  // Use the pressed signal (otherwise we can unpress buttons without clicking them)
  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );
  
  this->updateWidget();  
}


void qSlicerLORManualSegmentationWidget
::enter()
{
}


void qSlicerLORManualSegmentationWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLORManualSegmentationWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;
}


void qSlicerLORManualSegmentationWidget
::onCollectButtonClicked()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->LORNode->GetCollectionState().compare( "" ) == 0 )
  {
    this->LORNode->StartCollecting( LORConstants::COLLECT_STRING );
  }
  else
  {
    this->LORNode->StopCollecting();
  }
}



void qSlicerLORManualSegmentationWidget
::updateWidget()
{
  Q_D(qSlicerLORManualSegmentationWidget);

  if ( this->LORNode == NULL )
  {
    return;
  }

  disconnect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );

  if ( this->LORNode->GetCollectionState().compare( LORConstants::COLLECT_STRING ) )
  {
    d->CollectButton->setChecked( true );
  }
  else
  {
    d->CollectButton->setChecked( false );
  }

  connect( d->CollectButton, SIGNAL( toggled( bool ) ), this, SLOT( onCollectButtonClicked() ) );

}
