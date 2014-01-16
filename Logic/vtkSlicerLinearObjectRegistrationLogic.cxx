
// LinearObjectRegistration Logic includes
#include "vtkSlicerLinearObjectRegistrationLogic.h"

const double FILTER_WIDTH = 5.0;
const double EXTRACTION_THRESHOLD = 0.5;
const double MATCHING_THRESHOLD = 10.0;

const int MINIMUM_COLLECTION_FRAMES = 100;
const int DIRECTION_SCALE = 100;

const int REFERENCE_DOF = 4; // This obviously isn't true, but we need to distinguish from points
const int POINT_DOF = 0;
const int LINE_DOF = 1;
const int PLANE_DOF = 2;
const int UNKNOWN_DOF = -1;


void PrintToFile( std::string str )
{
  ofstream o( "LinearObjectRegistrationLog.txt", std::ios_base::app );
  int c = clock();
  o << c / CLOCKS_PER_SEC << " : " << str << std::endl;
  o.close();
}


//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkSlicerLinearObjectRegistrationLogic );


// Constructor ---------------------------------------------------------------------------
// 
vtkSlicerLinearObjectRegistrationLogic
::vtkSlicerLinearObjectRegistrationLogic()
{ 
  this->ObservedTransformNode = NULL;
  this->ActivePositionBuffer = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();

  this->OutputMessage = "";
}


vtkSlicerLinearObjectRegistrationLogic::
~vtkSlicerLinearObjectRegistrationLogic()
{
}


// Slicer Internal ----------------------------------------------------------------------------


void
vtkSlicerLinearObjectRegistrationLogic
::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  
  os << indent << "vtkSlicerLinearObjectRegistrationLogic: " << this->GetClassName() << "\n";
}



void
vtkSlicerLinearObjectRegistrationLogic
::OnMRMLSceneEndClose()
{
}



void
vtkSlicerLinearObjectRegistrationLogic
::SetMRMLSceneInternal( vtkMRMLScene * newScene )
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}



void
vtkSlicerLinearObjectRegistrationLogic
::RegisterNodes()
{
  if( ! this->GetMRMLScene() )
  {
    return;
  }

  vtkMRMLLinearObjectRegistrationNode* lorNode = vtkMRMLLinearObjectRegistrationNode::New();
  this->GetMRMLScene()->RegisterNodeClass( lorNode );
  lorNode->Delete();

  vtkMRMLLORLinearObjectCollectionNode* lorCollectionNode = vtkMRMLLORLinearObjectCollectionNode::New();
  this->GetMRMLScene()->RegisterNodeClass( lorCollectionNode );
  lorCollectionNode->Delete();

  vtkMRMLLORLinearObjectCollectionStorageNode* lorCollectionStorageNode = vtkMRMLLORLinearObjectCollectionStorageNode::New();
  this->GetMRMLScene()->RegisterNodeClass( lorCollectionStorageNode );
  lorCollectionStorageNode->Delete();
}



void
vtkSlicerLinearObjectRegistrationLogic
::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}



void
vtkSlicerLinearObjectRegistrationLogic
::OnMRMLSceneNodeAdded( vtkMRMLNode* vtkNotUsed( node ) )
{
}



void
vtkSlicerLinearObjectRegistrationLogic
::OnMRMLSceneNodeRemoved( vtkMRMLNode* vtkNotUsed( node ) )
{
}



// Linear Object Registration ----------------------------------------------------

std::string vtkSlicerLinearObjectRegistrationLogic
::GetOutputMessage()
{
  return this->OutputMessage;
}


void vtkSlicerLinearObjectRegistrationLogic
::SetOutputMessage( std::string newOutputMessage )
{
  this->OutputMessage = newOutputMessage;
  this->Modified();
}


void vtkSlicerLinearObjectRegistrationLogic
::ObserveTransformNode( vtkMRMLNode* node )
{
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  if ( transformNode == NULL )
  {
    return;
  }
  if ( this->ObservedTransformNode != NULL )
  {
    this->ObservedTransformNode->RemoveObservers( vtkMRMLLinearTransformNode::TransformModifiedEvent, (vtkCommand*) this->GetMRMLNodesCallbackCommand() );
  }

  transformNode->AddObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent, (vtkCommand*) this->GetMRMLNodesCallbackCommand() );

  this->ObservedTransformNode = transformNode;
}


// Let the selection node singleton keep track of this - we will just offer convenience functions
void vtkSlicerLinearObjectRegistrationLogic
::SetActiveCollectionNode( vtkMRMLLORLinearObjectCollectionNode* newActiveCollectionNode )
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode != NULL )
  {
    selectionNode->SetAttribute( "ActiveLinearObjectCollectionID", newActiveCollectionNode->GetID() );
  }
}


