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
  
  // Use the toggled singal since it is checkable
  connect( d->ActiveButton, SIGNAL( toggled( bool ) ), this, SLOT( SetCurrentActive() ) );

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

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( currentNode );

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

  emit collectionNodeChanged();

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );

  if ( currentCollectionNode == NULL )
  {
    this->updateWidget(); // Have to update the widget anyway
    return;
  }

  this->SetCurrentActive();

  this->updateWidget();
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
}


void qSlicerLinearObjectCollectionWidget
::onCollectionTableContextMenu(const QPoint& position)
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  QPoint globalPosition = d->CollectionTableWidget->viewport()->mapToGlobal( position );

  QMenu* collectionMenu = new QMenu( d->CollectionTableWidget );
  QAction* deleteAction = new QAction( "Delete linear object", collectionMenu );
  deleteAction->setIcon( QIcon( ":/Icons/LinearObjectDelete.png" ) );
  QAction* upAction = new QAction( "Move linear object up", collectionMenu );
  upAction->setIcon( QIcon( ":/Icons/LinearObjectUp.png" ) );
  QAction* downAction = new QAction( "Move linear object down", collectionMenu );
  downAction->setIcon( QIcon( ":/Icons/LinearObjectDown.png" ) );

  collectionMenu->addAction( deleteAction );
  collectionMenu->addAction( upAction );
  collectionMenu->addAction( downAction );

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
    typeComboBox->addItem( QString::fromStdString( "Reference" ) );
    typeComboBox->addItem( QString::fromStdString( "Point" ) );
    typeComboBox->addItem( QString::fromStdString( "Line" ) );
    typeComboBox->addItem( QString::fromStdString( "Plane" ) );

    typeComboBox->setCurrentIndex( LORConstants::STRING_TO_INDEX( currentLinearObject->GetType() ) );

    connect( typeComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onTypeSelected( int ) ) );

    // Update the widget and then add the combobox
    this->updateWidget();
    d->CollectionTableWidget->setCellWidget( row, col, typeComboBox );
    d->CollectionTableWidget->setCurrentCell( row, col );
  }

  if ( col == LINEAROBJECT_BUFFER_COLUMN )
  {
    this->updateWidget();
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
    int dof = LORConstants::STRING_TO_DOF( LORConstants::INDEX_TO_STRING( typeIndex ) );
    newLinearObject = this->LORLogic->PositionBufferToLinearObject( currentLinearObject->GetPositionBuffer(), dof );
  }
  else if ( currentLinearObject->GetType().compare( "Point" ) == 0 && typeIndex == 0 )
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
::updateWidget()
{
  Q_D(qSlicerLinearObjectCollectionWidget);

  vtkMRMLLinearObjectCollectionNode* currentCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( d->LinearObjectCollectionNodeComboBox->currentNode() );
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

    d->CollectionTableWidget->setItem( i, LINEAROBJECT_VISIBILITY_COLUMN, visItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_NAME_COLUMN, nameItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_TYPE_COLUMN, typeItem );
    d->CollectionTableWidget->setItem( i, LINEAROBJECT_BUFFER_COLUMN, bufferItem );
  }
  d->CollectionTableWidget->resizeRowsToContents();

  d->CollectionTableWidget->blockSignals( false );
}
