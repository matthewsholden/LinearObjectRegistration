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
#include "qSlicerLinearObjectCollectionWidget.h"

#include <QtGui>


int LINEAROBJECT_NAME_COLUMN = 0;
int LINEAROBJECT_TYPE_COLUMN = 1;
int LINEAROBJECT_BUFFER_COLUMN = 2;
int LINEAROBJECT_COLUMNS = 3;


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CreateModels
class qSlicerLinearObjectCollectionWidgetPrivate
  : public Ui_qSlicerLinearObjectCollectionWidget
{
  Q_DECLARE_PUBLIC(qSlicerLinearObjectCollectionWidget);
protected:
  qSlicerLinearObjectCollectionWidget* const q_ptr;

public:
  qSlicerLinearObjectCollectionWidgetPrivate( qSlicerLinearObjectCollectionWidget& object);
  ~qSlicerLinearObjectCollectionWidgetPrivate();
  virtual void setupUi(qSlicerLinearObjectCollectionWidget*);
};

// --------------------------------------------------------------------------
qSlicerLinearObjectCollectionWidgetPrivate
::qSlicerLinearObjectCollectionWidgetPrivate( qSlicerLinearObjectCollectionWidget& object) : q_ptr(&object)
{
}

qSlicerLinearObjectCollectionWidgetPrivate
::~qSlicerLinearObjectCollectionWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLinearObjectCollectionWidgetPrivate
::setupUi(qSlicerLinearObjectCollectionWidget* widget)
{
  this->Ui_qSlicerLinearObjectCollectionWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLinearObjectCollectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLinearObjectCollectionWidget
::qSlicerLinearObjectCollectionWidget(QWidget* parentWidget) : Superclass( parentWidget ) , d_ptr( new qSlicerLinearObjectCollectionWidgetPrivate(*this) )
{
}


qSlicerLinearObjectCollectionWidget
::~qSlicerLinearObjectCollectionWidget()
{
}


qSlicerLinearObjectCollectionWidget* qSlicerLinearObjectCollectionWidget
::New( vtkSlicerLinearObjectRegistrationLogic* newLORLogic )
{
  qSlicerLinearObjectCollectionWidget* newLinearObjectCollectionWidget = new qSlicerLinearObjectCollectionWidget();
  newLinearObjectCollectionWidget->LORLogic = newLORLogic;
  newLinearObjectCollectionWidget->setup();
  return newLinearObjectCollectionWidget;
}


void qSlicerLinearObjectCollectionWidget
::setup()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  d->setupUi(this);
  this->setMRMLScene( this->LORLogic->GetMRMLScene() );

  connect( d->LinearObjectCollectionNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCollectionNodeChanged() ) );
  
  // Use the pressed signal (otherwise we can unpress buttons without clicking them)
  connect( d->ActiveButton, SIGNAL( toggled( bool ) ), this, SLOT( SetCurrentActive() ) );

  d->CollectionTableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( d->CollectionTableWidget, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( onCollectionTableContextMenu(const QPoint&) ) );
  connect( d->CollectionTableWidget, SIGNAL( cellChanged( int, int ) ), this, SLOT( onLinearObjectEdited( int, int ) ) );

  // Connect to the markups mrml events
  this->qvtkConnect( this->GetCurrentNode(), vtkCommand::ModifiedEvent, this, SLOT( onCollectionNodeModified() ) );

  this->updateWidget();  
}


void qSlicerLinearObjectCollectionWidget
::enter()
{
}


vtkMRMLNode* qSlicerLinearObjectCollectionWidget
::GetCurrentNode()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  return d->LinearObjectCollectionNodeComboBox->currentNode();
}


void qSlicerLinearObjectCollectionWidget
::SetCurrentNode( vtkMRMLNode* currentNode )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLORLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( currentNode );

  // Prevent the active fiducial list from being changed when this is called programatically
  disconnect( d->LinearObjectCollectionNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCollectionNodeChanged() ) );
  d->LinearObjectCollectionNodeComboBox->setCurrentNode( currentCollectionNode );
  connect( d->LinearObjectCollectionNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCollectionNodeChanged() ) );

  this->updateWidget(); // This will update the active button and table widget
}


void qSlicerLinearObjectCollectionWidget
::SetNodeBaseName( std::string newNodeBaseName )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  d->LinearObjectCollectionNodeComboBox->setBaseName( QString::fromStdString( newNodeBaseName ) );
}