vtkMRMLLORLinearObjectCollectionNode* vtkSlicerLinearObjectRegistrationLogic
::GetActiveCollectionNode()
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode == NULL )
  {
    return NULL;
  }

  const char* activeCollectionNodeID = selectionNode->GetAttribute( "ActiveLinearObjectCollectionID" );
  return vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( activeCollectionNodeID ) );
}



// This makes the module ready to record - see the ProcessMRMLNodesEvents for this class to see whether or not any recording actually happens
void vtkSlicerLinearObjectRegistrationLogic
::InitializeActivePositionBuffer( std::string collectType )
{
  this->ActivePositionBuffer->Clear();
  this->CollectType = collectType;
}


void vtkSlicerLinearObjectRegistrationLogic
::FinalizeActivePositionBuffer()
{
  if ( this->ActivePositionBuffer == NULL )
  {
    return;
  }

  vtkSmartPointer< vtkMRMLLORLinearObjectNode > currentLinearObject;

  if ( this->CollectType.compare( "Reference" ) == 0 )
  {
    currentLinearObject = this->PositionBufferToLinearObject( this->ActivePositionBuffer, REFERENCE_DOF );
  }
  else if ( this->CollectType.compare( "Point" ) == 0 )
  {
    currentLinearObject = this->PositionBufferToLinearObject( this->ActivePositionBuffer, POINT_DOF );
  }
  else if ( this->CollectType.compare( "Line" ) == 0 )
  {
    currentLinearObject = this->PositionBufferToLinearObject( this->ActivePositionBuffer, LINE_DOF );
  }
  else if ( this->CollectType.compare( "Plane" ) == 0 )
  {
    currentLinearObject = this->PositionBufferToLinearObject( this->ActivePositionBuffer, PLANE_DOF );
  }
  else
  {
    currentLinearObject = this->PositionBufferToLinearObject( this->ActivePositionBuffer, UNKNOWN_DOF );
  }

  if ( currentLinearObject == NULL )
  {
    this->ActivePositionBuffer->Clear();
    return;
  }

  currentLinearObject->SetPositionBuffer( this->ActivePositionBuffer->DeepCopy() );
  this->InsertNewLinearObject( currentLinearObject );
  this->ActivePositionBuffer->Clear();
}


