
// Qt includes
#include <QDebug>
#include <QtCore>
#include <QtGui>
#include <QTimer>

// SlicerQt includes
#include "qSlicerLinearObjectRegistrationModuleWidget.h"
#include "ui_qSlicerLinearObjectRegistrationModule.h"

#include "qSlicerLinearObjectCollectionWidget.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"
#include "qSlicerLORManualDOFWidget.h"

#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLinearObjectRegistrationModuleWidgetPrivate: public Ui_qSlicerLinearObjectRegistrationModule
{
  Q_DECLARE_PUBLIC( qSlicerLinearObjectRegistrationModuleWidget );
protected:
  qSlicerLinearObjectRegistrationModuleWidget* const q_ptr;
public:
  qSlicerLinearObjectRegistrationModuleWidgetPrivate( qSlicerLinearObjectRegistrationModuleWidget& object );

  vtkSlicerLinearObjectRegistrationLogic* logic() const;

  // Add embedded widgets here
  qSlicerLinearObjectCollectionWidget* FromCollectionWidget;
  qSlicerLinearObjectCollectionWidget* ToCollectionWidget;
  qSlicerLORManualDOFWidget* ManualDOFWidget;
};




//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModuleWidgetPrivate methods



qSlicerLinearObjectRegistrationModuleWidgetPrivate
::qSlicerLinearObjectRegistrationModuleWidgetPrivate( qSlicerLinearObjectRegistrationModuleWidget& object )
 : q_ptr( &object )
{
}


//-----------------------------------------------------------------------------
vtkSlicerLinearObjectRegistrationLogic*
qSlicerLinearObjectRegistrationModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerLinearObjectRegistrationModuleWidget );
  return vtkSlicerLinearObjectRegistrationLogic::SafeDownCast( q->logic() );
}





//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModuleWidget methods


//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModuleWidget::qSlicerLinearObjectRegistrationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLinearObjectRegistrationModuleWidgetPrivate( *this ) )
{
}


//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModuleWidget::~qSlicerLinearObjectRegistrationModuleWidget()
{
}


void qSlicerLinearObjectRegistrationModuleWidget
::OnTransformNodeSelected( vtkMRMLNode* newRegistrationTransformNode )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );
      
  d->logic()->SetOutputTransform( vtkMRMLLinearTransformNode::SafeDownCast( newRegistrationTransformNode ) );
  
  this->UpdateGUI();
}


void qSlicerLinearObjectRegistrationModuleWidget
::OnRegisterButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  vtkMRMLLORLinearObjectCollectionNode* fromCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->FromCollectionWidget->GetCurrentNode() );
  vtkMRMLLORLinearObjectCollectionNode* toCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->ToCollectionWidget->GetCurrentNode() );
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( d->OutputNodeComboBox->currentNode() );
      
  d->logic()->Register( fromCollection, toCollection, transformNode );
  
  this->UpdateGUI();
}


void qSlicerLinearObjectRegistrationModuleWidget
::OnMatchButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  vtkMRMLLORLinearObjectCollectionNode* fromCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->FromCollectionWidget->GetCurrentNode() );
  vtkMRMLLORLinearObjectCollectionNode* toCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->ToCollectionWidget->GetCurrentNode() );

  d->logic()->MatchCollections( fromCollection, toCollection );
  
  this->UpdateGUI();
}


void qSlicerLinearObjectRegistrationModuleWidget
::OnFromMatchRequested( int matchIndex )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  this->FromMatchState = matchIndex;

  if ( this->ToMatchState >= 0 )
  {
    vtkMRMLLORLinearObjectCollectionNode* currentCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->ToCollectionWidget->GetCurrentNode() );
    currentCollection->Swap( this->ToMatchState, this->FromMatchState );

    this->ToMatchState = -1;
    this->FromMatchState = -1;
  }

}


void qSlicerLinearObjectRegistrationModuleWidget
::OnToMatchRequested( int matchIndex )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  this->ToMatchState = matchIndex;

  if ( this->FromMatchState >= 0 )
  {
    vtkMRMLLORLinearObjectCollectionNode* currentCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->FromCollectionWidget->GetCurrentNode() );
    currentCollection->Swap( this->FromMatchState, this->ToMatchState );

    this->FromMatchState = -1;
    this->ToMatchState = -1;
  }

}


void
qSlicerLinearObjectRegistrationModuleWidget
::setup()
{
  Q_D(qSlicerLinearObjectRegistrationModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Embed widgets here
  d->FromCollectionWidget = qSlicerLinearObjectCollectionWidget::New( d->logic() );
  d->FromGroupBox->layout()->addWidget( d->FromCollectionWidget );

  d->ToCollectionWidget = qSlicerLinearObjectCollectionWidget::New( d->logic() );
  d->ToGroupBox->layout()->addWidget( d->ToCollectionWidget );

  d->ManualDOFWidget = qSlicerLORManualDOFWidget::New( d->logic() );
  d->CollectionGroupBox->layout()->addWidget( d->ManualDOFWidget );

  this->FromMatchState = -1;
  this->ToMatchState = -1;
  
  connect( d->OutputNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( OnTransformNodeSelected( vtkMRMLNode* ) ) );

  connect( d->RegisterButton, SIGNAL( clicked() ), this, SLOT( OnRegisterButtonClicked() ) );

  connect( d->MatchButton, SIGNAL( clicked() ), this, SLOT( OnMatchButtonClicked() ) );

  connect( d->FromCollectionWidget, SIGNAL( matchRequested( int ) ), this, SLOT( OnFromMatchRequested( int ) ) );
  connect( d->ToCollectionWidget, SIGNAL( matchRequested( int ) ), this, SLOT( OnToMatchRequested( int ) ) );
}



void qSlicerLinearObjectRegistrationModuleWidget
::UpdateGUI()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  std::stringstream statusString;
  statusString << "Status: ";
  statusString << d->logic()->GetOutputMessage();
  d->StatusLabel->setText( QString::fromStdString( statusString.str() ) );
  
}

