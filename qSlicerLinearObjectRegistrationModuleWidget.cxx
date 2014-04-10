
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
#include "qSlicerLORModelWidget.h"
#include "qSlicerTransformPreviewWidget.h"

#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"


int COLLECTION_MANUAL_DOF_TAB = 0;
int COLLECTION_MANUAL_SEGMENTATION_TAB = 1;
int COLLECTION_AUTOMATIC_TAB = 2;
int COLLECTION_MODEL_TAB = 3;
int COLLECTION_TABS = 4;

int LINEAROBJECT_VISIBILITY_COLUMN = 0;
int LINEAROBJECT_NAME_COLUMN = 1;
int LINEAROBJECT_TYPE_COLUMN = 2;
int LINEAROBJECT_BUFFER_COLUMN = 3;
int LINEAROBJECT_COLUMNS = 4;


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

  QTabBar* CollectTabs;

  qSlicerTransformPreviewWidget* TransformPreviewWidget;
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
::OnMatchButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  // Find the positions and swap
  vtkMRMLLinearObjectCollectionNode* fromCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->FromCollectionWidget->GetCurrentNode() );
  vtkMRMLLinearObjectCollectionNode* toCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->ToCollectionWidget->GetCurrentNode() );

  int fromPosition = fromCollection->GetLinearObjectPosition( d->FromCollectionWidget->GetCurrentLinearObject() );
  int toPosition = toCollection->GetLinearObjectPosition( d->ToCollectionWidget->GetCurrentLinearObject() );

  if ( fromPosition >= 0 && toPosition >= 0 )
  {
    toCollection->Swap( toPosition, fromPosition );
  }
}