void qSlicerLinearObjectCollectionWidget
::onCollectionNodeChanged()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  emit collectionNodeChanged();

  vtkMRMLLORLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );

  if ( currentCollectionNode == NULL )
  {
    this->updateWidget(); // Have to update the widget anyway
    return;
  }

  this->SetCurrentActive();

  // Disconnect and reconnect to new node
  this->qvtkDisconnectAll();
  this->qvtkConnect( this->GetCurrentNode(), vtkCommand::ModifiedEvent, this, SLOT( onCollectionNodeModified() ) );

  this->updateWidget();
}


void qSlicerLinearObjectCollectionWidget
::SetCurrentActive()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLORLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );

  if ( currentCollectionNode == NULL )
  {
    return;
  }

  this->LORLogic->SetActiveCollectionNode( currentCollectionNode );

  // Modify all collection nodes to force the other LOR collection widgets to change the state of their active button
  vtkSmartPointer< vtkCollection > collectionNodeCollection;
  collectionNodeCollection.TakeReference( this->mrmlScene()->GetNodesByClass( "vtkMRMLLORLinearObjectCollectionNode" ) );
  for ( int i = 0; i < collectionNodeCollection->GetNumberOfItems(); i++ )
  {
    collectionNodeCollection->GetItemAsObject( i )->Modified();
  }

  this->updateWidget();
}


void qSlicerLinearObjectCollectionWidget
::onCollectionTableContextMenu(const QPoint& position)
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  QPoint globalPosition = d->CollectionTableWidget->viewport()->mapToGlobal( position );

  QMenu* collectionMenu = new QMenu( d->CollectionTableWidget );
  QAction* activateAction = new QAction( "Make collection active", collectionMenu );
  QAction* deleteAction = new QAction( "Delete current linear object", collectionMenu );
  QAction* upAction = new QAction( "Move current linear object up", collectionMenu );
  QAction* downAction = new QAction( "Move current linear object down", collectionMenu );
  QAction* shuffleAction = new QAction( "Remove blank linear objects", collectionMenu );
  QAction* convertAction = new QAction( "Convert to reference", collectionMenu );
  QAction* modelAction = new QAction( "Show/hide model", collectionMenu ); 
  QAction* matchAction = new QAction( "Match...", collectionMenu );

  collectionMenu->addAction( activateAction );
  collectionMenu->addAction( deleteAction );
  collectionMenu->addAction( upAction );
  collectionMenu->addAction( downAction );
  collectionMenu->addAction( shuffleAction );  
  collectionMenu->addAction( convertAction );
  collectionMenu->addAction( modelAction );
  collectionMenu->addAction( matchAction );

  QAction* selectedAction = collectionMenu->exec( globalPosition );

  int currentIndex = d->CollectionTableWidget->currentRow();
  vtkMRMLLORLinearObjectCollectionNode* currentCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  
  if ( currentCollection == NULL )
  {
    return;
  }

  // Only do this for non-null node
  if ( selectedAction == activateAction )
  {
    this->SetCurrentActive();
  }

  if ( selectedAction == deleteAction )
  {
    currentCollection->RemoveLinearObject( currentIndex );
  }

  if ( selectedAction == upAction )
  {
    if ( currentIndex > 0 )
    {
      currentCollection->Swap( currentIndex, currentIndex - 1 );
    }
  }

  if ( selectedAction == downAction )
  {
    if ( currentIndex < currentCollection->Size() - 1 )
    {
      currentCollection->Swap( currentIndex, currentIndex + 1 );
    }
  }

  if ( selectedAction == shuffleAction )
  {
    currentCollection->ShuffleOutNull();
  }

  if ( selectedAction == convertAction )
  {
    vtkMRMLLORLinearObjectNode* currentNode = currentCollection->GetLinearObject( currentIndex );
    if ( currentNode != NULL && currentNode->GetType().compare( "Point" ) == 0 )
    {
      vtkSmartPointer< vtkMRMLLORReferenceNode > referenceNode = vtkSmartPointer< vtkMRMLLORReferenceNode >::New();
      referenceNode->SetBasePoint( currentNode->GetBasePoint() );
      referenceNode->SetPositionBuffer( currentNode->GetPositionBuffer() );
      currentCollection->SetLinearObject( currentIndex, referenceNode );
    }
  }

  if ( selectedAction == modelAction )
  {
    this->LORLogic->ToggleLinearObjectModelVisibility( currentCollection->GetLinearObject( currentIndex ) );
  }

  if ( selectedAction == matchAction )
  {
    emit matchRequested( currentIndex );
  }

  
  this->updateWidget();
}


