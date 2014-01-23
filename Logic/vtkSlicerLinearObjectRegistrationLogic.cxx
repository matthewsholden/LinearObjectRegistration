
// LinearObjectRegistration Logic includes
#include "vtkSlicerLinearObjectRegistrationLogic.h"


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
::GetOutputMessage( std::string nodeID )
{
  return this->OutputMessages[ nodeID ];
}


void vtkSlicerLinearObjectRegistrationLogic
::SetOutputMessage( std::string nodeID, std::string newOutputMessage )
{
  this->OutputMessages[ nodeID ] = newOutputMessage;
  this->Modified();
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

  // Get number of eigenvectors with eigenvalues larger than the threshold
  double calculatedDOF = 0;
  for ( int i = 0; i < vtkMRMLLORPositionNode::SIZE; i++ )
  {
    if ( abs( eigenvalues.get( i ) ) > vtkMRMLLORConstants::NOISE_THRESHOLD )
    {
      calculatedDOF++;
    }
  }

  // The threshold noise is twice the extraction threshold
  if ( dof == vtkMRMLLORConstants::REFERENCE_DOF )
  {
    vtkMRMLLORReferenceNode* referenceObject = vtkMRMLLORReferenceNode::New( centroid );
    return vtkSmartPointer< vtkMRMLLORReferenceNode >::Take( referenceObject );
  }
  if ( dof == vtkMRMLLORConstants::POINT_DOF || dof == vtkMRMLLORConstants::UNKNOWN_DOF && calculatedDOF == vtkMRMLLORConstants::POINT_DOF )
  {
    vtkMRMLLORPointNode* pointObject = vtkMRMLLORPointNode::New( centroid );
    return vtkSmartPointer< vtkMRMLLORPointNode >::Take( pointObject );
  }
  if ( dof == vtkMRMLLORConstants::LINE_DOF || dof == vtkMRMLLORConstants::UNKNOWN_DOF && calculatedDOF == vtkMRMLLORConstants::LINE_DOF )
  {
    vtkMRMLLORLineNode* lineObject = vtkMRMLLORLineNode::New( centroid, vtkMRMLLORVectorMath::Add( centroid, Eigenvector3 ) );
    return vtkSmartPointer< vtkMRMLLORLineNode >::Take( lineObject );
  }
  if ( dof == vtkMRMLLORConstants::PLANE_DOF || dof == vtkMRMLLORConstants::UNKNOWN_DOF && calculatedDOF >= vtkMRMLLORConstants::PLANE_DOF )
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

    if ( bestDistance < vtkMRMLLORConstants::MATCHING_THRESHOLD )
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
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "Failed to find module node." ); // This should never happen
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
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "One or more linear object collections not defined." );
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
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "Output transform is not defined." );
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
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "Could not find appropriate references." );
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
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), e.what() );
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
    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) ) ;

    // Subtract direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    TempToCollection->Concatenate( toReferenceCollection );


    vtkMRMLLORLineNode* CurrentFromObject = vtkMRMLLORLineNode::SafeDownCast( fromLineCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentFromObject->GetDirection() ) );
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
    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );
    
    // Subtract direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkMRMLLORPointNode::New( testVector ) );

    TempToCollection->Concatenate( toReferenceCollection );


    vtkMRMLLORPlaneNode* CurrentFromObject = vtkMRMLLORPlaneNode::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    
    // Add direction vector to projection of origin
    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORConstants::DIRECTION_SCALE, CurrentFromObject->GetNormal() ) );
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
  vtkSmartPointer< vtkMatrix4x4 > FromToToTransform = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkSmartPointer< vtkMatrix4x4 > FromToToRotation = NULL;
  std::vector<double> FromToToTranslation;

  try
  {
    // TODO: Add check for collinearity - it is not done in the spherical registration method
    FromToToRotation = this->SphericalRegistration( FromPositions, ToPositions, NULL );
  }
  catch( std::logic_error e )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), e.what() );
	return;
  }

  // And set the output matrix
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromICPTACollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  fromICPTACollection->Concatenate( fromPlaneCollection );
  fromICPTACollection->Concatenate( fromLineCollection );
  fromICPTACollection->Concatenate( fromPointCollection );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toICPTACollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  toICPTACollection->Concatenate( toPlaneCollection );
  toICPTACollection->Concatenate( toLineCollection );
  toICPTACollection->Concatenate( toPointCollection );

  double rmsError = this->LinearObjectICPTA( fromICPTACollection, toICPTACollection, FromToToRotation, FromToToTransform );
  this->MatrixRotationPart( FromToToTransform, FromToToRotation );
  FromToToTranslation = this->MatrixTranslationPart( FromToToTransform );
  FromToToTranslation = this->TranslationalRegistration( fromCentroid, vtkMRMLLORVectorMath::Add( toCentroid, FromToToTranslation ), FromToToRotation ); 
  this->RotationTranslationToMatrix( FromToToRotation, FromToToTranslation, FromToToTransform );

  this->UpdateOutputTransform( outputTransform, FromToToTransform );

  std::stringstream successMessage;
  successMessage << "Success! RMS Error: " << rmsError;
  this->SetOutputMessage( linearObjectRegistrationNode->GetID(), successMessage.str() );
}