void vtkSlicerLinearObjectRegistrationLogic
::InsertNewLinearObject( vtkMRMLLORLinearObjectNode* linearObject )
{
  if ( this->GetActiveCollectionNode() == NULL )
  {
    return;
  }
   
  this->GetActiveCollectionNode()->AddLinearObject( linearObject );
  // TODO: Need to insert the current linear object into the correct slot in the collection
}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkMRMLLORLinearObjectNode > vtkSlicerLinearObjectRegistrationLogic
::PositionBufferToLinearObject( vtkMRMLLORPositionBufferNode* positionBuffer, int dof )
{
  if ( positionBuffer == NULL || positionBuffer->Size() == 0 )
  {
    return NULL;
  }

  std::vector<double> centroid = positionBuffer->CalculateCentroid();
  vnl_matrix<double>* cov = positionBuffer->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( vtkMRMLLORPositionNode::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Grab only the most important eigenvectors
  std::vector<double> Eigenvector1( vtkMRMLLORPositionNode::SIZE, 0.0 ); // Smallest
  std::vector<double> Eigenvector2( vtkMRMLLORPositionNode::SIZE, 0.0 ); // Medium
  std::vector<double> Eigenvector3( vtkMRMLLORPositionNode::SIZE, 0.0 ); // Largest

  Eigenvector1.at(0) = eigenvectors.get( 0, 0 );
  Eigenvector1.at(1) = eigenvectors.get( 1, 0 );
  Eigenvector1.at(2) = eigenvectors.get( 2, 0 );

  Eigenvector2.at(0) = eigenvectors.get( 0, 1 );
  Eigenvector2.at(1) = eigenvectors.get( 1, 1 );
  Eigenvector2.at(2) = eigenvectors.get( 2, 1 );

  Eigenvector3.at(0) = eigenvectors.get( 0, 2 );
  Eigenvector3.at(1) = eigenvectors.get( 1, 2 );
  Eigenvector3.at(2) = eigenvectors.get( 2, 2 );

  // The threshold noise is twice the extraction threshold
  if ( dof == REFERENCE_DOF )
  {
    vtkMRMLLORReferenceNode* referenceObject = vtkMRMLLORReferenceNode::New( centroid );
    return vtkSmartPointer< vtkMRMLLORReferenceNode >::Take( referenceObject );
  }
  if ( dof == POINT_DOF )
  {
    vtkMRMLLORPointNode* pointObject = vtkMRMLLORPointNode::New( centroid );
    return vtkSmartPointer< vtkMRMLLORPointNode >::Take( pointObject );
  }
  if ( dof == LINE_DOF )
  {
    vtkMRMLLORLineNode* lineObject = vtkMRMLLORLineNode::New( centroid, vtkMRMLLORVectorMath::Add( centroid, Eigenvector3 ) );
    return vtkSmartPointer< vtkMRMLLORLineNode >::Take( lineObject );
  }
  if ( dof == PLANE_DOF )
  {
    vtkMRMLLORPlaneNode* planeObject = vtkMRMLLORPlaneNode::New( centroid, vtkMRMLLORVectorMath::Add( centroid, Eigenvector2 ), vtkMRMLLORVectorMath::Add( centroid, Eigenvector3 ) );
    return vtkSmartPointer< vtkMRMLLORPlaneNode >::Take( planeObject );
  }

  return NULL; // TODO: Do something more productive if the dof is unknown (ie determine the dof automatically)...
}


void vtkSlicerLinearObjectRegistrationLogic
::MatchCollections( vtkMRMLLORLinearObjectCollectionNode* collection0, vtkMRMLLORLinearObjectCollectionNode* collection1, bool removeUnmatched )
{
  // Get the reference from both to calculate all of their objects' signatures
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > referenceCollection0 = this->GetReferences( collection0 );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > referenceCollection1 = this->GetReferences( collection1 );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > nonReferenceCollection0 = this->GetNonReferences( collection0 );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > nonReferenceCollection1 = this->GetNonReferences( collection1 );

  nonReferenceCollection0->CalculateSignature( referenceCollection0 );
  nonReferenceCollection1->CalculateSignature( referenceCollection1 );

  // Now, stored the matched and unmatched collections
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > matchedCollection0 = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > matchedCollection1 = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > unmatchedCollection0 = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > unmatchedCollection1 = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();


  // This changes the from collection, but creates a new to collection - we want to change the old to collection
  if ( nonReferenceCollection0->Size() == 0 || nonReferenceCollection1->Size() == 0 || referenceCollection0->Size() != referenceCollection1->Size() )
  {
    return; // The original collections are unchanged
  }

  for( int i = 0; i < nonReferenceCollection0->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentObject0 = nonReferenceCollection0->GetLinearObject( i );
    double bestDistance = std::numeric_limits<double>::max();
    int bestIndex = -1;

    if ( currentObject0 == NULL )
    {
      continue;
    }

    for ( int j = 0; j < nonReferenceCollection1->Size(); j++ )
    {
      vtkMRMLLORLinearObjectNode* currentObject1 = nonReferenceCollection1->GetLinearObject( j );

      if ( currentObject1 == NULL )
      {
        continue;
      }

      if ( currentObject0->GetSignature().size() != currentObject1->GetSignature().size() )
      {
        continue;
      }
      
      double currentDistance = vtkMRMLLORVectorMath::Distance( currentObject0->GetSignature(), currentObject1->GetSignature() );

      if ( currentDistance < bestDistance )
      {
        bestDistance = currentDistance;
        bestIndex = j;
      }
    }// for over collection 1

    if ( bestDistance < MATCHING_THRESHOLD )
    {
      matchedCollection0->AddLinearObject( currentObject0 );
      matchedCollection1->AddLinearObject( nonReferenceCollection1->GetLinearObject( bestIndex ) );
      nonReferenceCollection0->RemoveLinearObject( i );
      nonReferenceCollection1->RemoveLinearObject( bestIndex );
    }
    else
    {
      unmatchedCollection0->AddLinearObject( currentObject0 );
      unmatchedCollection1->AddLinearObject( NULL );
      nonReferenceCollection0->RemoveLinearObject( i );
    }

  }// for over collection 0

  // Deal with any possible remaining linear objects from collection1
  for ( int i = 0; i < nonReferenceCollection1->Size(); i++ )
  {
    if ( nonReferenceCollection1->GetLinearObject( i ) != NULL )
    {
      unmatchedCollection1->AddLinearObject( nonReferenceCollection1->GetLinearObject( i ) );
      unmatchedCollection0->AddLinearObject( NULL );
      nonReferenceCollection1->RemoveLinearObject( i );
    }
  }

  // Now put everything back together
  collection0->Copy( referenceCollection0 );
  collection0->Concatenate( matchedCollection0 );

  collection1->Copy( referenceCollection1 );
  collection1->Concatenate( matchedCollection1 );

  if ( ! removeUnmatched )
  {
    collection0->Concatenate( unmatchedCollection0 );
    collection1->Concatenate( unmatchedCollection1 );
  }

}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetReferences( vtkMRMLLORLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > referenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject != NULL && currentLinearObject->GetType().compare( "Reference" ) == 0 )
    {
      referenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return referenceCollection;
}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetNonReferences( vtkMRMLLORLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > nonReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject != NULL && currentLinearObject->GetType().compare( "Reference" ) != 0 )
    {
      nonReferenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return nonReferenceCollection;
}


// Show/hide the model associated with a linear object
void vtkSlicerLinearObjectRegistrationLogic
::ToggleLinearObjectModelVisibility( vtkMRMLLORLinearObjectNode* linearObject )
{
  // First, if the model exists then show it
  std::string modelID = linearObject->GetModelID().c_str();
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( modelID.c_str() ) );

  if ( modelNode != NULL && modelNode->GetDisplayNode() != NULL )
  {
    modelNode->GetDisplayNode()->SetVisibility( ! modelNode->GetDisplayNode()->GetVisibility() );
    return;
  }

  // Otherwise, create a new model
  vtkPolyData* linearObjectPolyData = linearObject->CreateModelPolyData();

  vtkMRMLModelNode* linearObjectModel = vtkMRMLModelNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelNode" ) );
  linearObjectModel->SetAndObservePolyData( linearObjectPolyData );
  std::string modelName = linearObject->GetName() + "Model";
  linearObjectModel->SetName( modelName.c_str() );
  linearObjectModel->SetScene( this->GetMRMLScene() );

  vtkMRMLModelDisplayNode* linearObjectModelDisplay = vtkMRMLModelDisplayNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelDisplayNode" ) );
  linearObjectModelDisplay->SetScene( this->GetMRMLScene() );
  linearObjectModelDisplay->SetInputPolyData( linearObjectModel->GetPolyData() );
  linearObjectModelDisplay->SetVisibility( true );
  linearObjectModelDisplay->BackfaceCullingOff();

  this->GetMRMLScene()->AddNode( linearObjectModelDisplay );
  this->GetMRMLScene()->AddNode( linearObjectModel );

  linearObjectModel->SetAndObserveDisplayNodeID( linearObjectModelDisplay->GetID() );

  // And let the linear object store the model's ID
  linearObject->SetModelID( linearObjectModel->GetID() );
}


