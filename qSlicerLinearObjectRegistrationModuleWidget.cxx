
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
#include "qSlicerLORManualSegmentationWidget.h"
#include "qSlicerLORAutomaticWidget.h"

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
  qSlicerLORManualSegmentationWidget* ManualSegmentationWidget;
  qSlicerLORAutomaticWidget* AutomaticWidget;
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
::OnFromMatchRequested( int matchIndex )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  this->FromMatchState = matchIndex;

  if ( this->ToMatchState >= 0 )
  {
    // Node in scene - no smart pointer
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
    // Node in scene - no smart pointer
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
  d->FromCollectionWidget->SetNodeBaseName( "FromLinearObjects" );
  d->FromGroupBox->layout()->addWidget( d->FromCollectionWidget );

  d->ToCollectionWidget = qSlicerLinearObjectCollectionWidget::New( d->logic() );
  d->ToCollectionWidget->SetNodeBaseName( "ToLinearObjects" );
  d->ToGroupBox->layout()->addWidget( d->ToCollectionWidget );

  d->ManualDOFWidget = qSlicerLORManualDOFWidget::New();
  d->CollectionGroupBox->layout()->addWidget( d->ManualDOFWidget );

  d->ManualSegmentationWidget = qSlicerLORManualSegmentationWidget::New();
  d->CollectionGroupBox->layout()->addWidget( d->ManualSegmentationWidget );

  d->AutomaticWidget = qSlicerLORAutomaticWidget::New();
  d->CollectionGroupBox->layout()->addWidget( d->AutomaticWidget );

  this->FromMatchState = -1;
  this->ToMatchState = -1;

  // If the mrml node changes, update from the mrml node 
  connect( d->ModuleNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateFromMRMLNode() ) );
  // Otherwise, we will update from the logic
  this->qvtkConnect( d->logic(), vtkCommand::ModifiedEvent, this, SLOT( UpdateFromMRMLNode() ) );

  // Make connections to update the mrml from the widget
  this->ConnectWidgets();  

  // Various other connections
  connect( d->FromCollectionWidget, SIGNAL( matchRequested( int ) ), this, SLOT( OnFromMatchRequested( int ) ) );
  connect( d->ToCollectionWidget, SIGNAL( matchRequested( int ) ), this, SLOT( OnToMatchRequested( int ) ) );
}


void qSlicerLinearObjectRegistrationModuleWidget
::ConnectWidgets()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  connect( d->OutputNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->ManualDOFRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->ManualSegmentationRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->AutomaticRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->AutomaticMatchCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->FromCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->ToCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
}


void qSlicerLinearObjectRegistrationModuleWidget
::DisconnectWidgets()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  disconnect( d->OutputNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->ManualDOFRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->ManualSegmentationRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->AutomaticRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->AutomaticMatchCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );

  disconnect( d->TransformNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );

  disconnect( d->FromCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->ToCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
}


