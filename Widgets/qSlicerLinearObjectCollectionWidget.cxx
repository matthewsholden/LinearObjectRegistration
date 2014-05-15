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


int LINEAROBJECT_VISIBILITY_COLUMN = 0;
int LINEAROBJECT_NAME_COLUMN = 1;
int LINEAROBJECT_TYPE_COLUMN = 2;
int LINEAROBJECT_BUFFER_COLUMN = 3;
int LINEAROBJECT_COLUMNS = 4;

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
  newLinearObjectCollectionWidget->LORNode = NULL;
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
  connect( d->LinearObjectCollectionNodeComboBox, SIGNAL( nodeAddedByUser( vtkMRMLNode* ) ), this, SLOT( onCollectionNodeAdded( vtkMRMLNode* ) ) );
  
  // Use the toggled singal since it is checkable
  connect( d->ActivateButton, SIGNAL( clicked() ), this, SLOT( SetCurrentActive() ) );

  // Options buttons
  d->VisibilityButton->setIcon( QIcon( ":/Icons/Small/SlicerInvisible.png" ) );
  connect( d->VisibilityButton, SIGNAL( clicked() ), this, SLOT( onVisibilityButtonClicked() ) );

  // Table connections
  d->CollectionTableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( d->CollectionTableWidget, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( onCollectionTableContextMenu(const QPoint&) ) );
  connect( d->CollectionTableWidget, SIGNAL( cellChanged( int, int ) ), this, SLOT( onLinearObjectEdited( int, int ) ) );
  connect( d->CollectionTableWidget, SIGNAL( cellClicked( int, int ) ), this, SLOT( onCollectionTableClicked( int, int ) ) );

  this->updateWidget();  
}


void qSlicerLinearObjectCollectionWidget
::enter()
{
}


void qSlicerLinearObjectCollectionWidget
::SetLORNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectRegistrationNode* newLORNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( newNode );
  if ( newLORNode == NULL )
  {
    return;
  }

  this->LORNode = newLORNode;
}


vtkMRMLNode* qSlicerLinearObjectCollectionWidget
::GetCurrentNode()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  return d->LinearObjectCollectionNodeComboBox->currentNode();
}


void qSlicerLinearObjectCollectionWidget
::SetCurrentNode( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* newCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( newNode );
  vtkMRMLLinearObjectCollectionNode* oldCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  
  // Only update the widget if it is a new collection node
  if ( newCollectionNode == newCollectionNode )
  {
    return;
  }

  // Prevent the active fiducial list from being changed when this is called programatically
  disconnect( d->LinearObjectCollectionNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCollectionNodeChanged() ) );
  d->LinearObjectCollectionNodeComboBox->setCurrentNode( newCollectionNode );
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
::SetDefaultNodeColor( double rgb[3] )
{
  this->DefaultNodeColor[ 0 ] = rgb[ 0 ];
  this->DefaultNodeColor[ 1 ] = rgb[ 1 ];
  this->DefaultNodeColor[ 2 ] = rgb[ 2 ];
}


void qSlicerLinearObjectCollectionWidget
::SetNodeColor( double rgb[3] )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* collectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetCurrentNode() );
  if ( collectionNode == NULL )
  {
    return;
  }
  
  vtkMRMLModelHierarchyNode* modelHierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->mrmlScene()->GetNodeByID( collectionNode->GetModelHierarchyNodeID() ) );
  if ( modelHierarchyNode == NULL )
  {
    return;
  }

  vtkMRMLModelDisplayNode* modelDisplayNode = modelHierarchyNode->GetModelDisplayNode();
  if ( modelDisplayNode == NULL )
  {
    return;
  }

  modelDisplayNode->SetColor( rgb );
  modelDisplayNode->SetSelectedColor( rgb );
}


void qSlicerLinearObjectCollectionWidget
::GetNodeColor( double rgb[3] )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* collectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetCurrentNode() );
  if ( collectionNode == NULL )
  {
    return;
  }
  
  vtkMRMLModelHierarchyNode* modelHierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->mrmlScene()->GetNodeByID( collectionNode->GetModelHierarchyNodeID() ) );
  if ( modelHierarchyNode == NULL )
  {
    return;
  }

  vtkMRMLModelDisplayNode* modelDisplayNode = modelHierarchyNode->GetModelDisplayNode();
  if ( modelDisplayNode == NULL )
  {
    return;
  }

  modelDisplayNode->GetColor( rgb );
}