// TODO: May be this function signature is too long...
void vtkSlicerLinearObjectRegistrationLogic
::GetFromAndToCollections( vtkMRMLLORLinearObjectCollectionNode* fromCollection, vtkMRMLLORLinearObjectCollectionNode* fromReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* fromPointCollection, vtkMRMLLORLinearObjectCollectionNode* fromLineCollection, vtkMRMLLORLinearObjectCollectionNode* fromPlaneCollection,
                          vtkMRMLLORLinearObjectCollectionNode* toCollection, vtkMRMLLORLinearObjectCollectionNode* toReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* toPointCollection, vtkMRMLLORLinearObjectCollectionNode* toLineCollection, vtkMRMLLORLinearObjectCollectionNode* toPlaneCollection )
{
  // Just add to the linear object collections based on the type attribute
  // It doesn't matter which collection we take from - if there's no match then there's no point in going through
  for ( int i = 0; i < fromCollection->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentFromLinearObject = fromCollection->GetLinearObject( i );
    vtkMRMLLORLinearObjectNode* currentToLinearObject = toCollection->GetLinearObject( i );

    if ( currentFromLinearObject == NULL || currentToLinearObject == NULL )
    {
      continue;
    }

    if ( currentFromLinearObject->GetType().compare( "Reference" ) == 0 && currentToLinearObject->GetType().compare( "Reference" ) == 0 )
    {
      fromReferenceCollection->AddLinearObject( currentFromLinearObject );
      toReferenceCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( "Point" ) == 0 && currentToLinearObject->GetType().compare( "Point" ) == 0 )
    {
      fromPointCollection->AddLinearObject( currentFromLinearObject );
      toPointCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( "Line" ) == 0 && currentToLinearObject->GetType().compare( "Line" ) == 0 )
    {
      fromLineCollection->AddLinearObject( currentFromLinearObject );
      toLineCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( "Plane" ) == 0 && currentToLinearObject->GetType().compare( "Plane" ) == 0 )
    {
      fromPlaneCollection->AddLinearObject( currentFromLinearObject );
      toPlaneCollection->AddLinearObject( currentToLinearObject );
    }

  }

}


