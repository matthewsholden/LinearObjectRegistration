
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
  this->FromLinearObjects = NULL;
  this->ToLinearObjects = NULL;
  
  this->OutputTransform = NULL;
  this->ObservedTransformNode = NULL;
  this->ActivePositionBuffer = NULL;
  this->ActiveCollectionNode = NULL;

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

  vtkMRMLLORLinearObjectCollectionNode* lorCollectionNode = vtkMRMLLORLinearObjectCollectionNode::New();
  this->GetMRMLScene()->RegisterNodeClass( lorCollectionNode );
  lorCollectionNode->Delete();
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
::SetOutputTransform( vtkMRMLNode* node )
{
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  if ( transformNode == NULL )
  {
    return;
  }

  this->OutputTransform = transformNode;
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


void vtkSlicerLinearObjectRegistrationLogic
::SetActiveCollectionNode( vtkMRMLLORLinearObjectCollectionNode* newActiveCollectionNode )
{
 this->ActiveCollectionNode = newActiveCollectionNode;
}


vtkMRMLLORLinearObjectCollectionNode* vtkSlicerLinearObjectRegistrationLogic
::GetActiveCollectionNode()
{
  return this->ActiveCollectionNode;
}


void vtkSlicerLinearObjectRegistrationLogic
::ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData )
{
  vtkMRMLLinearTransformNode* callerNode = vtkMRMLLinearTransformNode::SafeDownCast( caller );

  if ( this->ActivePositionBuffer == NULL )
  {
    return;
  }

  if ( strcmp( callerNode->GetID(), this->ObservedTransformNode->GetID() ) == 0 )
  {
    vtkMatrix4x4* matrix = callerNode->GetMatrixTransformToParent();
    vtkMRMLLORPositionNode* currentPositionNode = vtkMRMLLORPositionNode::New( callerNode->GetMatrixTransformToParent() );
    this->ActivePositionBuffer->AddPosition( currentPositionNode );
  }
}


// This makes the module ready to record - see the ProcessMRMLNodesEvents for this class to see whether or not any recording actually happens
void vtkSlicerLinearObjectRegistrationLogic
::InitializeActivePositionBuffer( std::string collectType )
{
  this->ActivePositionBuffer = vtkMRMLLORPositionBufferNode::New();
  this->CollectType = collectType;
}


void vtkSlicerLinearObjectRegistrationLogic
::FinalizeActivePositionBuffer()
{
  if ( this->ActivePositionBuffer == NULL )
  {
    return;
  }

  vtkMRMLLORLinearObjectNode* currentLinearObject;

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
    this->ActivePositionBuffer->Delete();
    this->ActivePositionBuffer = NULL;
    return;
  }

  this->InsertNewLinearObject( currentLinearObject );
  currentLinearObject->SetPositionBuffer( this->ActivePositionBuffer->DeepCopy() ); // TODO: May be don't need to deep copy (just don't delete)
  this->ActivePositionBuffer->Delete();
  this->ActivePositionBuffer = NULL;
}


void vtkSlicerLinearObjectRegistrationLogic
::InsertNewLinearObject( vtkMRMLLORLinearObjectNode* linearObject )
{
  if ( this->ActiveCollectionNode == NULL )
  {
    return;
  }
   
  this->ActiveCollectionNode->AddLinearObject( vtkSmartPointer< vtkMRMLLORLinearObjectNode >::Take( linearObject ) );
  // TODO: Need to insert the current linear object into the correct slot in the collection
}


vtkMRMLLORLinearObjectNode* vtkSlicerLinearObjectRegistrationLogic
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
    return vtkMRMLLORReferenceNode::New( centroid );
  }
  if ( dof == POINT_DOF )
  {
    return vtkMRMLLORPointNode::New( centroid );
  }
  if ( dof == LINE_DOF )
  {
    return vtkMRMLLORLineNode::New( centroid, vtkMRMLLORVectorMath::Add( centroid, Eigenvector3 ) ); 
  }
  if ( dof == PLANE_DOF )
  {
    return vtkMRMLLORPlaneNode::New( centroid, vtkMRMLLORVectorMath::Add( centroid, Eigenvector2 ), vtkMRMLLORVectorMath::Add( centroid, Eigenvector3 ) );
  }

  return NULL; // TODO: Do something more productive if the dof is unknown...
}