void qSlicerLinearObjectRegistrationModuleWidget
::UpdateToMRMLNode()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  // Node in scene - no smart pointer
  vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );

  if ( linearObjectRegistrationNode == NULL )
  {
    return;
  }

  this->qvtkBlockAll( true );

  if ( d->TransformNodeComboBox->currentNode() == NULL )
  {
    linearObjectRegistrationNode->SetCollectTransformID( "", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetCollectTransformID( d->TransformNodeComboBox->currentNode()->GetID(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  if ( d->OutputNodeComboBox->currentNode() == NULL )
  {
    linearObjectRegistrationNode->SetOutputTransformID( "", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetOutputTransformID( d->OutputNodeComboBox->currentNode()->GetID(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  if ( d->FromCollectionWidget->GetCurrentNode() == NULL )
  {
    linearObjectRegistrationNode->SetFromCollectionID( "", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetFromCollectionID( d->FromCollectionWidget->GetCurrentNode()->GetID(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  if ( d->ToCollectionWidget->GetCurrentNode() == NULL )
  {
    linearObjectRegistrationNode->SetToCollectionID( "", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetToCollectionID( d->ToCollectionWidget->GetCurrentNode()->GetID(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  if ( d->ManualDOFRadioButton->isChecked() )
  {
    linearObjectRegistrationNode->SetCollectionMode( "ManualDOF", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  if ( d->ManualSegmentationRadioButton->isChecked() )
  {
    linearObjectRegistrationNode->SetCollectionMode( "ManualSegmentation", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  if ( d->AutomaticRadioButton->isChecked() )
  {
    linearObjectRegistrationNode->SetCollectionMode( "Automatic", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  if ( d->AutomaticMatchCheckBox->isChecked() )
  {
    linearObjectRegistrationNode->SetAutomaticMatch( "True", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetAutomaticMatch( "False", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  this->qvtkBlockAll( false );

  // The modified event will be blocked... Now allow it to happen
  d->ModuleNodeComboBox->currentNode()->Modified();
  this->UpdateFromMRMLNode();
}


void qSlicerLinearObjectRegistrationModuleWidget
::UpdateFromMRMLNode()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  // Node in scene - no smart pointer
  vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );

  if ( linearObjectRegistrationNode == NULL )
  {
    d->OutputNodeComboBox->setEnabled( false );
    d->ManualDOFRadioButton->setEnabled( false );
    d->ManualSegmentationRadioButton->setEnabled( false );
    d->AutomaticRadioButton->setEnabled( false );
    d->AutomaticMatchCheckBox->setEnabled( false );
    d->ManualDOFWidget->setEnabled( false );
    d->ManualSegmentationWidget->setEnabled( false );
    d->AutomaticWidget->setEnabled( false );
    d->FromCollectionWidget->setEnabled( false );
    d->ToCollectionWidget->setEnabled( false );
    d->StatusLabel->setText( "No Linear Object Registration module node selected." );
    return;
  }

  d->OutputNodeComboBox->setEnabled( true );
  d->ManualDOFRadioButton->setEnabled( true );
  d->ManualSegmentationRadioButton->setEnabled( true );
  d->AutomaticRadioButton->setEnabled( true );
  d->AutomaticMatchCheckBox->setEnabled( true );
  d->ManualDOFWidget->setEnabled( true );
  d->ManualSegmentationWidget->setEnabled( true );
  d->AutomaticWidget->setEnabled( true );
  d->FromCollectionWidget->setEnabled( true );
  d->ToCollectionWidget->setEnabled( true );

  // Disconnect to prevent signals form cuing slots
  this->DisconnectWidgets();

  d->TransformNodeComboBox->setCurrentNodeID( QString::fromStdString( linearObjectRegistrationNode->GetCollectTransformID() ) );
  d->OutputNodeComboBox->setCurrentNodeID( QString::fromStdString( linearObjectRegistrationNode->GetOutputTransformID() ) );

  d->FromCollectionWidget->SetCurrentNode( this->mrmlScene()->GetNodeByID( linearObjectRegistrationNode->GetFromCollectionID() ) );
  d->ToCollectionWidget->SetCurrentNode( this->mrmlScene()->GetNodeByID( linearObjectRegistrationNode->GetToCollectionID() ) );

  d->ManualDOFWidget->SetLORNode( linearObjectRegistrationNode );
  d->ManualSegmentationWidget->SetLORNode( linearObjectRegistrationNode );
  d->AutomaticWidget->SetLORNode( linearObjectRegistrationNode );
  
  if ( linearObjectRegistrationNode->GetCollectionMode().compare( "ManualDOF" ) == 0 )
  {
    d->ManualDOFRadioButton->setChecked( Qt::Checked );
    d->ManualSegmentationRadioButton->setChecked( Qt::Unchecked );
    d->AutomaticRadioButton->setChecked( Qt::Unchecked );

    d->ManualDOFWidget->show();
    d->ManualSegmentationWidget->hide();
    d->AutomaticWidget->hide();
  }
  if ( linearObjectRegistrationNode->GetCollectionMode().compare( "ManualSegmentation" ) == 0 )
  {
    d->ManualDOFRadioButton->setChecked( Qt::Unchecked );
    d->ManualSegmentationRadioButton->setChecked( Qt::Checked );
    d->AutomaticRadioButton->setChecked( Qt::Unchecked );

    d->ManualDOFWidget->hide();
    d->ManualSegmentationWidget->show();
    d->AutomaticWidget->hide();
  }
  if ( linearObjectRegistrationNode->GetCollectionMode().compare( "Automatic" ) == 0 )
  {
    d->ManualDOFRadioButton->setChecked( Qt::Unchecked );
    d->ManualSegmentationRadioButton->setChecked( Qt::Unchecked );
    d->AutomaticRadioButton->setChecked( Qt::Checked );

    d->ManualDOFWidget->hide();
    d->ManualSegmentationWidget->hide();
    d->AutomaticWidget->show();
  }

  if ( linearObjectRegistrationNode->GetAutomaticMatch().compare( "True" ) == 0 )
  {
    d->AutomaticMatchCheckBox->setChecked( Qt::Checked );
  }
  else
  {
    d->AutomaticMatchCheckBox->setChecked( Qt::Unchecked );
  }

  // Unblock all singals from firing
  this->ConnectWidgets();

  std::stringstream statusString;
  statusString << "Status: ";
  statusString << d->logic()->GetOutputMessage( linearObjectRegistrationNode->GetID() );
  d->StatusLabel->setText( QString::fromStdString( statusString.str() ) ); // Also update the results
}