void vtkSlicerLinearObjectRegistrationLogic
::UpdateOutputTransform( vtkMRMLLinearTransformNode* outputTransform, vtkMatrix4x4* newTransformMatrix )
{
  vtkMatrix4x4* outputMatrix = outputTransform->GetMatrixTransformToParent();
  outputMatrix->DeepCopy( newTransformMatrix );
}


// This is a pure rotation
vtkSmartPointer< vtkMatrix4x4 > vtkSlicerLinearObjectRegistrationLogic
::SphericalRegistration( vtkMRMLLORPositionBufferNode* fromPoints, vtkMRMLLORPositionBufferNode* toPoints, vtkMatrix4x4* currentFromToToTransform )
{
  // Assume that it is already mean zero
  const double CONDITION_THRESHOLD = 1e-3;

  // Calculate the translation to be applied to the from positions
  vtkSmartPointer< vtkMatrix4x4 > translation = vtkSmartPointer< vtkMatrix4x4 >::New();
  if ( currentFromToToTransform != NULL )
  {
    vtkSmartPointer< vtkMatrix4x4 > rotationInverse = vtkSmartPointer< vtkMatrix4x4 >::New();
    this->MatrixRotationPart( currentFromToToTransform, rotationInverse );
    rotationInverse->Invert();
    vtkMatrix4x4::Multiply4x4( rotationInverse, currentFromToToTransform, translation );
  }

  // Let us construct the data matrix
  vnl_matrix<double>* DataMatrix = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE + 1, vtkMRMLLORPositionNode::SIZE + 1, 0.0 );

  if ( fromPoints->Size() != toPoints->Size() )
  {
    throw std::logic_error( "Inconsistent number of points!" ); // This should never happen
  }

  // Pick two dimensions, and find their data matrix entry
  for ( int d1 = 0; d1 < vtkMRMLLORPositionNode::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkMRMLLORPositionNode::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < fromPoints->Size(); i++ )
	  {
        double fromCoordinate = fromPoints->GetPosition(i)->GetPositionVector().at( d1 );
        double toCoordinate = toPoints->GetPosition(i)->GetPositionVector().at( d2 );

        if ( currentFromToToTransform != NULL )
        {
          fromCoordinate += translation->GetElement( d1, 3 );
        }

	    DataMatrix->put( d1, d2, DataMatrix->get( d1, d2 ) + fromCoordinate * toCoordinate );
	  }
	}
  }

  // Now we can calculate its svd
  vnl_svd<double>* SVDMatrix = new vnl_svd<double>( *DataMatrix, 0.0 );
  vnl_matrix<double>* rotationMatrix = new vnl_matrix<double>( SVDMatrix->V() * SVDMatrix->U().transpose() );

  vtkSmartPointer< vtkMatrix4x4 > rotation = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->VNLMatrixToVTKMatrix( rotationMatrix, rotation );

  return rotation;
}


std::vector<double> vtkSlicerLinearObjectRegistrationLogic
::TranslationalRegistration( std::vector<double> fromCentroid, std::vector<double> toCentroid, vtkMatrix4x4* rotation )
{
  vtkSmartPointer< vtkMRMLLORPositionNode > tempPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
  tempPosition->SetPositionVector( fromCentroid );
  tempPosition->Transform( rotation );

  std::vector<double> translation = vtkMRMLLORVectorMath::Subtract( toCentroid, tempPosition->GetPositionVector() );
  return translation;
}