std::string qSlicerLinearObjectCollectionWidget
::GetQtStyleStringActive()
{
  double NodeColor[ 3 ] = { 0, 0, 0 };
  this->GetNodeColor( NodeColor );
  int QtNodeColor[ 3 ] = { 255 * NodeColor[ 0 ], 255 * NodeColor[ 1 ], 255 * NodeColor[ 2 ] };
  std::stringstream styleStringstream;
  styleStringstream << "QGroupBox { font-weight : bold; background-color: rgb( " << QtNodeColor[0] << ", " << QtNodeColor[1] << ", " << QtNodeColor[2] << ") }";
  return styleStringstream.str();
}


std::string qSlicerLinearObjectCollectionWidget
::GetQtStyleStringInactive()
{
  return "QGroupBox { font-weight : normal; background-color: white }";
}


vtkLORLinearObject* qSlicerLinearObjectCollectionWidget
::GetCurrentLinearObject()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  if ( currentCollectionNode == NULL )
  {
    return NULL;
  }

  return currentCollectionNode->GetLinearObject( d->CollectionTableWidget->currentRow() );
}


void qSlicerLinearObjectCollectionWidget
::onCollectionNodeChanged()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );

  // Create connections to update when the collection node is modified (parent widget does not need to update this widget)
  this->qvtkDisconnectAll();
  this->qvtkConnect( currentCollectionNode, vtkCommand::ModifiedEvent, this, SLOT( updateWidget() ) );
  this->qvtkConnect( currentCollectionNode, vtkMRMLLinearObjectCollectionNode::LinearObjectAboutToBeAddedEvent, this, SLOT( setScrollLinearObject( vtkObject*, void* ) ) );

  if ( currentCollectionNode != NULL )
  {
    this->SetCurrentActive();
  }

  this->updateWidget();

  emit collectionNodeChanged();
}


void qSlicerLinearObjectCollectionWidget
::onCollectionNodeAdded( vtkMRMLNode* newNode )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* newCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( newNode );
  d->LinearObjectCollectionNodeComboBox->setCurrentNode( newCollectionNode );
  this->SetNodeColor( this->DefaultNodeColor );
  this->onCollectionNodeChanged();
}


void qSlicerLinearObjectCollectionWidget
::onVisibilityButtonClicked()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  if ( currentCollectionNode == NULL )
  {
    this->updateWidget(); // Have to update the widget anyway
    return;
  }

  bool allVisible = true;

  // TODO: This should be changed to do all on and all off, rather than toggle
  if ( this->LORLogic->GetLinearObjectCollectionModelVisibility( currentCollectionNode ) )
  {
    this->LORLogic->HideLinearObjectCollectionModel( currentCollectionNode );
  }
  else
  {
    this->LORLogic->ShowLinearObjectCollectionModel( currentCollectionNode );
  }

  this->updateWidget();
}

void qSlicerLinearObjectCollectionWidget
::SetCurrentActive()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );

  if ( currentCollectionNode == NULL )
  {
    return;
  }

  this->LORLogic->SetActiveCollectionNode( currentCollectionNode );

  // Modify all collection nodes to force the other LOR collection widgets to change the state of their active button
  vtkSmartPointer< vtkCollection > collectionNodeCollection;
  collectionNodeCollection.TakeReference( this->mrmlScene()->GetNodesByClass( "vtkMRMLLinearObjectCollectionNode" ) );
  for ( int i = 0; i < collectionNodeCollection->GetNumberOfItems(); i++ )
  {
    collectionNodeCollection->GetItemAsObject( i )->Modified();
  }

  this->updateWidget();

  emit collectionNodeActivated();
}