void vtkSlicerLinearObjectRegistrationLogic
::MatchCollections( vtkMRMLLORLinearObjectCollectionNode* fromCollection, vtkMRMLLORLinearObjectCollectionNode* toCollection )
{
  // Get the reference from both to calculate all of their objects' signatures
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromReferenceCollection = this->GetReferences( fromCollection );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toReferenceCollection = this->GetReferences( toCollection );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromNonReferenceCollection = this->GetNonReferences( fromCollection );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toNonReferenceCollection = this->GetNonReferences( toCollection );

  fromNonReferenceCollection->CalculateSignature( fromReferenceCollection );
  toNonReferenceCollection->CalculateSignature( toReferenceCollection );

  // This changes the from collection, but creates a new to collection - we want to change the old to collection
  toNonReferenceCollection = fromNonReferenceCollection->GetMatches( toNonReferenceCollection, MATCHING_THRESHOLD );

  fromCollection->Copy( fromReferenceCollection );
  fromCollection->Concatenate( fromNonReferenceCollection );

  toCollection->Copy( toReferenceCollection );
  toCollection->Concatenate( toNonReferenceCollection );
}


vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetReferences( vtkMRMLLORLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > referenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject->Type.compare( "Reference" ) == 0 )
    {
      referenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return referenceCollection;
}


vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetNonReferences( vtkMRMLLORLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > nonReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject->Type.compare( "Reference" ) != 0 )
    {
      nonReferenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return nonReferenceCollection;
}


// TODO: May be this function signature is too long...
void vtkSlicerLinearObjectRegistrationLogic
::GetFromAndToCollections( vtkMRMLLORLinearObjectCollectionNode* fromReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* fromPointCollection, vtkMRMLLORLinearObjectCollectionNode* fromLineCollection, vtkMRMLLORLinearObjectCollectionNode* fromPlaneCollection,
                          vtkMRMLLORLinearObjectCollectionNode* toReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* toPointCollection, vtkMRMLLORLinearObjectCollectionNode* toLineCollection, vtkMRMLLORLinearObjectCollectionNode* toPlaneCollection )
{
  // Just add to the linear object collections based on the type attribute
  // It doesn't matter which collection we take from - if there's no match then there's no point in going through
  for ( int i = 0; i < this->FromLinearObjects->Size(); i++ )
  {
    vtkMRMLLORLinearObjectNode* currentFromLinearObject = this->FromLinearObjects->GetLinearObject( i );
    vtkMRMLLORLinearObjectNode* currentToLinearObject = this->ToLinearObjects->GetLinearObject( i );

    if ( currentFromLinearObject == NULL || currentToLinearObject == NULL )
    {
      continue;
    }

    if ( currentFromLinearObject->Type.compare( "Reference" ) == 0 && currentToLinearObject->Type.compare( "Reference" ) == 0 )
    {
      fromReferenceCollection->AddLinearObject( currentFromLinearObject );
      toReferenceCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->Type.compare( "Point" ) == 0 && currentToLinearObject->Type.compare( "Point" ) == 0 )
    {
      fromPointCollection->AddLinearObject( currentFromLinearObject );
      toPointCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->Type.compare( "Line" ) == 0 && currentToLinearObject->Type.compare( "Line" ) == 0 )
    {
      fromLineCollection->AddLinearObject( currentFromLinearObject );
      toLineCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->Type.compare( "Plane" ) == 0 && currentToLinearObject->Type.compare( "Plane" ) == 0 )
    {
      fromPlaneCollection->AddLinearObject( currentFromLinearObject );
      toPlaneCollection->AddLinearObject( currentToLinearObject );
    }

  }

}