void qSlicerLinearObjectRegistrationModuleWidget
::UpdateMatchCandidates()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  vtkLORLinearObject* fromLinearObject = d->FromCollectionWidget->GetCurrentLinearObject();
  vtkLORLinearObject* toLinearObject = d->ToCollectionWidget->GetCurrentLinearObject();

  QTableWidgetItem* fromFrameItem = new QTableWidgetItem();
  QTableWidgetItem* fromNameItem = new QTableWidgetItem();
  QTableWidgetItem* fromTypeItem = new QTableWidgetItem();
  QTableWidgetItem* fromBufferItem = new QTableWidgetItem();
  if ( fromLinearObject != NULL )
  { 
    fromFrameItem = new QTableWidgetItem( QString::fromStdString( "From:" ) );
    fromNameItem = new QTableWidgetItem( QString::fromStdString( fromLinearObject->GetName() ) );
    fromTypeItem = new QTableWidgetItem( QString::fromStdString( fromLinearObject->GetType() ) );
    fromBufferItem = new QTableWidgetItem( QString::fromStdString( fromLinearObject->GetPositionBufferString() ) );
  }

  d->FromMatchTable->setColumnCount( LINEAROBJECT_COLUMNS );
  d->FromMatchTable->setRowCount( 1 );

  d->FromMatchTable->setItem( 0, LINEAROBJECT_VISIBILITY_COLUMN, fromFrameItem );
  d->FromMatchTable->setItem( 0, LINEAROBJECT_NAME_COLUMN, fromNameItem );
  d->FromMatchTable->setItem( 0, LINEAROBJECT_TYPE_COLUMN, fromTypeItem );
  d->FromMatchTable->setItem( 0, LINEAROBJECT_BUFFER_COLUMN, fromBufferItem );

  d->FromMatchTable->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
  d->FromMatchTable->horizontalHeader()->hide();
  d->FromMatchTable->resizeRowsToContents();
  d->FromMatchTable->setMaximumHeight( d->FromMatchTable->verticalHeader()->sectionSize( 0 ) );


  QTableWidgetItem* toFrameItem = new QTableWidgetItem();
  QTableWidgetItem* toNameItem = new QTableWidgetItem();
  QTableWidgetItem* toTypeItem = new QTableWidgetItem();
  QTableWidgetItem* toBufferItem = new QTableWidgetItem();
  if ( toLinearObject != NULL )
  {
    toFrameItem = new QTableWidgetItem( QString::fromStdString( "To:" ) );
    toNameItem = new QTableWidgetItem( QString::fromStdString( toLinearObject->GetName() ) );
    toTypeItem = new QTableWidgetItem( QString::fromStdString( toLinearObject->GetType() ) );
    toBufferItem = new QTableWidgetItem( QString::fromStdString( toLinearObject->GetPositionBufferString() ) );
  }

  d->ToMatchTable->setRowCount( 1 );
  d->ToMatchTable->setColumnCount( LINEAROBJECT_COLUMNS );

  d->ToMatchTable->setItem( 0, LINEAROBJECT_VISIBILITY_COLUMN, toFrameItem );
  d->ToMatchTable->setItem( 0, LINEAROBJECT_NAME_COLUMN, toNameItem );
  d->ToMatchTable->setItem( 0, LINEAROBJECT_TYPE_COLUMN, toTypeItem );
  d->ToMatchTable->setItem( 0, LINEAROBJECT_BUFFER_COLUMN, toBufferItem );

  d->ToMatchTable->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
  d->ToMatchTable->horizontalHeader()->hide();
  d->ToMatchTable->resizeRowsToContents();
  d->ToMatchTable->setMaximumHeight( d->ToMatchTable->verticalHeader()->sectionSize( 0 ) );

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


  d->CollectTabs = new QTabBar( d->CollectGroupBox );
  d->CollectTabsLayout->addWidget( d->CollectTabs );

  this->AddCollectWidget( qSlicerLORManualDOFWidget::New( d->logic() ) );
  this->AddCollectWidget( qSlicerLORManualSegmentationWidget::New( d->logic() ) );
  this->AddCollectWidget( qSlicerLORAutomaticWidget::New( d->logic() ) );
  this->AddCollectWidget( qSlicerLORModelWidget::New( d->logic() ) );

  d->TransformPreviewWidget = qSlicerTransformPreviewWidget::New( d->logic()->GetMRMLScene() );
  d->PreviewTransformGroupBox->layout()->addWidget( d->TransformPreviewWidget );

  // If the mrml node changes, update from the mrml node 
  connect( d->ModuleNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateFromMRMLNode() ) );
  // Otherwise, we will update from the logic
  this->qvtkConnect( d->logic(), vtkCommand::ModifiedEvent, this, SLOT( UpdateFromMRMLNode() ) );

  // Make connections to update the mrml from the widget
  this->ConnectWidgets();  

  // Various other connections
  connect( d->MatchButton, SIGNAL( clicked() ), this, SLOT( OnMatchButtonClicked() ) );

  connect( d->FromCollectionWidget, SIGNAL( linearObjectSelected() ), this, SLOT( UpdateMatchCandidates() ) );
  connect( d->ToCollectionWidget, SIGNAL( linearObjectSelected() ), this, SLOT( UpdateMatchCandidates() ) );

  this->UpdateMatchCandidates();
}


void qSlicerLinearObjectRegistrationModuleWidget
::AddCollectWidget( qSlicerLORCollectControlsWidget* newControlsWidget )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  // Create the collect widget
  qSlicerLORCollectWidget* newCollectWidget = qSlicerLORCollectWidget::New( d->logic() );
  newCollectWidget->SetControlsWidget( newControlsWidget );

  // Add this to the collection tabs
  int tabIndex = d->CollectTabs->addTab( QString::fromStdString( newControlsWidget->GetCollectModeName() ) );
  this->CollectWidgets[ tabIndex ] = newCollectWidget;

  // Add it to the collection tabs layout
  d->CollectTabsLayout->addWidget( newCollectWidget );

}


void qSlicerLinearObjectRegistrationModuleWidget
::enter()
{
  // Nothing to do
  this->Superclass::enter();
}