void vtkSlicerLinearObjectRegistrationLogic
::CalculateTransform( vtkMRMLNode* node )
{
  vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( node );
  if ( linearObjectRegistrationNode == NULL )
  {
    this->SetOutputMessage( "Failed to find module node." );
    return;
  }

  // All returns should be proceeded by a SetOutputMessage function
  // Re-add the observer before the SetOutputMessage function
  // Prevents check for modified events when matching
  linearObjectRegistrationNode->RemoveObservers( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );


  // This is ok without smart pointer
  vtkMRMLLORLinearObjectCollectionNode* fromCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetFromCollectionID() ) );
  vtkMRMLLORLinearObjectCollectionNode* toCollection = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetToCollectionID() ) );
  vtkMRMLLinearTransformNode* outputTransform = vtkMRMLLinearTransformNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetOutputTransformID() ) );
  // This is not
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromMatchedCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toMatchedCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  if ( fromCollection == NULL || toCollection == NULL )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( "One or more linear object collections not defined." );
    return;
  }

  if ( linearObjectRegistrationNode->GetAutomaticMatch().compare( "True" ) == 0 )
  {
    // Match the collections (for the interface)
    this->MatchCollections( fromCollection, toCollection, false );
    // Match and remove collections (for the algorithm)
    fromMatchedCollection->Copy( fromCollection );
    toMatchedCollection->Copy( toCollection );
    this->MatchCollections( fromMatchedCollection, toMatchedCollection, true );
  }

  if ( outputTransform == NULL )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( "Output transform is not defined." );
    return;
  }
  
  // Grab the linear object collections
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromPointCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromLineCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromPlaneCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toPointCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toLineCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toPlaneCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  
  this->GetFromAndToCollections( fromMatchedCollection, fromReferenceCollection, fromPointCollection, fromLineCollection, fromPlaneCollection, 
    toMatchedCollection, toReferenceCollection, toPointCollection, toLineCollection, toPlaneCollection );
  // Note: The number of each type of collection should be the same

  if ( fromReferenceCollection->Size() == 0 || toReferenceCollection->Size() == 0 || fromReferenceCollection->Size() != toReferenceCollection->Size() )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( "Failed: Could not find appropriate references." );
    return;
  }

  // The matching should already be done (in real-time)

  // Calculate the centroids
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromCentroidCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  fromCentroidCollection->Concatenate( fromPointCollection );
  fromCentroidCollection->Concatenate( fromLineCollection );
  fromCentroidCollection->Concatenate( fromPlaneCollection );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toCentroidCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  toCentroidCollection->Concatenate( toPointCollection );
  toCentroidCollection->Concatenate( toLineCollection );
  toCentroidCollection->Concatenate( toPlaneCollection );

  std::vector<double> fromCentroid, toCentroid; 
  try
  {
    fromCentroid = fromCentroidCollection->CalculateCentroid();
	toCentroid = toCentroidCollection->CalculateCentroid();
  }
  catch( std::logic_error e )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( e.what() );
	return;
  }

  std::vector<double> negativeFromCentroid( fromCentroid.size(), 0.0 );
  negativeFromCentroid = vtkMRMLLORVectorMath::Subtract( negativeFromCentroid, fromCentroid );

  std::vector<double> negativeToCentroid( toCentroid.size(), 0.0 );
  negativeToCentroid = vtkMRMLLORVectorMath::Subtract( negativeToCentroid, toCentroid );


  // Now, translate everything by the negative centroid
  fromPointCollection->Translate( negativeFromCentroid );
  fromLineCollection->Translate( negativeFromCentroid );
  fromPlaneCollection->Translate( negativeFromCentroid );
  fromReferenceCollection->Translate( negativeFromCentroid );

  toPointCollection->Translate( negativeToCentroid );
  toLineCollection->Translate( negativeToCentroid );
  toPlaneCollection->Translate( negativeToCentroid );
  toReferenceCollection->Translate( negativeToCentroid );


  // Next, add the base points to the final point observation vectors
  std::vector<double> BlankVector( negativeToCentroid.size(), 0.0 );

  vtkSmartPointer< vtkMRMLLORPositionBufferNode > FromPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
  for ( int i = 0; i < fromPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }

  vtkSmartPointer< vtkMRMLLORPositionBufferNode > ToPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
  for ( int i = 0; i < toPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkMRMLLORPositionNode::New( basePoint ) );
  }


  // Then, add the direction vector to the final point observation vectors
  /*
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    vtkMRMLLORLineNode* CurrentGeometryObject = vtkMRMLLORLineNode::SafeDownCast( fromLineCollection->GetLinearObject(i) );
    std::vector<double> scaledDirection = vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() );
    FromPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledDirection ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    vtkMRMLLORPlaneNode* CurrentGeometryObject = vtkMRMLLORPlaneNode::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    std::vector<double> scaledNormal = vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() );
    FromPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledNormal ) );
  }
  */

  // TODO: Fix memory leaks
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    

    vtkMRMLLORLineNode* CurrentToObject = vtkMRMLLORLineNode::SafeDownCast( toLineCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) ) ;

    // Subtract direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    TempToCollection->Concatenate( toReferenceCollection );


    vtkMRMLLORLineNode* CurrentFromObject = vtkMRMLLORLineNode::SafeDownCast( fromLineCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentFromObject->GetDirection() ) );
    TempFromCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    TempFromCollection->Concatenate( fromReferenceCollection );

    // Do the matching
    this->MatchCollections( TempToCollection, TempFromCollection, true );

    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToNonReferenceCollection = this->GetNonReferences( TempToCollection );
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromNonReferenceCollection = this->GetNonReferences( TempFromCollection );
    
    if( TempToNonReferenceCollection->Size() > 0 && TempFromNonReferenceCollection->Size() > 0 )
    {
      // subtract off the projection of the origin to get just the direction vector back
      std::vector<double> scaledToDirection = vtkMRMLLORVectorMath::Subtract( TempToNonReferenceCollection->GetLinearObject(0)->GetBasePoint(), CurrentToObject->ProjectVector( BlankVector ) );
      ToPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledToDirection ) );
      std::vector<double> scaledFromDirection = vtkMRMLLORVectorMath::Subtract( TempFromNonReferenceCollection->GetLinearObject(0)->GetBasePoint(), CurrentFromObject->ProjectVector( BlankVector ) );
      FromPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledFromDirection ) );
    }
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    
    vtkMRMLLORPlaneNode* CurrentToObject = vtkMRMLLORPlaneNode::SafeDownCast( toPlaneCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    // Subtract direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );

    TempToCollection->Concatenate( toReferenceCollection );


    vtkMRMLLORPlaneNode* CurrentFromObject = vtkMRMLLORPlaneNode::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    
    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentFromObject->GetNormal() ) );
    TempFromCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    TempFromCollection->Concatenate( fromReferenceCollection );

    // Do the matching
    this->MatchCollections( TempToCollection, TempFromCollection, true );

    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToNonReferenceCollection = this->GetNonReferences( TempToCollection );
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromNonReferenceCollection = this->GetNonReferences( TempFromCollection );
    
    if( TempToNonReferenceCollection->Size() > 0 && TempFromNonReferenceCollection->Size() > 0 )
    {
      // subtract off the projection of the origin to get just the direction vector back
      std::vector<double> scaledToNormal = vtkMRMLLORVectorMath::Subtract( TempToNonReferenceCollection->GetLinearObject( 0 )->GetBasePoint(), CurrentToObject->ProjectVector( BlankVector ) );
      ToPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledToNormal ) );
      std::vector<double> scaledFromNormal = vtkMRMLLORVectorMath::Subtract( TempFromNonReferenceCollection->GetLinearObject( 0 )->GetBasePoint(), CurrentFromObject->ProjectVector( BlankVector ) );
      FromPositions->AddPosition( vtkMRMLLORPositionNode::New( scaledFromNormal ) );
    }
  }


  // Finally, calculate the registration
  vnl_matrix<double>* FromToToRotation;

  try
  {
    // TODO: Add check for collinearity - it is not done in the spherical registration method
    FromToToRotation = this->SphericalRegistration( FromPositions, ToPositions );
  }
  catch( std::logic_error e )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->OutputMessage = e.what();
	return;
  }

  vnl_matrix<double>* FromToToTranslation = this->TranslationalRegistration( fromCentroid, toCentroid, FromToToRotation ); 

  // And set the output matrix
  this->UpdateOutputTransform( outputTransform, FromToToRotation, FromToToTranslation );

  linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
  this->OutputMessage = "Success!";
}