void vtkSlicerLinearObjectRegistrationLogic
::Register( vtkMRMLLORLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLORLinearObjectCollectionNode* toLinearObjects, vtkMRMLLinearTransformNode* outputTransform )
{

  this->FromLinearObjects = fromLinearObjects;
  this->ToLinearObjects = toLinearObjects;
  
  // Grab the linear object collections
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromPointCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromLineCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > fromPlaneCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toReferenceCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toPointCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toLineCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > toPlaneCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  
  this->GetFromAndToCollections( fromReferenceCollection, fromPointCollection, fromLineCollection, fromPlaneCollection, 
    toReferenceCollection, toPointCollection, toLineCollection, toPlaneCollection );
  // Note: The number of each type of collection should be the same

  if ( fromReferenceCollection->Size() == 0 || toReferenceCollection->Size() == 0 || fromReferenceCollection->Size() != toReferenceCollection->Size() )
  {
    this->OutputMessage = "Failed: Could not find appropriate references.";
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
    this->OutputMessage = e.what();
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

  toPointCollection->Translate( negativeToCentroid );
  toLineCollection->Translate( negativeToCentroid );
  toPlaneCollection->Translate( negativeToCentroid );


  // Next, add the base points to the final point observation vectors
  std::vector<double> BlankVector( negativeToCentroid.size(), 0.0 );

  vtkSmartPointer< vtkMRMLLORPositionBufferNode > FromPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
  for ( int i = 0; i < fromPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }

  vtkSmartPointer< vtkMRMLLORPositionBufferNode > ToPositions = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
  for ( int i = 0; i < toPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( basePoint ) ) );
  }


  // Then, add the direction vector to the final point observation vectors
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORLineNode > CurrentGeometryObject = vtkMRMLLORLineNode::SafeDownCast( fromLineCollection->GetLinearObject(i) );
    std::vector<double> scaledDirection = vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() );
    FromPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( scaledDirection ) ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORPlaneNode > CurrentGeometryObject = vtkMRMLLORPlaneNode::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    std::vector<double> scaledNormal = vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() );
    FromPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( scaledNormal ) ) );
  }

  // TODO: Fix memory leaks
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    

    vtkSmartPointer< vtkMRMLLORLineNode > CurrentToObject = vtkMRMLLORLineNode::SafeDownCast( toLineCollection->GetLinearObject(i) );

    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );

    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );
    
    TempToCollection->CalculateSignature( toReferenceCollection );


    vtkSmartPointer< vtkMRMLLORLineNode > CurrentFromObject = vtkMRMLLORLineNode::SafeDownCast( fromLineCollection->GetLinearObject(i) );

    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentFromObject->GetDirection() ) );
    TempFromCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );
    
    TempFromCollection->CalculateSignature( fromReferenceCollection );


	TempToCollection = TempFromCollection->GetMatches( TempToCollection, MATCHING_THRESHOLD );

    std::vector<double> scaledDirection = vtkMRMLLORVectorMath::Subtract( TempToCollection->GetLinearObject(0)->BasePoint, CurrentToObject->ProjectVector( BlankVector ) );
    ToPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( scaledDirection ) ) );
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    
    vtkSmartPointer< vtkMRMLLORPlaneNode > CurrentToObject = vtkMRMLLORPlaneNode::SafeDownCast( toPlaneCollection->GetLinearObject(i) );

    testVector = vtkMRMLLORVectorMath::Add( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );
    
    testVector = vtkMRMLLORVectorMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );

    TempToCollection->CalculateSignature( toReferenceCollection );


    vtkSmartPointer< vtkMRMLLORPlaneNode > CurrentFromObject = vtkMRMLLORPlaneNode::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    
    testVector = vtkMRMLLORVectorMath::Add( CurrentFromObject->ProjectVector( BlankVector ), vtkMRMLLORVectorMath::Multiply( DIRECTION_SCALE, CurrentFromObject->GetNormal() ) );
    TempFromCollection->AddLinearObject( vtkSmartPointer< vtkMRMLLORPointNode >::Take( vtkMRMLLORPointNode::New( testVector ) ) );
    
    TempFromCollection->CalculateSignature( fromReferenceCollection );

	TempToCollection = TempFromCollection->GetMatches( TempToCollection, MATCHING_THRESHOLD );

    std::vector<double> scaledNormal = vtkMRMLLORVectorMath::Subtract( TempToCollection->GetLinearObject(0)->BasePoint, CurrentToObject->ProjectVector( BlankVector ) );
    ToPositions->AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode >::Take( vtkMRMLLORPositionNode::New( scaledNormal ) ) );
  }


  // Finally, calculate the registration
  vnl_matrix<double>* FromToToRotation;

  try
  {
    FromToToRotation = this->SphericalRegistration( FromPositions, ToPositions );
  }
  catch( std::logic_error e )
  {
    this->OutputMessage = e.what();
	return;
  }

  vnl_matrix<double>* FromToToTranslation = this->TranslationalRegistration( fromCentroid, toCentroid, FromToToRotation ); 

  // And set the output matrix
  this->UpdateOutputTransform( FromToToRotation, FromToToTranslation );

  this->OutputMessage = "Success!";

}


void vtkSlicerLinearObjectRegistrationLogic
::UpdateOutputTransform( vnl_matrix<double>* newTransformMatrix )
{
  vtkMatrix4x4* outputMatrix = this->OutputTransform->GetMatrixTransformToParent();

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
::UpdateOutputTransform( vnl_matrix<double>* newRotationMatrix, vnl_matrix<double>* newTranslationVector )
{
  vtkMatrix4x4* outputMatrix = this->OutputTransform->GetMatrixTransformToParent();

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
::SphericalRegistration( vtkSmartPointer< vtkMRMLLORPositionBufferNode > fromPoints, vtkSmartPointer< vtkMRMLLORPositionBufferNode > toPoints )
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
	    DataMatrix->put( d1, d2, DataMatrix->get( d1, d2 ) + fromPoints->GetPosition(i)->Position.at(d1) * toPoints->GetPosition(i)->Position.at(d2) );
	  }
	}
  }

  // Now we can calculate its svd
  vnl_svd<double>* SVDMatrix = new vnl_svd<double>( *DataMatrix, 0.0 );
  if ( SVDMatrix->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error( "Failed - spherical registration is ill-conditioned!" );
  }

  return new vnl_matrix<double>( SVDMatrix->V() * SVDMatrix->U().transpose() );
}


vnl_matrix<double>* vtkSlicerLinearObjectRegistrationLogic
::TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vnl_matrix<double>* rotation )
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