std::vector<double> vtkSlicerLinearObjectRegistrationLogic
::TranslationalAdjustment( vtkMRMLLORPositionBufferNode* fromPositions, vtkMRMLLORPositionBufferNode* toPositions, vtkMatrix4x4* currentFromToToTransform )
{
  // Note: The linear objects should already be matched
  if ( fromPositions->Size() != toPositions->Size() )
  {
    return std::vector<double>( 0, 0.0 );
  }

  std::vector<double> sumTranslations( vtkMRMLLORPositionNode::SIZE, 0.0 );
  std::vector<double> sumMagnitudes( vtkMRMLLORPositionNode::SIZE, 0.0 );

  for ( int i = 0; i < fromPositions->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORPositionNode > transformedPosition = fromPositions->GetPosition( i )->DeepCopy();
    transformedPosition->Transform( currentFromToToTransform );

    std::vector<double> fromVector = transformedPosition->GetPositionVector();
    std::vector<double> toVector = toPositions->GetPosition( i )->GetPositionVector();

    std::vector<double> difference = vtkMRMLLORVectorMath::Subtract( fromVector, toVector );
    
    sumTranslations = vtkMRMLLORVectorMath::Add( sumTranslations, difference );
    sumMagnitudes = vtkMRMLLORVectorMath::Add( sumMagnitudes, vtkMRMLLORVectorMath::Abs( vtkMRMLLORVectorMath::Normalize( difference ) ) );
  }

  std::vector<double> newTranslation( vtkMRMLLORPositionNode::SIZE, 0.0 );
  for ( int i = 0; i < vtkMRMLLORPositionNode::SIZE; i++ )
  {
    newTranslation.at( i ) = sumTranslations.at( i ) / sumMagnitudes.at( i );
  }

  // Now put the translation back into the from coordinate system
  vtkSmartPointer< vtkMatrix4x4 > currentFromToToRotationInverse = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->MatrixRotationPart( currentFromToToTransform, currentFromToToRotationInverse );
  currentFromToToRotationInverse->Invert();

  vtkSmartPointer< vtkMRMLLORPositionNode > tempPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
  tempPosition->SetPositionVector( newTranslation );
  tempPosition->Transform( currentFromToToRotationInverse );
  newTranslation = tempPosition->GetPositionVector();

  return newTranslation;
}


void vtkSlicerLinearObjectRegistrationLogic
::FindClosestPositions( vtkMRMLLORLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLORLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* currentFromToToTransform,
                    vtkMRMLLORPositionBufferNode* fromPositions, vtkMRMLLORPositionBufferNode* toPositions )
{
  // Note: The linear objects should already be matched
  if ( fromLinearObjects->Size() != toLinearObjects->Size() )
  {
    return;
  }

  // Note: The != serves as an XOR for booleans
  vtkMRMLLORLinearObjectCollectionNode* bufferfulLinearObjects = NULL;
  vtkMRMLLORLinearObjectCollectionNode* bufferlessLinearObjects = NULL;
  vtkMRMLLORPositionBufferNode* bufferfulPositions = NULL;
  vtkMRMLLORPositionBufferNode* bufferlessPositions = NULL;
  vtkSmartPointer< vtkMatrix4x4 > bufferfulToBufferlessTransform = vtkSmartPointer< vtkMatrix4x4 >::New();
  bufferfulToBufferlessTransform->DeepCopy( currentFromToToTransform );
  if ( fromLinearObjects->AllHavePositionBuffers() && ! toLinearObjects->AllHavePositionBuffers() )
  {
    bufferfulLinearObjects = fromLinearObjects;
    bufferlessLinearObjects = toLinearObjects;
    bufferfulPositions = fromPositions;
    bufferlessPositions = toPositions;
  }
  if ( ! fromLinearObjects->AllHavePositionBuffers() && toLinearObjects->AllHavePositionBuffers() )
  {
    bufferfulLinearObjects = toLinearObjects;
    bufferlessLinearObjects = fromLinearObjects;
    bufferfulPositions = toPositions;
    bufferlessPositions = fromPositions;
    bufferfulToBufferlessTransform->Invert();
  }
  if ( fromLinearObjects->AllHavePositionBuffers() == toLinearObjects->AllHavePositionBuffers() )
  {
    return;
  }


  // Here is the work
  for ( int i = 0; i < bufferlessLinearObjects->Size(); i++ )
  {

    vtkMRMLLORPositionBufferNode* currentPositionBuffer = bufferfulLinearObjects->GetLinearObject( i )->GetPositionBuffer();
    vtkMRMLLORLinearObjectNode* currentLinearObject = bufferlessLinearObjects->GetLinearObject( i );

    for( int j = 0; j < currentPositionBuffer->Size(); j++ )
    {
      // Transform the point
      vtkMRMLLORPositionNode* currentPosition = currentPositionBuffer->GetPosition( j );
      vtkSmartPointer< vtkMRMLLORPositionNode > transformedPosition = currentPosition->DeepCopy();
      transformedPosition->Transform( bufferfulToBufferlessTransform );

      std::vector<double> projectedVector = currentLinearObject->ProjectVector( transformedPosition->GetPositionVector() );

      vtkSmartPointer< vtkMRMLLORPositionNode > closestPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
      closestPosition->SetPositionVector( projectedVector );

      // Add the points to the position buffers for registration
      bufferfulPositions->AddPosition( currentPosition );
      bufferlessPositions->AddPosition( closestPosition );
    }
  }

}