void qSlicerLinearObjectRegistrationModuleWidget
::exit()
{
  // Nothing to do
  this->Superclass::exit();
}


void qSlicerLinearObjectRegistrationModuleWidget
::ConnectWidgets()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  connect( d->OutputNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->CollectTabs, SIGNAL( currentChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->AutomaticMatchCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->AutomaticMergeCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->FromCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->ToCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );

  connect( d->MergeThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->NoiseThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->MatchingThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->MinimumCollectionPositionsSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );
  connect( d->TrimPositionsSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );
}


void qSlicerLinearObjectRegistrationModuleWidget
::DisconnectWidgets()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  disconnect( d->OutputNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( UpdateToMRMLNode() ) );
  
  disconnect( d->CollectTabs, SIGNAL( currentChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );
  
  disconnect( d->AutomaticMatchCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->AutomaticMergeCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( UpdateToMRMLNode() ) );

  disconnect( d->FromCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->ToCollectionWidget, SIGNAL( collectionNodeChanged() ), this, SLOT( UpdateToMRMLNode() ) );

  disconnect( d->MergeThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->NoiseThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->MatchingThresholdSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->MinimumCollectionPositionsSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );
  disconnect( d->TrimPositionsSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( UpdateToMRMLNode() ) );
}