void vtkSlicerLinearObjectRegistrationLogic
::UpdateOutputTransform( vtkMRMLLinearTransformNode* outputTransform, vnl_matrix<double>* newTransformMatrix )
{
  vtkMatrix4x4* outputMatrix = outputTransform->GetMatrixTransformToParent();

  outputMatrix->SetElement( 0, 0, newTransformMatrix->get( 0, 0 ) );
  outputMatrix->SetElement( 0, 1, newTransformMatrix->get( 0, 1 ) );
  outputMatrix->SetElement( 0, 2, newTransformMatrix->get( 0, 2 ) );
  outputMatrix->SetElement( 0, 3, newTransformMatrix->get( 0, 3 ) );

  outputMatrix->SetElement( 1, 0, newTransformMatrix->get( 1, 0 ) );
  outputMatrix->SetElement( 1, 1, newTransformMatrix->get( 1, 1 ) );
  outputMatrix->SetElement( 1, 2, newTransformMatrix->get( 1, 2 ) );
  outputMatrix->SetElement( 1, 3, newTransformMatrix->get( 1, 3 ) );

  outputMatrix->SetElement( 2, 0, newTransformMatrix->get( 2, 0 ) );
  outputMatrix->SetElement( 2, 1, newTransformMatrix->get( 2, 1 ) );
  outputMatrix->SetElement( 2, 2, newTransformMatrix->get( 2, 2 ) );
  outputMatrix->SetElement( 2, 3, newTransformMatrix->get( 2, 3 ) );

  outputMatrix->SetElement( 3, 0, 0 );
  outputMatrix->SetElement( 3, 1, 0 );
  outputMatrix->SetElement( 3, 2, 0 );
  outputMatrix->SetElement( 3, 3, 1 );
}