vtkSmartPointer< vtkMatrix4x4 > vtkSlicerLinearObjectRegistrationLogic
::CombineRotationAndTranslation( vtkMatrix4x4* rotation, std::vector<double> translation )
{
  vtkSmartPointer< vtkMRMLLORPositionNode > tempPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
  tempPosition->SetPositionVector( translation );
  tempPosition->Transform( rotation );
  std::vector<double> rotatedTranslation = tempPosition->GetPositionVector();

  vtkSmartPointer< vtkMatrix4x4 > totalTransform = vtkSmartPointer< vtkMatrix4x4 >::New();

  totalTransform->SetElement( 0, 0, rotation->GetElement( 0, 0 ) );
  totalTransform->SetElement( 0, 1, rotation->GetElement( 0, 1 ) );
  totalTransform->SetElement( 0, 2, rotation->GetElement( 0, 2 ) );
  totalTransform->SetElement( 0, 3, -rotatedTranslation.at( 0 ) );

  totalTransform->SetElement( 1, 0, rotation->GetElement( 1, 0 ) );
  totalTransform->SetElement( 1, 1, rotation->GetElement( 1, 1 ) );
  totalTransform->SetElement( 1, 2, rotation->GetElement( 1, 2 ) );
  totalTransform->SetElement( 1, 3, -rotatedTranslation.at( 1 ) );

  totalTransform->SetElement( 2, 0, rotation->GetElement( 2, 0 ) );
  totalTransform->SetElement( 2, 1, rotation->GetElement( 2, 1 ) );
  totalTransform->SetElement( 2, 2, rotation->GetElement( 2, 2 ) );
  totalTransform->SetElement( 2, 3, -rotatedTranslation.at( 2 ) );

  totalTransform->SetElement( 3, 0, 0.0 );
  totalTransform->SetElement( 3, 1, 0.0 );
  totalTransform->SetElement( 3, 2, 0.0 );
  totalTransform->SetElement( 3, 3, 1.0 );

  return totalTransform;
}


void vtkSlicerLinearObjectRegistrationLogic
::VNLMatrixToVTKMatrix( vnl_matrix<double>* vnlMatrix, vtkMatrix4x4* vtkMatrix )
{
  if ( vnlMatrix->rows() != 4 || vnlMatrix->cols() != 4 )
  {
    return;
  }

  for ( int i = 0; i < 4; i++ )
  {
    for ( int j = 0; j < 4; j++ )
    {
      vtkMatrix->SetElement( i, j, vnlMatrix->get( i, j ) );
    }
  }

}

void vtkSlicerLinearObjectRegistrationLogic
::MatrixRotationPart( vtkMatrix4x4* matrix, vtkMatrix4x4* rotation )
{
  rotation->Identity();

  for ( int i = 0; i < 3; i++ )
  {
    for ( int j = 0; j < 3; j++ )
    {
      rotation->SetElement( i, j, matrix->GetElement( i, j ) );
    }
  }

}


std::vector<double> vtkSlicerLinearObjectRegistrationLogic
::MatrixTranslationPart( vtkMatrix4x4* matrix )
{
  std::vector<double> translation( vtkMRMLLORPositionNode::SIZE, 0.0 );

  translation.at( 0 ) = matrix->GetElement( 0, 3 );
  translation.at( 1 ) = matrix->GetElement( 1, 3 );
  translation.at( 2 ) = matrix->GetElement( 2, 3 );

  return translation;
}


void vtkSlicerLinearObjectRegistrationLogic
::RotationTranslationToMatrix( vtkMatrix4x4* rotation, std::vector<double> translation, vtkMatrix4x4* matrix )
{
  matrix->DeepCopy( rotation );
  matrix->SetElement( 0, 3, translation.at( 0 ) );
  matrix->SetElement( 1, 3, translation.at( 1 ) );
  matrix->SetElement( 2, 3, translation.at( 2 ) );
}