void qSlicerLinearObjectCollectionWidget
::onLinearObjectEdited( int row, int column )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLORLinearObjectCollectionNode* currentCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  vtkMRMLLORLinearObjectNode* currentLinearObject = currentCollection->GetLinearObject( row );
  if ( currentCollection == NULL && currentLinearObject != NULL )
  {
    this->updateWidget();
    return;
  }

  // Find the entry that we changed
  QTableWidgetItem* qItem = d->CollectionTableWidget->item( row, column );
  QString qText = qItem->text();

  // Change the name
  if ( column == LINEAROBJECT_NAME_COLUMN )
  {
    currentLinearObject->SetName( qText.toStdString() );
  }

  // Also allow changing the type here
  if ( column == LINEAROBJECT_TYPE_COLUMN )
  {
    if ( currentLinearObject->GetPositionBuffer() == NULL )
    {
      this->updateWidget();
      return;
    }

    // Otherwise, we can do any sort of change we want
    vtkSmartPointer< vtkMRMLLORLinearObjectNode > newLinearObject = NULL;
    if ( qText.toStdString().compare( "Reference" ) == 0 )
    {
      newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), vtkSlicerLinearObjectRegistrationLogic::REFERENCE_DOF );
    }
    if ( qText.toStdString().compare( "Point" ) == 0 )
    {
      newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), vtkSlicerLinearObjectRegistrationLogic::POINT_DOF );
    }
    if ( qText.toStdString().compare( "Line" ) == 0 )
    {
      newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), vtkSlicerLinearObjectRegistrationLogic::LINE_DOF );
    }
    if ( qText.toStdString().compare( "Plane" ) == 0 )
    {
      newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), vtkSlicerLinearObjectRegistrationLogic::PLANE_DOF );
    }

    if ( newLinearObject == NULL )
    {
      this->updateWidget();
      return;
    }

    newLinearObject->SetName( currentLinearObject->GetName() );
    newLinearObject->SetPositionBuffer( currentLinearObject->GetPositionBuffer() );
    currentCollection->SetLinearObject( row, newLinearObject );
  }

  this->updateWidget(); // This may not be necessary the widget is updated whenever a fiducial is changed
}



void qSlicerLinearObjectCollectionWidget
::updateWidget()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLORLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  if ( currentCollectionNode == NULL )
  {
    d->CollectionTableWidget->clear();
    d->CollectionTableWidget->setRowCount( 0 );
    d->CollectionTableWidget->setColumnCount( 0 );
    d->ActiveButton->setChecked( false );
    return;
  }

  // Set the button indicating if this list is active
  d->ActiveButton->blockSignals( true );

  if ( this->LORLogic->GetActiveCollectionNode() != NULL && strcmp( this->LORLogic->GetActiveCollectionNode()->GetID(), currentCollectionNode->GetID() ) == 0 )
  {
    d->ActiveButton->setChecked( true );
  }
  else
  {
    d->ActiveButton->setChecked( false );
  }

  d->ActiveButton->blockSignals( false );

  // Update the fiducials table
  d->CollectionTableWidget->blockSignals( true );
 
  d->CollectionTableWidget->clear();
  QStringList CollectionTableHeaders;
  CollectionTableHeaders << "Name" << "Type" << "Buffer";
  d->CollectionTableWidget->setRowCount( currentCollectionNode->Size() );
  d->CollectionTableWidget->setColumnCount( LINEAROBJECT_COLUMNS );
  d->CollectionTableWidget->setHorizontalHeaderLabels( CollectionTableHeaders );
  d->CollectionTableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
  
  for ( int i = 0; i < currentCollectionNode->Size(); i++ )
  {
    if ( currentCollectionNode->GetLinearObject( i ) == NULL )
    {
      continue;
    }

    QTableWidgetItem* nameItem = new QTableWidgetItem( QString::fromStdString( currentCollectionNode->GetLinearObject( i )->GetName() ) );
    QTableWidgetItem* typeItem = new QTableWidgetItem( QString::fromStdString( currentCollectionNode->GetLinearObject( i )->GetType() ) );

    std::stringstream bufferString;
    if ( currentCollectionNode->GetLinearObject( i )->GetPositionBuffer() == NULL )
    {
      bufferString << "None";
    }
    else
    {
      bufferString << currentCollectionNode->GetLinearObject( i )->GetPositionBuffer()->Size() << " Positions";
    }
    QTableWidgetItem* bufferItem = new QTableWidgetItem( QString::fromStdString( bufferString.str() ) );


    d->CollectionTableWidget->setItem( i, LINEAROBJECT_NAME_COLUMN, nameItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_TYPE_COLUMN, typeItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_BUFFER_COLUMN, bufferItem );
  }
  d->CollectionTableWidget->resizeRowsToContents();

  d->CollectionTableWidget->blockSignals( false );
}