void vtkSlicerLinearObjectRegistrationLogic
::UpdateOutputTransform( vtkMRMLLinearTransformNode* outputTransform, vnl_matrix<double>* newRotationMatrix, vnl_matrix<double>* newTranslationVector )
{
  vtkMatrix4x4* outputMatrix = outputTransform->GetMatrixTransformToParent();

  outputMatrix->SetElement( 0, 0, newRotationMatrix->get( 0, 0 ) );
  outputMatrix->SetElement( 0, 1, newRotationMatrix->get( 0, 1 ) );
  outputMatrix->SetElement( 0, 2, newRotationMatrix->get( 0, 2 ) );
  outputMatrix->SetElement( 0, 3, newTranslationVector->get( 0, 0 ) );

  outputMatrix->SetElement( 1, 0, newRotationMatrix->get( 1, 0 ) );
  outputMatrix->SetElement( 1, 1, newRotationMatrix->get( 1, 1 ) );
  outputMatrix->SetElement( 1, 2, newRotationMatrix->get( 1, 2 ) );
  outputMatrix->SetElement( 1, 3, newTranslationVector->get( 1, 0 ) );

  outputMatrix->SetElement( 2, 0, newRotationMatrix->get( 2, 0 ) );
  outputMatrix->SetElement( 2, 1, newRotationMatrix->get( 2, 1 ) );
  outputMatrix->SetElement( 2, 2, newRotationMatrix->get( 2, 2 ) );
  outputMatrix->SetElement( 2, 3, newTranslationVector->get( 2, 0 ) );

  outputMatrix->SetElement( 3, 0, 0 );
  outputMatrix->SetElement( 3, 1, 0 );
  outputMatrix->SetElement( 3, 2, 0 );
  outputMatrix->SetElement( 3, 3, 1 );
}


vnl_matrix<double>* vtkSlicerLinearObjectRegistrationLogic
::SphericalRegistration( vtkMRMLLORPositionBufferNode* fromPoints, vtkMRMLLORPositionBufferNode* toPoints )
{
  // Assume that it is already mean zero
  const double CONDITION_THRESHOLD = 1e-3;

  // Let us construct the data matrix
  vnl_matrix<double>* DataMatrix = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE, 0.0 );

  if ( fromPoints->Size() != toPoints->Size() )
  {
    throw std::logic_error( "Failed - inconsistent number of points!" );
  }

  // Pick two dimensions, and find their data matrix entry
  for ( int d1 = 0; d1 < vtkMRMLLORPositionNode::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkMRMLLORPositionNode::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < fromPoints->Size(); i++ )
	  {
	    DataMatrix->put( d1, d2, DataMatrix->get( d1, d2 ) + fromPoints->GetPosition(i)->GetPositionVector().at(d1) * toPoints->GetPosition(i)->GetPositionVector().at(d2) );
	  }
	}
  }

  // Now we can calculate its svd
  vnl_svd<double>* SVDMatrix = new vnl_svd<double>( *DataMatrix, 0.0 );

  /*
  if ( SVDMatrix->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error( "Failed - spherical registration is ill-conditioned!" );
  }
  */

  return new vnl_matrix<double>( SVDMatrix->V() * SVDMatrix->U().transpose() );
}