void qSlicerLinearObjectCollectionWidget
::onCollectionTableContextMenu(const QPoint& position)
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  QPoint globalPosition = d->CollectionTableWidget->viewport()->mapToGlobal( position );

  QMenu* collectionMenu = new QMenu( d->CollectionTableWidget );
  QAction* deleteAction = new QAction( "Delete selected linear object(s)", collectionMenu );
  deleteAction->setIcon( QIcon( ":/Icons/LinearObjectDelete.png" ) );
  QAction* upAction = new QAction( "Move linear object up", collectionMenu );
  upAction->setIcon( QIcon( ":/Icons/LinearObjectUp.png" ) );
  QAction* downAction = new QAction( "Move linear object down", collectionMenu );
  downAction->setIcon( QIcon( ":/Icons/LinearObjectDown.png" ) );
  QAction* removeBufferAction = new QAction( "Remove position buffer", collectionMenu );
  removeBufferAction->setIcon( QIcon( ":/Icons/LinearObjectRemoveBuffer.png" ) );
  QAction* mergeAction = new QAction( "Merge linear objects", collectionMenu );
  mergeAction->setIcon( QIcon( ":/Icons/LinearObjectMerge.png" ) );

  collectionMenu->addAction( deleteAction );
  collectionMenu->addAction( upAction );
  collectionMenu->addAction( downAction );
  collectionMenu->addAction( removeBufferAction );
  collectionMenu->addAction( mergeAction );

  QAction* selectedAction = collectionMenu->exec( globalPosition );

  int currentIndex = d->CollectionTableWidget->currentRow();
  vtkMRMLLinearObjectCollectionNode* currentCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  
  if ( currentCollection == NULL )
  {
    return;
  }

  // Only do this for non-null node
  if ( selectedAction == deleteAction )
  {
    QItemSelectionModel* selectionModel = d->CollectionTableWidget->selectionModel();
    std::vector< int > deleteRows;
    // Need to find selected before removing because removing automatically refreshes the table
    for ( int i = 0; i < d->CollectionTableWidget->rowCount(); i++ )
    {
      if ( selectionModel->rowIntersectsSelection( i, d->CollectionTableWidget->rootIndex() ) )
      {
        deleteRows.push_back( i );
      }
    }
    //Traversing this way should be more efficient and correct
    for ( int i = deleteRows.size() - 1; i >= 0; i-- )
    {
      currentCollection->RemoveLinearObject( deleteRows.at( i ) );
    }
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

  if ( selectedAction == removeBufferAction )
  {
	if ( currentCollection->GetLinearObject( currentIndex ) != NULL )
    {
      currentCollection->GetLinearObject( currentIndex )->SetPositionBuffer( NULL );
    }
  }

  if ( selectedAction == mergeAction )
  {
    QItemSelectionModel* selectionModel = d->CollectionTableWidget->selectionModel();
    std::vector<int> mergeRows;
    for ( int i = 0; i < d->CollectionTableWidget->rowCount(); i++ )
    {
      if ( selectionModel->rowIntersectsSelection( i, d->CollectionTableWidget->rootIndex() ) )
      {
        mergeRows.push_back( i );
      }
    }

    vtkSmartPointer< vtkLORLinearObject > mergedLinearObject = this->LORLogic->MergeLinearObjects( currentCollection, mergeRows, this->LORNode->GetNoiseThreshold() );
    currentCollection->AddLinearObject( mergedLinearObject );

    for ( int i = 0; i < mergeRows.size(); i++ )
    {
      currentCollection->RemoveLinearObject( mergeRows.at( i ) );
    }
  }
  
  this->updateWidget();
}


void qSlicerLinearObjectCollectionWidget
::onLinearObjectEdited( int row, int column )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  vtkLORLinearObject* currentLinearObject = currentCollection->GetLinearObject( row );
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

  this->updateWidget(); // This may not be necessary the widget is updated whenever a fiducial is changed
}


void qSlicerLinearObjectCollectionWidget
::onCollectionTableClicked( int row, int col )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  vtkLORLinearObject* currentLinearObject = currentCollection->GetLinearObject( row );
  if ( currentCollection == NULL || currentLinearObject == NULL )
  {
    return;
  }

  if ( col == LINEAROBJECT_VISIBILITY_COLUMN )
  {
    if ( this->LORLogic->GetLinearObjectModelVisibility( currentLinearObject ) )
    {
      this->LORLogic->HideLinearObjectModel( currentLinearObject );
    }
    else
    {
      this->LORLogic->ShowLinearObjectModel( currentLinearObject );
    }
    this->updateWidget();
  }

  if ( col == LINEAROBJECT_TYPE_COLUMN )
  {
    QComboBox* typeComboBox = new QComboBox();
    typeComboBox->addItem( QString::fromStdString( LORConstants::REFERENCE_STRING ) );
    typeComboBox->addItem( QString::fromStdString( LORConstants::POINT_STRING ) );
    typeComboBox->addItem( QString::fromStdString( LORConstants::LINE_STRING ) );
    typeComboBox->addItem( QString::fromStdString( LORConstants::PLANE_STRING ) );

    typeComboBox->setCurrentIndex( LORConstants::STRING_TO_SPINNER_INDEX( currentLinearObject->GetType() ) );

    connect( typeComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onTypeSelected( int ) ) );

    // Update the widget and then add the combobox
    this->updateWidget();
    d->CollectionTableWidget->setCellWidget( row, col, typeComboBox );
    d->CollectionTableWidget->setCurrentCell( row, col );
  }

  emit linearObjectSelected();
}