void qSlicerLinearObjectRegistrationModuleWidget
::EnableAllWidgets( bool enable )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

    d->OutputNodeComboBox->setEnabled( enable );

    d->CollectTabs->setEnabled( enable );
    // Disable all of the collect widgets
    for ( std::map< int, qSlicerLORCollectWidget* >::iterator itr = this->CollectWidgets.begin(); itr != this->CollectWidgets.end(); itr++ )
    {
      itr->second->setEnabled( enable );
    }

    d->FromCollectionWidget->setEnabled( enable );
    d->ToCollectionWidget->setEnabled( enable );

    d->TransformPreviewWidget->setEnabled( enable );

    d->AutomaticMatchCheckBox->setEnabled( enable );
    d->AutomaticMergeCheckBox->setEnabled( enable );

    d->MergeThresholdSpinBox->setEnabled( enable );
    d->NoiseThresholdSpinBox->setEnabled( enable );
    d->MatchingThresholdSpinBox->setEnabled( enable );
    d->MinimumCollectionPositionsSpinBox->setEnabled( enable );
    d->TrimPositionsSpinBox->setEnabled( enable );
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

  /*
  if ( d->TransformNodeComboBox->currentNode() == NULL )
  {
    linearObjectRegistrationNode->SetCollectTransformID( "", vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetCollectTransformID( d->TransformNodeComboBox->currentNode()->GetID(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  */

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

  // Add set collect mode function to linear object registration node
  linearObjectRegistrationNode->SetCollectMode( this->CollectWidgets[ d->CollectTabs->currentIndex() ]->GetControlsWidget()->GetCollectModeName() );

  if ( d->AutomaticMatchCheckBox->isChecked() )
  {
    linearObjectRegistrationNode->SetAutomaticMatch( true, vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetAutomaticMatch( false, vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  if ( d->AutomaticMergeCheckBox->isChecked() )
  {
    linearObjectRegistrationNode->SetAutomaticMerge( true, vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }
  else
  {
    linearObjectRegistrationNode->SetAutomaticMerge( false, vtkMRMLLinearObjectRegistrationNode::NeverModify );
  }

  linearObjectRegistrationNode->SetMergeThreshold( pow( 10, d->MergeThresholdSpinBox->value() ), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  linearObjectRegistrationNode->SetNoiseThreshold( d->NoiseThresholdSpinBox->value(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  linearObjectRegistrationNode->SetMatchingThreshold( d->MatchingThresholdSpinBox->value(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  linearObjectRegistrationNode->SetMinimumCollectionPositions( d->MinimumCollectionPositionsSpinBox->value(), vtkMRMLLinearObjectRegistrationNode::NeverModify );
  linearObjectRegistrationNode->SetTrimPositions( d->TrimPositionsSpinBox->value(), vtkMRMLLinearObjectRegistrationNode::NeverModify );

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
    this->EnableAllWidgets( false );
    d->StatusLabel->setText( "No Linear Object Registration module node selected." );
    return;
  }

  this->EnableAllWidgets( true );

  // Disconnect to prevent signals form cuing slots
  this->DisconnectWidgets();

  //d->TransformNodeComboBox->setCurrentNodeID( QString::fromStdString( linearObjectRegistrationNode->GetCollectTransformID() ) );
  d->OutputNodeComboBox->setCurrentNodeID( QString::fromStdString( linearObjectRegistrationNode->GetOutputTransformID() ) );

  d->FromCollectionWidget->SetCurrentNode( this->mrmlScene()->GetNodeByID( linearObjectRegistrationNode->GetFromCollectionID() ) );
  d->ToCollectionWidget->SetCurrentNode( this->mrmlScene()->GetNodeByID( linearObjectRegistrationNode->GetToCollectionID() ) );
  d->TransformPreviewWidget->SetCurrentNode( this->mrmlScene()->GetNodeByID( linearObjectRegistrationNode->GetOutputTransformID() ) );

  d->FromCollectionWidget->SetLORNode( linearObjectRegistrationNode );
  d->ToCollectionWidget->SetLORNode( linearObjectRegistrationNode );
  
  // Find the tab with the label equal to the collect mode
  std::string tabString = linearObjectRegistrationNode->GetCollectMode();
  int tabIndex;
  // Find the corresponding tab
  for ( int i = 0; i < d->CollectTabs->count(); i++ )
  {
    if ( tabString.compare( d->CollectTabs->tabText( i ).toStdString() ) == 0 )
    {
      tabIndex = i;
    }
  }
  // Iterate over all tabs and hide all the non-equal tabs
  for ( std::map< int, qSlicerLORCollectWidget* >::iterator itr = this->CollectWidgets.begin(); itr != this->CollectWidgets.end(); itr++ )
  {
    itr->second->SetLORNode( linearObjectRegistrationNode );
    if ( itr->first == tabIndex )
    {
      itr->second->show();
    }
    else
    {
      itr->second->hide();
    }
  }


  if ( linearObjectRegistrationNode->GetAutomaticMatch() )
  {
    d->AutomaticMatchCheckBox->setChecked( Qt::Checked );
  }
  else
  {
    d->AutomaticMatchCheckBox->setChecked( Qt::Unchecked );
  }
  if ( linearObjectRegistrationNode->GetAutomaticMerge() )
  {
    d->AutomaticMergeCheckBox->setChecked( Qt::Checked );
  }
  else
  {
    d->AutomaticMergeCheckBox->setChecked( Qt::Unchecked );
  }

  d->MergeThresholdSpinBox->setValue( log10( linearObjectRegistrationNode->GetMergeThreshold() ) );
  d->NoiseThresholdSpinBox->setValue( linearObjectRegistrationNode->GetNoiseThreshold() );
  d->MatchingThresholdSpinBox->setValue( linearObjectRegistrationNode->GetMatchingThreshold() );
  d->MinimumCollectionPositionsSpinBox->setValue( linearObjectRegistrationNode->GetMinimumCollectionPositions() );
  d->TrimPositionsSpinBox->setValue( linearObjectRegistrationNode->GetTrimPositions() );

  // Unblock all singals from firing
  this->ConnectWidgets();

  std::stringstream statusString;
  statusString << "Status: ";
  statusString << d->logic()->GetOutputMessage( linearObjectRegistrationNode->GetID() );
  d->StatusLabel->setText( QString::fromStdString( statusString.str() ) ); // Also update the results
}