double vtkSlicerLinearObjectRegistrationLogic
::LinearObjectICPTA( vtkMRMLLORLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLORLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* initialRotation, vtkMatrix4x4* calculatedMatrix )
{

  const double CONVERGENCE_THRESHOLD = 1e-3;
  double currError = std::numeric_limits<double>::max() / 2;
  double prevError = std::numeric_limits<double>::max();

  vtkSmartPointer< vtkMatrix4x4 > estimatedRotation = initialRotation;
  std::vector<double> estimatedTranslation( 3, 0.0 );
  vtkSmartPointer< vtkMatrix4x4 > estimatedMatrix;

  while ( abs( currError - prevError ) > CONVERGENCE_THRESHOLD )
  {
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > fromPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > toPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();

	prevError = currError;

    // Translational adjustment
    estimatedMatrix = this->CombineRotationAndTranslation( estimatedRotation, estimatedTranslation );
    this->FindClosestPositions( fromLinearObjects, toLinearObjects, estimatedMatrix, fromPositions, toPositions );
    estimatedTranslation = vtkMRMLLORVectorMath::Add( estimatedTranslation, this->TranslationalAdjustment( fromPositions, toPositions, estimatedMatrix ) );

    // Spherical recalculation
    estimatedMatrix = this->CombineRotationAndTranslation( estimatedRotation, estimatedTranslation );
    this->FindClosestPositions( fromLinearObjects, toLinearObjects, estimatedMatrix, fromPositions, toPositions );
    estimatedRotation = this->SphericalRegistration( fromPositions, toPositions, estimatedMatrix );

    // Calculate rms-error
    estimatedMatrix = this->CombineRotationAndTranslation( estimatedRotation, estimatedTranslation );
    this->FindClosestPositions( fromLinearObjects, toLinearObjects, estimatedMatrix, fromPositions, toPositions );

    currError = 0;
    for ( int i = 0; i < fromPositions->Size(); i++ )
    {
      vtkSmartPointer< vtkMRMLLORPositionNode > transformedFromPosition = fromPositions->GetPosition( i )->DeepCopy();
      transformedFromPosition->Transform( estimatedMatrix );
      vtkMRMLLORPositionNode* toPosition = toPositions->GetPosition( i );
      currError += vtkMRMLLORVectorMath::Norm( vtkMRMLLORVectorMath::Subtract( transformedFromPosition->GetPositionVector(), toPosition->GetPositionVector() ) );
    }
    currError = sqrt( currError / fromPositions->Size() );

  }

  calculatedMatrix->DeepCopy( estimatedMatrix );

  return currError;
}



// Node update methods ----------------------------------------------------------

void vtkSlicerLinearObjectRegistrationLogic
::ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData )
{
  // In case the module node is updated
  vtkMRMLLinearObjectRegistrationNode* lorNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( caller );

  // The caller must be a vtkMRMLLinearObjectRegistrationNode
  if ( lorNode != NULL && event == vtkCommand::ModifiedEvent )
  {
    this->CalculateTransform( lorNode ); // Will create modified event to update widget
  }

  // The position buffer must be ready for conversion to linear object
  if ( lorNode != NULL && event == vtkMRMLLinearObjectRegistrationNode::PositionBufferReady )
  {
    vtkSmartPointer< vtkMRMLLORLinearObjectNode > currentLinearObject = NULL;
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > positionBufferCopy = lorNode->GetActivePositionBuffer()->DeepCopy();
    lorNode->GetActivePositionBuffer()->Clear();

    if ( lorNode->GetCollectionState().compare( "Reference" ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::REFERENCE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( "Point" ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::POINT_DOF );
    }
    if ( lorNode->GetCollectionState().compare( "Line" ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::LINE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( "Plane" ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::PLANE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( "Collect" ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::UNKNOWN_DOF );
    }
    if ( lorNode->GetCollectionState().compare( "Automatic" ) == 0 )
    {
      positionBufferCopy->Trim( vtkMRMLLORConstants::TRIM_POSITIONS );
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, vtkMRMLLORConstants::UNKNOWN_DOF );
    }

    if ( currentLinearObject != NULL && this->GetActiveCollectionNode() != NULL )
    {
      currentLinearObject->SetPositionBuffer( positionBufferCopy );
      this->GetActiveCollectionNode()->AddLinearObject( currentLinearObject );
    }

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
    linearObjectRegistrationNode->AddObserver( vtkMRMLLinearObjectRegistrationNode::PositionBufferReady, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    linearObjectRegistrationNode->UpdateScene( this->GetMRMLScene() );
    linearObjectRegistrationNode->ObserveAllReferenceNodes(); // This will update
    this->CalculateTransform( linearObjectRegistrationNode ); // Will create modified event to update widget
  }

}