void qSlicerLinearObjectCollectionWidget
::onTypeSelected( int typeIndex )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  if ( currentCollectionNode == NULL )
  {
    this->updateWidget(); // Have to update the widget anyway
    return;
  }

  vtkLORLinearObject* currentLinearObject = currentCollectionNode->GetLinearObject( d->CollectionTableWidget->currentRow() );

  vtkSmartPointer< vtkLORLinearObject > newLinearObject = NULL;
  if ( currentLinearObject->GetPositionBuffer() != NULL )
  {
    int dof = LORConstants::SPINNER_INDEX_TO_DOF( typeIndex );
    newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), this->LORNode->GetNoiseThreshold(), dof );
  }
  else if ( currentLinearObject->GetType().compare( LORConstants::POINT_STRING ) == 0 && typeIndex == LORConstants::REFERENCE_SPINNER_INDEX )
  {
    newLinearObject = vtkSmartPointer< vtkLORReference >::New();
    newLinearObject->SetBasePoint( currentLinearObject->GetBasePoint() );
  }

  if ( newLinearObject == NULL )
  {
    this->updateWidget();
    return;
  }

  newLinearObject->SetName( currentLinearObject->GetName() );
  newLinearObject->SetPositionBuffer( currentLinearObject->GetPositionBuffer() );
  currentCollectionNode->SetLinearObject( d->CollectionTableWidget->currentRow(), newLinearObject );

  this->updateWidget();
}


void qSlicerLinearObjectCollectionWidget
::setScrollLinearObject( vtkObject* caller, void* callData )
{
  vtkLORLinearObject* scrollLinearObject = reinterpret_cast< vtkLORLinearObject* >( callData );
  this->ScrollLinearObject = scrollLinearObject; // If it's null, that's ok
}


void qSlicerLinearObjectCollectionWidget
::highlightNthLinearObject( int index )
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  if ( index >= 0 && index < d->CollectionTableWidget->rowCount() )
  {
    d->CollectionTableWidget->selectRow( index );
  }
  else
  {
    d->CollectionTableWidget->clearSelection();
  }
}


void qSlicerLinearObjectCollectionWidget
::updateWidget()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
  if ( currentCollectionNode == NULL )
  {
    d->CollectionTableWidget->clear();
    d->CollectionTableWidget->setRowCount( 0 );
    d->CollectionTableWidget->setColumnCount( 0 );
    return;
  }

  if ( this->LORLogic->GetLinearObjectCollectionModelVisibility( currentCollectionNode ) )
  {
    d->VisibilityButton->setIcon( QIcon( ":/Icons/Small/SlicerVisible.png" ) );
  }
  else
  {
    d->VisibilityButton->setIcon( QIcon( ":/Icons/Small/SlicerInvisible.png" ) );
  }

  // Update the fiducials table
  d->CollectionTableWidget->blockSignals( true );
 
  d->CollectionTableWidget->clear();
  QStringList CollectionTableHeaders;
  CollectionTableHeaders << "Visibility" << "Name" << "Type" << "Buffer";
  d->CollectionTableWidget->setRowCount( currentCollectionNode->Size() );
  d->CollectionTableWidget->setColumnCount( LINEAROBJECT_COLUMNS );
  d->CollectionTableWidget->setHorizontalHeaderLabels( CollectionTableHeaders );
  d->CollectionTableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );

  QTableWidgetItem* scrollItem = NULL;
  
  for ( int i = 0; i < currentCollectionNode->Size(); i++ )
  {
    if ( currentCollectionNode->GetLinearObject( i ) == NULL )
    {
      continue;
    }

    QTableWidgetItem* visItem = new QTableWidgetItem( QString( "" ) );
    if ( this->LORLogic->GetLinearObjectModelVisibility( currentCollectionNode->GetLinearObject( i ) ) )
    {
      visItem->setIcon( QIcon( ":/Icons/Small/SlicerVisible.png" ) );
    }
    else
    {
      visItem->setIcon( QIcon( ":/Icons/Small/SlicerInvisible.png" ) );
    }
    QTableWidgetItem* nameItem = new QTableWidgetItem( QString::fromStdString( currentCollectionNode->GetLinearObject( i )->GetName() ) );
    QTableWidgetItem* typeItem = new QTableWidgetItem( QString::fromStdString( currentCollectionNode->GetLinearObject( i )->GetType() ) );
    QTableWidgetItem* bufferItem = new QTableWidgetItem( QString::fromStdString( currentCollectionNode->GetLinearObject( i )->GetPositionBufferString() ) );

    if ( this->ScrollLinearObject == currentCollectionNode->GetLinearObject( i ) )
    {
      scrollItem = nameItem;
    }

    d->CollectionTableWidget->setItem( i, LINEAROBJECT_VISIBILITY_COLUMN, visItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_NAME_COLUMN, nameItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_TYPE_COLUMN, typeItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_BUFFER_COLUMN, bufferItem );
  }
  d->CollectionTableWidget->resizeRowsToContents();

  if ( scrollItem != NULL )
  {
    d->CollectionTableWidget->scrollToItem( scrollItem );
  }
  this->ScrollLinearObject = NULL;

  d->CollectionTableWidget->blockSignals( false );

  emit updateFinished();
}