vnl_matrix<double>* vtkSlicerLinearObjectRegistrationLogic
::TranslationalRegistration( std::vector<double> fromCentroid, std::vector<double> toCentroid, vnl_matrix<double>* rotation )
{
  // Make matrices out of the centroids
  vnl_matrix<double>* toMatrix = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, 1, 0.0 );
  vnl_matrix<double>* fromMatrix = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, 1, 0.0 );

  for ( int i = 0; i < vtkMRMLLORPositionNode::SIZE; i++ )
  {
    toMatrix->put( i, 0, toCentroid.at(i) );
	fromMatrix->put( i, 0, fromCentroid.at(i) );
  }

  return new vnl_matrix<double>( (*toMatrix) - (*rotation) * (*fromMatrix) );
}


/*
vnl_matrix<double>* vtkSlicerLinearObjectRegistrationLogic
::LinearObjectICP( vnl_matrix<double>* initialRotation )
{

  const int CONVERGENCE_THRESHOLD = 1e-6;
  double currError = -1;
  double prevError = -1;
  vnl_matrix<double>* currRotation = initialRotation;

  while ( prevError < 0 || abs( currError - prevError ) < CONVERGENCE_THRESHOLD )
  {
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > GeometryPoints = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > RecordPoints = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();

	prevError = currError;

	// Find the closest point on each linear object to each point so we can do spherical registration
    for ( int i = 0; i < this->PointPoints.size(); i++ )
	{
      for ( int j = 0; j < this->PointPoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->PointPoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->PointPoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkMRMLLORPositionNode > rotPoint = vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryPointBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( projection ) ) );

	  }
	}

    for ( int i = 0; i < this->LinePoints.size(); i++ )
	{
      for ( int j = 0; j < this->LinePoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->LinePoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->LinePoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkMRMLLORPositionNode > rotPoint = vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryLineBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( projection ) ) );

	  }
	}

    for ( int i = 0; i < this->PlanePoints.size(); i++ )
	{
      for ( int j = 0; j < this->PlanePoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->PlanePoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->PlanePoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkMRMLLORPositionNode > rotPoint = vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryPlaneBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( projection ) ) );

	  }
	}

	// Now perform the spherical registration and calculate the rms error
    currRotation = GeometryPoints->SphericalRegistration( RecordPoints );

    currError = 0;
	for ( int i = 0; i < GeometryPoints->Size(); i++ )
	{
	  // Rotate the observed point
      std::vector<double> currObservation = RecordPoints->GetObservation(i)->Observation;
      vtkSmartPointer< vtkMRMLLORPositionNode > rotPoint = vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( currObservation ) );
      rotPoint->Rotate( currRotation );

	  currError = currError + Distance( GeometryPoints->GetObservation(i)->Observation, rotPoint->Observation );
	}
	currError = sqrt( currError / GeometryPoints->Size() );

  }

  this->ErrorRMS = currError;
  return currRotation;

}
*/


// Node update methods ----------------------------------------------------------

void vtkSlicerLinearObjectRegistrationLogic
::ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData )
{
  // In case the module node is updated
  vtkMRMLLinearObjectRegistrationNode* lorNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( caller );
  // The caller must be a vtkMRMLLinearObjectRegistrationNode
  if ( lorNode != NULL )
  {
    this->CalculateTransform( lorNode ); // Will create modified event to update widget
  }

  // In case the observed transform node is updated
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( caller );
  // Must be a transform node
  if ( transformNode == NULL || this->ActivePositionBuffer == NULL )
  {
    return;
  }
  if ( strcmp( transformNode->GetID(), this->ObservedTransformNode->GetID() ) == 0 )
  {
    vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
    this->ActivePositionBuffer->AddPosition( vtkMRMLLORPositionNode::New( matrix ) );
  }
}


void vtkSlicerLinearObjectRegistrationLogic
::ProcessMRMLSceneEvents( vtkObject* caller, unsigned long event, void* callData )
{
  vtkMRMLScene* callerNode = vtkMRMLScene::SafeDownCast( caller );

  // If the added node was a fiducial registration wizard node then observe it
  vtkMRMLNode* addedNode = reinterpret_cast< vtkMRMLNode* >( callData );
  vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( addedNode );

  if ( event == vtkMRMLScene::NodeAddedEvent && linearObjectRegistrationNode != NULL )
  {
    // This will get called exactly once, and we will add the observer only once (since node is never replaced)
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    linearObjectRegistrationNode->UpdateScene( this->GetMRMLScene() );
    linearObjectRegistrationNode->ObserveAllReferenceNodes(); // This will update
    this->CalculateTransform( linearObjectRegistrationNode ); // Will create modified event to update widget
  }

}