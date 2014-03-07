
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

  vtkMRMLLinearObjectCollectionNode* lorCollectionNode = vtkMRMLLinearObjectCollectionNode::New();
  this->GetMRMLScene()->RegisterNodeClass( lorCollectionNode );
  lorCollectionNode->Delete();

  vtkMRMLLinearObjectCollectionStorageNode* lorCollectionStorageNode = vtkMRMLLinearObjectCollectionStorageNode::New();
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
::SetActiveCollectionNode( vtkMRMLLinearObjectCollectionNode* newActiveCollectionNode )
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode != NULL )
  {
    selectionNode->SetAttribute( "ActiveLinearObjectCollectionID", newActiveCollectionNode->GetID() );
  }
}


vtkMRMLLinearObjectCollectionNode* vtkSlicerLinearObjectRegistrationLogic
::GetActiveCollectionNode()
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode == NULL )
  {
    return NULL;
  }

  const char* activeCollectionNodeID = selectionNode->GetAttribute( "ActiveLinearObjectCollectionID" );
  return vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( activeCollectionNodeID ) );
}


vtkMRMLMarkupsNode* vtkSlicerLinearObjectRegistrationLogic
::GetActiveMarkupsNode()
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode == NULL )
  {
    return NULL;
  }

  const char* activeMarkupsNodeID = selectionNode->GetActivePlaceNodeID();
  vtkMRMLMarkupsNode* activeMarkupsNode = vtkMRMLMarkupsNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( activeMarkupsNodeID ) );

  // Create a new markups node to be active if none is active
  if ( activeMarkupsNode != NULL )
  {
    return activeMarkupsNode;
  }

  activeMarkupsNode = vtkMRMLMarkupsFiducialNode::New();
  activeMarkupsNode->SetName( "F" );

  vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::New();
  this->GetMRMLScene()->AddNode( displayNode );
  displayNode->InvokeEvent( vtkMRMLMarkupsDisplayNode::ResetToDefaultsEvent ); // set to defaults

  activeMarkupsNode->AddAndObserveDisplayNodeID( displayNode->GetID() );
  this->GetMRMLScene()->AddNode( activeMarkupsNode );

  selectionNode->SetActivePlaceNodeID( activeMarkupsNode->GetID() );

  return activeMarkupsNode;
}


void vtkSlicerLinearObjectRegistrationLogic
::SetActiveMarkupsNode( vtkMRMLMarkupsNode* newActiveMarkupsNode )
{
  vtkMRMLApplicationLogic* appLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = appLogic->GetSelectionNode();

  if ( selectionNode == NULL )
  {
    return;
  }

  selectionNode->SetActivePlaceNodeID( newActiveMarkupsNode->GetID() );
}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkLORLinearObject > vtkSlicerLinearObjectRegistrationLogic
::PositionBufferToLinearObject( vtkLORPositionBuffer* positionBuffer, double noiseThreshold, int dof )
{
  if ( positionBuffer == NULL || positionBuffer->Size() == 0 )
  {
    return NULL;
  }

  std::vector<double> centroid = positionBuffer->CalculateCentroid();
  vnl_matrix<double>* cov = positionBuffer->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( vtkLORPosition::SIZE, vtkLORPosition::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( vtkLORPosition::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Grab only the most important eigenvectors
  std::vector<double> Eigenvector1( vtkLORPosition::SIZE, 0.0 ); // Smallest
  std::vector<double> Eigenvector2( vtkLORPosition::SIZE, 0.0 ); // Medium
  std::vector<double> Eigenvector3( vtkLORPosition::SIZE, 0.0 ); // Largest

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
  for ( int i = 0; i < vtkLORPosition::SIZE; i++ )
  {
    // Assuming RMS error E, then the eigenvalues should be E^2
    // Thus, compare the eigenvalues to the squared assumed rms noise level
    if ( abs( eigenvalues.get( i ) ) > noiseThreshold * noiseThreshold )
    {
      calculatedDOF++;
    }
  }

  // The threshold noise is twice the extraction threshold
  if ( dof == LORConstants::REFERENCE_DOF )
  {
    vtkLORReference* referenceObject = vtkLORReference::New( centroid );
    return vtkSmartPointer< vtkLORReference >::Take( referenceObject );
  }
  if ( dof == LORConstants::POINT_DOF || dof == LORConstants::UNKNOWN_DOF && calculatedDOF == LORConstants::POINT_DOF )
  {
    vtkLORPoint* pointObject = vtkLORPoint::New( centroid );
    return vtkSmartPointer< vtkLORPoint >::Take( pointObject );
  }
  if ( dof == LORConstants::LINE_DOF || dof == LORConstants::UNKNOWN_DOF && calculatedDOF == LORConstants::LINE_DOF )
  {
    vtkLORLine* lineObject = vtkLORLine::New( centroid, LORMath::Add( centroid, Eigenvector3 ) );
    return vtkSmartPointer< vtkLORLine >::Take( lineObject );
  }
  if ( dof == LORConstants::PLANE_DOF || dof == LORConstants::UNKNOWN_DOF && calculatedDOF >= LORConstants::PLANE_DOF )
  {
    vtkLORPlane* planeObject = vtkLORPlane::New( centroid, LORMath::Add( centroid, Eigenvector2 ), LORMath::Add( centroid, Eigenvector3 ) );
    return vtkSmartPointer< vtkLORPlane >::Take( planeObject );
  }

  return NULL; // TODO: Do something more productive if the dof is unknown (ie determine the dof automatically)...
}


// Check if the point should be automatically converted to reference
vtkSmartPointer< vtkLORLinearObject > vtkSlicerLinearObjectRegistrationLogic
::CorrespondPointToReference( vtkLORLinearObject* linearObject, vtkMRMLLinearObjectRegistrationNode* lorNode )
{
  vtkSmartPointer< vtkLORPoint > point = NULL;
  point.TakeReference( vtkLORPoint::SafeDownCast( linearObject ) );

  vtkMRMLLinearObjectCollectionNode* fromCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( lorNode->GetFromCollectionID() ) );
  vtkMRMLLinearObjectCollectionNode* toCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( lorNode->GetToCollectionID() ) );

  vtkMRMLLinearObjectCollectionNode* currentCollection = NULL;
  vtkMRMLLinearObjectCollectionNode* otherCollection = NULL;

  if ( this->GetActiveCollectionNode() == fromCollection )
  {
    currentCollection = fromCollection;
    otherCollection = toCollection;
  }
  if ( this->GetActiveCollectionNode() == toCollection )
  {
    currentCollection = toCollection;
    otherCollection = fromCollection;
  }

  // Don't do anything if there is no other collection or it is not a point
  if ( point == NULL || currentCollection == NULL || otherCollection == NULL || point->GetPositionBuffer() == NULL )
  {
    return linearObject;
  }

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > currentReferences = this->GetReferences( currentCollection );
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > otherReferences = this->GetReferences( otherCollection );
  if ( currentReferences->Size() >= otherReferences->Size() )
  {
    return linearObject;
  }

  vtkSmartPointer< vtkLORLinearObject > reference = this->PositionBufferToLinearObject( point->GetPositionBuffer(), lorNode->GetNoiseThreshold(), LORConstants::REFERENCE_DOF );
  reference->SetPositionBuffer( point->GetPositionBuffer()->DeepCopy() );
  return reference;
}


void vtkSlicerLinearObjectRegistrationLogic
::CreateModelPlane( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer, vtkMRMLLinearObjectRegistrationNode* lorNode )
{
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( node );
  if ( modelNode == NULL )
  {
    return;
  }

  vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast( this->GetActiveMarkupsNode() );
  if ( markupsNode == NULL || markupsNode->GetNumberOfMarkups() < 1 )
  {
    return;
  }
  vtkPolyData* modelPolyData = modelNode->GetPolyData();

  // Calculate the normal vector and the basePoint
  double fiducialPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  markupsNode->GetNthFiducialPosition( markupsNode->GetNumberOfMarkups() - 1, fiducialPosition );
  markupsNode->RemoveMarkup( markupsNode->GetNumberOfMarkups() - 1 ); // Remove the markup when done
  double closestPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  vtkSmartPointer< vtkGenericCell > closestCell = vtkSmartPointer< vtkGenericCell >::New();
  vtkIdType closestCellId = 0;
  int subId = 0;
  double squaredDistance = 0;

  vtkSmartPointer< vtkCellLocator > locator = vtkSmartPointer< vtkCellLocator >::New();
  locator->SetDataSet( modelPolyData );
  locator->BuildLocator();
  locator->FindClosestPoint( fiducialPosition, closestPosition, closestCell, closestCellId, subId, squaredDistance );

  // This is already normalized for us
  double normal[ 3 ] = { 0.0, 0.0, 0.0 };
  vtkPolygon::ComputeNormal( closestCell->GetPoints(), normal );

  // Caluclate the equation of the plane (point-normal form)
  double D = closestPosition[0] * normal[0] + closestPosition[1] * normal[1] + closestPosition[2] * normal[2];

  vtkPoints* modelPoints = modelPolyData->GetPoints();
  for ( int i = 0; i < modelPoints->GetNumberOfPoints(); i++ )
  {
    double currentPoint[ 3 ] = { 0.0, 0.0, 0.0 };
    modelPoints->GetPoint( i, currentPoint );
    double currentD = currentPoint[0] * normal[0] + currentPoint[1] * normal[1] + currentPoint[2] * normal[2];

    // We are on the plane
    if ( abs( currentD - D ) < lorNode->GetModelThreshold() )
    {
      std::vector< double > currentVector( 3, 0.0 );
      currentVector.at(0) = currentPoint[0];
      currentVector.at(1) = currentPoint[1];
      currentVector.at(2) = currentPoint[2];

      vtkSmartPointer< vtkLORPosition > currentPosition = vtkSmartPointer< vtkLORPosition >::New();
      currentPosition->SetPositionVector( currentVector );
      positionBuffer->AddPosition( currentPosition );
    }
  }

}


void vtkSlicerLinearObjectRegistrationLogic
::CreateModelLine( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer, vtkMRMLLinearObjectRegistrationNode* lorNode )
{
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( node );
  if ( modelNode == NULL )
  {
    return;
  }

  vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast( this->GetActiveMarkupsNode() );
  if ( markupsNode == NULL || markupsNode->GetNumberOfMarkups() < 1 )
  {
    return;
  }
  vtkPolyData* modelPolyData = modelNode->GetPolyData();

  vtkSmartPointer< vtkFeatureEdges > edgesFilter = vtkSmartPointer< vtkFeatureEdges >::New();
  edgesFilter->FeatureEdgesOn();
  edgesFilter->BoundaryEdgesOn(); // Shouldn't matter because the model should be closed
  edgesFilter->NonManifoldEdgesOff();
  edgesFilter->ManifoldEdgesOff();
  edgesFilter->SetInput( modelPolyData );
  edgesFilter->Update();
  

  // Calculate the closest position
  double fiducialPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  markupsNode->GetNthFiducialPosition( markupsNode->GetNumberOfMarkups() - 1, fiducialPosition );
  markupsNode->RemoveMarkup( markupsNode->GetNumberOfMarkups() - 1 ); // Remove the markup when done
  double closestPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  vtkSmartPointer< vtkGenericCell > closestCell = vtkSmartPointer< vtkGenericCell >::New();
  vtkIdType closestCellId = 0;
  int subId = 0;
  double squaredDistance = 0;

  vtkSmartPointer< vtkCellLocator > locator = vtkSmartPointer< vtkCellLocator >::New();
  locator->SetDataSet( edgesFilter->GetOutput() );
  locator->BuildLocator();
  locator->FindClosestPoint( fiducialPosition, closestPosition, closestCell, closestCellId, subId, squaredDistance );

  // There should be two points in the current vtkLine cell
  double closestEndPoint0[ 3 ] = { 0.0, 0.0, 0.0 };
  double closestEndPoint1[ 3 ] = { 0.0, 0.0, 0.0 };

  closestCell->GetPoints()->GetPoint( 0, closestEndPoint0 );
  closestCell->GetPoints()->GetPoint( 1, closestEndPoint1 );

  double direction[ 3 ] = { 0.0, 0.0, 0.0 };
  direction[ 0 ] = closestEndPoint0[ 0 ] - closestEndPoint1[ 0 ];
  direction[ 1 ] = closestEndPoint0[ 1 ] - closestEndPoint1[ 1 ];
  direction[ 2 ] = closestEndPoint0[ 2 ] - closestEndPoint1[ 2 ];

  double directionNorm = sqrt( direction[ 0 ] * direction[ 0 ] + direction[ 1 ] * direction[ 1 ] + direction[ 2 ] * direction[ 2 ] );
  direction[ 0 ] = direction[ 0 ] / directionNorm;
  direction[ 1 ] = direction[ 1 ] / directionNorm;
  direction[ 2 ] = direction[ 2 ] / directionNorm;


  vtkPoints* edgePoints = edgesFilter->GetOutput()->GetPoints();
  for ( int i = 0; i < edgePoints->GetNumberOfPoints(); i++ )
  {
    double currentPoint[ 3 ] = { 0.0, 0.0, 0.0 };
    edgePoints->GetPoint( i, currentPoint );

    double relativePoint[ 3 ] = { 0.0, 0.0, 0.0 };
    relativePoint[ 0 ] = currentPoint[ 0 ] - closestEndPoint0[ 0 ];
    relativePoint[ 1 ] = currentPoint[ 1 ] - closestEndPoint0[ 1 ];
    relativePoint[ 2 ] = currentPoint[ 2 ] - closestEndPoint0[ 2 ];

    double dot = direction[ 0 ] * relativePoint[ 0 ] + direction[ 1 ] * relativePoint[ 1 ] + direction[ 2 ] * relativePoint[ 2 ];

    double nonProjection[ 3 ] = { 0.0, 0.0, 0.0 };
    nonProjection[ 0 ] = relativePoint[ 0 ] - dot * direction[ 0 ];
    nonProjection[ 1 ] = relativePoint[ 1 ] - dot * direction[ 1 ];
    nonProjection[ 2 ] = relativePoint[ 2 ] - dot * direction[ 2 ];

    double dist = nonProjection[ 0 ] * nonProjection[ 0 ] + nonProjection[ 1 ] * nonProjection[ 1 ] + nonProjection[ 2 ] * nonProjection[ 2 ];
    

    // We are on the plane
    if ( abs( dist ) <  lorNode->GetModelThreshold() )
    {
      std::vector< double > currentVector( 3, 0.0 );
      currentVector.at(0) = currentPoint[0];
      currentVector.at(1) = currentPoint[1];
      currentVector.at(2) = currentPoint[2];

      vtkSmartPointer< vtkLORPosition > currentPosition = vtkSmartPointer< vtkLORPosition >::New();
      currentPosition->SetPositionVector( currentVector );
      positionBuffer->AddPosition( currentPosition );
    }
  }

}


void vtkSlicerLinearObjectRegistrationLogic
::CreateModelPoint( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer, vtkMRMLLinearObjectRegistrationNode* lorNode )
{
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( node );
  if ( modelNode == NULL )
  {
    return;
  }

  vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast( this->GetActiveMarkupsNode() );
  if ( markupsNode == NULL || markupsNode->GetNumberOfMarkups() < 1 )
  {
    return;
  }
  vtkPolyData* modelPolyData = modelNode->GetPolyData();

  // Calculate the closest position
  double fiducialPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  markupsNode->GetNthFiducialPosition( markupsNode->GetNumberOfMarkups() - 1, fiducialPosition );
  markupsNode->RemoveMarkup( markupsNode->GetNumberOfMarkups() - 1 ); // Remove the markup when done
  double closestPoint[ 3 ] = { 0.0, 0.0, 0.0 };
  vtkIdType closestPointId = 0;

  vtkSmartPointer< vtkPointLocator > locator = vtkSmartPointer< vtkPointLocator >::New();
  locator->SetDataSet( modelPolyData );
  locator->BuildLocator();
  closestPointId = locator->FindClosestPoint( fiducialPosition );
  modelPolyData->GetPoint( closestPointId, closestPoint );

  std::vector< double > currentVector( 3, 0.0 );
  currentVector.at(0) = closestPoint[0];
  currentVector.at(1) = closestPoint[1];
  currentVector.at(2) = closestPoint[2];

  vtkSmartPointer< vtkLORPosition > currentPosition = vtkSmartPointer< vtkLORPosition >::New();
  currentPosition->SetPositionVector( currentVector );
  positionBuffer->AddPosition( currentPosition );

}


void vtkSlicerLinearObjectRegistrationLogic
::CreateModelReference( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer, vtkMRMLLinearObjectRegistrationNode* lorNode )
{
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( node );
  if ( modelNode == NULL )
  {
    return;
  }

  vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast( this->GetActiveMarkupsNode() );
  if ( markupsNode == NULL || markupsNode->GetNumberOfMarkups() < 1 )
  {
    return;
  }
  vtkPolyData* modelPolyData = modelNode->GetPolyData();

  // Calculate the normal vector and the basePoint
  double fiducialPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  markupsNode->GetNthFiducialPosition( markupsNode->GetNumberOfMarkups() - 1, fiducialPosition );
  markupsNode->RemoveMarkup( markupsNode->GetNumberOfMarkups() - 1 ); // Remove the markup when done
  double closestPosition[ 3 ] = { 0.0, 0.0, 0.0 };
  vtkSmartPointer< vtkGenericCell > closestCell = vtkSmartPointer< vtkGenericCell >::New();
  vtkIdType closestCellId = 0;
  int subId = 0;
  double squaredDistance = 0;

  vtkSmartPointer< vtkCellLocator > locator = vtkSmartPointer< vtkCellLocator >::New();
  locator->SetDataSet( modelPolyData );
  locator->BuildLocator();
  locator->FindClosestPoint( fiducialPosition, closestPosition, closestCell, closestCellId, subId, squaredDistance );

  std::vector< double > currentVector( 3, 0.0 );
  currentVector.at(0) = closestPosition[0];
  currentVector.at(1) = closestPosition[1];
  currentVector.at(2) = closestPosition[2];

  vtkSmartPointer< vtkLORPosition > currentPosition = vtkSmartPointer< vtkLORPosition >::New();
  currentPosition->SetPositionVector( currentVector );
  positionBuffer->AddPosition( currentPosition );

}


void vtkSlicerLinearObjectRegistrationLogic
::PairCollections( vtkMRMLLinearObjectCollectionNode* collection0, vtkMRMLLinearObjectCollectionNode* collection1 )
{
  // Find the smaller size
  int smallSize = 0;
  if ( collection0->Size() < collection1->Size() )
  {
    smallSize = collection0->Size();
  }
  else
  {
    smallSize = collection1->Size();
  }

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > pairedCollection0 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > pairedCollection1 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  for ( int i = 0; i < smallSize; i++ )
  {
    vtkLORLinearObject* currentObject0 = collection0->GetLinearObject( i );
    vtkLORLinearObject* currentObject1 = collection1->GetLinearObject( i );
    if ( currentObject0 != NULL && currentObject1 != NULL && currentObject0->GetType().compare( currentObject1->GetType() ) == 0 )
    {
      pairedCollection0->AddLinearObject( currentObject0 );
      pairedCollection1->AddLinearObject( currentObject1 );
    }
  }

  collection0->Clear();
  collection0->Concatenate( pairedCollection0 );

  collection1->Clear();
  collection1->Concatenate( pairedCollection1 );

}


void vtkSlicerLinearObjectRegistrationLogic
::MatchCollections( vtkMRMLLinearObjectCollectionNode* collection0, vtkMRMLLinearObjectCollectionNode* collection1, double matchingThreshold, bool removeUnmatched )
{
  // Get the reference from both to calculate all of their objects' signatures
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > referenceCollection0 = this->GetReferences( collection0 );
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > referenceCollection1 = this->GetReferences( collection1 );

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > nonReferenceCollection0 = this->GetNonReferences( collection0 );
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > nonReferenceCollection1 = this->GetNonReferences( collection1 );

  if ( referenceCollection0->Size() != referenceCollection1->Size() )
  {
    throw std::logic_error( "Could not find sufficient references." ); // The original collections are unchanged
  }
  // If there are no references then change nothing
  if ( referenceCollection0->Size() == 0 && referenceCollection1->Size() == 0 )
  {
    return;
  }

  nonReferenceCollection0->CalculateSignature( referenceCollection0 );
  nonReferenceCollection1->CalculateSignature( referenceCollection1 );

  // Now, stored the matched and unmatched collections
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > matchedCollection0 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > matchedCollection1 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > unmatchedCollection0 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > unmatchedCollection1 = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();


  // This changes the from collection, but creates a new to collection - we want to change the old to collection
  if ( referenceCollection0->Size() == 0 || nonReferenceCollection1->Size() == 0 || referenceCollection0->Size() != referenceCollection1->Size() )
  {
    throw std::logic_error( "Could not find sufficient references." ); // The original collections are unchanged
  }

  for( int i = 0; i < nonReferenceCollection0->Size(); i++ )
  {
    vtkLORLinearObject* currentObject0 = nonReferenceCollection0->GetLinearObject( i );
    double bestDistance = std::numeric_limits<double>::max();
    int bestIndex = -1;

    if ( currentObject0 == NULL )
    {
      continue;
    }

    for ( int j = 0; j < nonReferenceCollection1->Size(); j++ )
    {
      vtkLORLinearObject* currentObject1 = nonReferenceCollection1->GetLinearObject( j );

      if ( currentObject1 == NULL )
      {
        continue;
      }

      if ( currentObject0->GetSignature().size() != currentObject1->GetSignature().size() || currentObject0->GetType().compare( currentObject1->GetType() ) != 0 )
      {
        continue;
      }
      
      double currentDistance = LORMath::Distance( currentObject0->GetSignature(), currentObject1->GetSignature() );

      if ( currentDistance < bestDistance )
      {
        bestDistance = currentDistance;
        bestIndex = j;
      }
    }// for over collection 1

    if ( bestDistance < matchingThreshold )
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
  collection0->Clear();
  collection0->Concatenate( referenceCollection0 );
  collection0->Concatenate( matchedCollection0 );

  collection1->Clear();
  collection1->Concatenate( referenceCollection1 );
  collection1->Concatenate( matchedCollection1 );

  if ( ! removeUnmatched )
  {
    collection0->Concatenate( unmatchedCollection0 );
    collection1->Concatenate( unmatchedCollection1 );
  }

}


vtkSmartPointer< vtkLORLinearObject> vtkSlicerLinearObjectRegistrationLogic
::MergeLinearObjects( vtkMRMLLinearObjectCollectionNode* collection, std::vector<int> indices, double noiseThreshold )
{
  if ( collection == NULL || collection->Size() == 0 || indices.size() == 0  )
  {
    return NULL;
  }

  vtkSmartPointer< vtkLORPositionBuffer > mergedPositionBuffer = vtkSmartPointer< vtkLORPositionBuffer >::New();

  for ( int i = 0; i < indices.size(); i++ )
  {
    vtkLORLinearObject* currentLinearObject = collection->GetLinearObject( indices.at( i ) );
    if ( currentLinearObject == NULL || currentLinearObject->GetPositionBuffer() == NULL )
    {
      continue;
    }
    
    mergedPositionBuffer->Concatenate( currentLinearObject->GetPositionBuffer()->DeepCopy() );
  }

  vtkSmartPointer< vtkLORLinearObject > mergedLinearObject = this->PositionBufferToLinearObject( mergedPositionBuffer, noiseThreshold, LORConstants::UNKNOWN_DOF );
  mergedLinearObject->SetPositionBuffer( mergedPositionBuffer );

  return mergedLinearObject;
}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetReferences( vtkMRMLLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > referenceCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkLORLinearObject* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject != NULL && currentLinearObject->GetType().compare( LORConstants::REFERENCE_STRING ) == 0 )
    {
      referenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return referenceCollection;
}


// Return smart pointer since we created the object in this function
vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > vtkSlicerLinearObjectRegistrationLogic
::GetNonReferences( vtkMRMLLinearObjectCollectionNode* collection )
{
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > nonReferenceCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  for ( int i = 0; i < collection->Size(); i++ )
  {
    vtkLORLinearObject* currentLinearObject = collection->GetLinearObject( i );

    if ( currentLinearObject != NULL && currentLinearObject->GetType().compare( LORConstants::REFERENCE_STRING ) != 0 )
    {
      nonReferenceCollection->AddLinearObject( currentLinearObject );
    }
  }

  return nonReferenceCollection;
}


// Model display methods
// -------------------------------------------------------------------------------------------------------

void vtkSlicerLinearObjectRegistrationLogic
::CreateLinearObjectModelHierarchyNode( vtkLORLinearObject* linearObject, vtkMRMLLinearObjectCollectionNode* collection )
{
  if ( linearObject->GetModelHierarchyNodeID().compare( "" ) != 0 || collection->GetModelHierarchyNodeID().compare( "" ) == 0 )
  {
    return;
  }

  // Otherwise, create a new model
  vtkPolyData* linearObjectPolyData = linearObject->CreateModelPolyData();

  vtkSmartPointer< vtkMRMLModelNode > linearObjectModel;
  linearObjectModel.TakeReference( vtkMRMLModelNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelNode" ) ) );
  linearObjectModel->SetAndObservePolyData( linearObjectPolyData );
  std::string modelName = linearObject->GetName() + "Model";
  linearObjectModel->SetName( modelName.c_str() );
  linearObjectModel->SetScene( this->GetMRMLScene() );

  vtkSmartPointer< vtkMRMLModelDisplayNode > linearObjectModelDisplay;
  linearObjectModelDisplay.TakeReference( vtkMRMLModelDisplayNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelDisplayNode" ) ) );
  linearObjectModelDisplay->SetScene( this->GetMRMLScene() );
  linearObjectModelDisplay->SetInputPolyData( linearObjectModel->GetPolyData() );
  linearObjectModelDisplay->SetVisibility( false );
  linearObjectModelDisplay->BackfaceCullingOff();

  vtkSmartPointer< vtkMRMLModelHierarchyNode > linearObjectModelHierarchyNode;
  linearObjectModelHierarchyNode.TakeReference( vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelHierarchyNode" ) ) );
  std::string modelHierarchyNodeName = linearObject->GetName() + "ModelHierarchyNode";
  linearObjectModelHierarchyNode->SetName( modelHierarchyNodeName.c_str() );
  linearObjectModelHierarchyNode->SetScene( this->GetMRMLScene() );

  this->GetMRMLScene()->AddNode( linearObjectModelDisplay );
  this->GetMRMLScene()->AddNode( linearObjectModel );
  this->GetMRMLScene()->AddNode( linearObjectModelHierarchyNode );

  linearObjectModel->SetAndObserveDisplayNodeID( linearObjectModelDisplay->GetID() );
  //linearObjectModelHierarchyNode->SetAndObserveDisplayNodeID( linearObjectModelDisplay->GetID() );
  linearObjectModelHierarchyNode->SetModelNodeID( linearObjectModel->GetID() );
  linearObjectModelHierarchyNode->SetParentNodeID( collection->GetModelHierarchyNodeID().c_str() );

  // And let the linear object store the model's ID
  linearObject->SetModelHierarchyNodeID( linearObjectModelHierarchyNode->GetID() );
  linearObjectModelHierarchyNode->SetHideFromEditors( true );
}


void vtkSlicerLinearObjectRegistrationLogic
::CreateLinearObjectCollectionModelHierarchyNode( vtkMRMLLinearObjectCollectionNode* collection )
{
  if ( collection->GetModelHierarchyNodeID().compare( "" ) != 0 )
  {
    return;
  }

  // Otherwise, create a new hierarchy node

  vtkSmartPointer< vtkMRMLModelDisplayNode > collectionModelDisplay;
  collectionModelDisplay.TakeReference( vtkMRMLModelDisplayNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelDisplayNode" ) ) );
  collectionModelDisplay->SetScene( this->GetMRMLScene() );
  collectionModelDisplay->SetVisibility( false );
  collectionModelDisplay->BackfaceCullingOff();

  vtkSmartPointer< vtkMRMLModelHierarchyNode > collectionModelHierarchyNode;
  collectionModelHierarchyNode.TakeReference( vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->CreateNodeByClass( "vtkMRMLModelHierarchyNode" ) ) );
  std::string modelHierarchyNodeName = std::string( collection->GetName() ) + "CollectionModelHierarchyNode";
  collectionModelHierarchyNode->SetName( modelHierarchyNodeName.c_str() );
  collectionModelHierarchyNode->SetScene( this->GetMRMLScene() );
  collectionModelHierarchyNode->ExpandedOff();

  this->GetMRMLScene()->AddNode( collectionModelDisplay );
  this->GetMRMLScene()->AddNode( collectionModelHierarchyNode );

  collectionModelHierarchyNode->SetAndObserveDisplayNodeID( collectionModelDisplay->GetID() );

  // And let the linear object store the model's ID
  collection->SetModelHierarchyNodeID( collectionModelHierarchyNode->GetID() );
}


// Note that this doesn't update the collection's model node, just its objects' model nodes
void vtkSlicerLinearObjectRegistrationLogic
::CreateLinearObjectModelHierarchyNodes( vtkMRMLLinearObjectCollectionNode* collection )
{
  // Also, update all of the linear objects' models if necessary
  for ( int i = 0; i < collection->Size(); i++ )
  {
    if ( collection->GetLinearObject( i ) != NULL && collection->GetLinearObject( i )->GetModelHierarchyNodeID().compare( "" ) == 0 )
    {
      this->CreateLinearObjectModelHierarchyNode( collection->GetLinearObject( i ), collection );
    }
  }
}


void vtkSlicerLinearObjectRegistrationLogic
::ShowLinearObjectModel( vtkLORLinearObject* linearObject )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObject->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL || hierarchyNode->GetModelNode() == NULL )
  {
    return;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetModelNode()->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return;
  }
  displayNode->VisibilityOn();
}


void vtkSlicerLinearObjectRegistrationLogic
::HideLinearObjectModel( vtkLORLinearObject* linearObject )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObject->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL || hierarchyNode->GetModelNode() == NULL )
  {
    return;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetModelNode()->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return;
  }
  displayNode->VisibilityOff();
}


bool vtkSlicerLinearObjectRegistrationLogic
::GetLinearObjectModelVisibility( vtkLORLinearObject* linearObject )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObject->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL || hierarchyNode->GetModelNode() == NULL )
  {
    return false;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetModelNode()->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return false;
  }
  return displayNode->GetVisibility();
}


void vtkSlicerLinearObjectRegistrationLogic
::ShowLinearObjectCollectionModel( vtkMRMLLinearObjectCollectionNode* collection )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( collection->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL )
  {
    return;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return;
  }
  displayNode->VisibilityOn();
}


void vtkSlicerLinearObjectRegistrationLogic
::HideLinearObjectCollectionModel( vtkMRMLLinearObjectCollectionNode* collection )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( collection->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL )
  {
    return;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return;
  }
  displayNode->VisibilityOff();
}


bool vtkSlicerLinearObjectRegistrationLogic
::GetLinearObjectCollectionModelVisibility( vtkMRMLLinearObjectCollectionNode* collection )
{
  vtkMRMLModelHierarchyNode* hierarchyNode = vtkMRMLModelHierarchyNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( collection->GetModelHierarchyNodeID() ) );
  if ( hierarchyNode == NULL )
  {
    return false;
  }
  vtkMRMLDisplayNode* displayNode = hierarchyNode->GetDisplayNode();
  if ( displayNode == NULL )
  {
    return false;
  }
  return displayNode->GetVisibility();
}


// TODO: May be this function signature is too long...
void vtkSlicerLinearObjectRegistrationLogic
::GetFromAndToCollections( vtkMRMLLinearObjectCollectionNode* fromCollection, vtkMRMLLinearObjectCollectionNode* fromReferenceCollection, vtkMRMLLinearObjectCollectionNode* fromPointCollection, vtkMRMLLinearObjectCollectionNode* fromLineCollection, vtkMRMLLinearObjectCollectionNode* fromPlaneCollection,
                          vtkMRMLLinearObjectCollectionNode* toCollection, vtkMRMLLinearObjectCollectionNode* toReferenceCollection, vtkMRMLLinearObjectCollectionNode* toPointCollection, vtkMRMLLinearObjectCollectionNode* toLineCollection, vtkMRMLLinearObjectCollectionNode* toPlaneCollection )
{
  // Just add to the linear object collections based on the type attribute
  // It doesn't matter which collection we take from - if there's no match then there's no point in going through
  for ( int i = 0; i < fromCollection->Size(); i++ )
  {
    vtkLORLinearObject* currentFromLinearObject = fromCollection->GetLinearObject( i );
    vtkLORLinearObject* currentToLinearObject = toCollection->GetLinearObject( i );

    if ( currentFromLinearObject == NULL || currentToLinearObject == NULL )
    {
      continue;
    }

    if ( currentFromLinearObject->GetType().compare( LORConstants::REFERENCE_STRING ) == 0 && currentToLinearObject->GetType().compare( LORConstants::REFERENCE_STRING ) == 0 )
    {
      fromReferenceCollection->AddLinearObject( currentFromLinearObject );
      toReferenceCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( LORConstants::POINT_STRING ) == 0 && currentToLinearObject->GetType().compare( LORConstants::POINT_STRING ) == 0 )
    {
      fromPointCollection->AddLinearObject( currentFromLinearObject );
      toPointCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( LORConstants::LINE_STRING ) == 0 && currentToLinearObject->GetType().compare( LORConstants::LINE_STRING ) == 0 )
    {
      fromLineCollection->AddLinearObject( currentFromLinearObject );
      toLineCollection->AddLinearObject( currentToLinearObject );
    }
    if ( currentFromLinearObject->GetType().compare( LORConstants::PLANE_STRING ) == 0 && currentToLinearObject->GetType().compare( LORConstants::PLANE_STRING ) == 0 )
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
  vtkMRMLLinearObjectCollectionNode* fromCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetFromCollectionID() ) );
  vtkMRMLLinearObjectCollectionNode* toCollection = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetToCollectionID() ) );
  vtkMRMLLinearTransformNode* outputTransform = vtkMRMLLinearTransformNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( linearObjectRegistrationNode->GetOutputTransformID() ) );
  // This is not
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromPairedCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toPairedCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  if ( fromCollection == NULL || toCollection == NULL )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "One or more linear object collections not defined." );
    return;
  }

  if ( linearObjectRegistrationNode->GetAutomaticMatch() )
  {
    // Match the collections
    try
    {
      this->MatchCollections( fromCollection, toCollection, linearObjectRegistrationNode->GetMatchingThreshold(), false );
    }
    catch( std::logic_error e )
    {
      linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
      this->SetOutputMessage( linearObjectRegistrationNode->GetID(), e.what() );
      return;
    }
  }
    
  if ( outputTransform == NULL )
  {
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    this->SetOutputMessage( linearObjectRegistrationNode->GetID(), "Output transform is not defined." );
    return;
  }

  // Pair collections, removing any incomplete pairs
  fromPairedCollection->Copy( fromCollection );
  toPairedCollection->Copy( toCollection );
  this->PairCollections( fromPairedCollection, toPairedCollection );
  
  // Grab the linear object collections
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromReferenceCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromPointCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromLineCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromPlaneCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toReferenceCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toPointCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toLineCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toPlaneCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  
  this->GetFromAndToCollections( fromPairedCollection, fromReferenceCollection, fromPointCollection, fromLineCollection, fromPlaneCollection, 
    toPairedCollection, toReferenceCollection, toPointCollection, toLineCollection, toPlaneCollection );
  // Note: The number of each type of collection should be the same


  // The matching should already be done (in real-time)

  // Calculate the centroids
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromCentroidCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  fromCentroidCollection->Concatenate( fromPointCollection );
  fromCentroidCollection->Concatenate( fromLineCollection );
  fromCentroidCollection->Concatenate( fromPlaneCollection );

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toCentroidCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
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
  negativeFromCentroid = LORMath::Subtract( negativeFromCentroid, fromCentroid );

  std::vector<double> negativeToCentroid( toCentroid.size(), 0.0 );
  negativeToCentroid = LORMath::Subtract( negativeToCentroid, toCentroid );


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

  vtkSmartPointer< vtkLORPositionBuffer > FromPositions = vtkSmartPointer< vtkLORPositionBuffer >::New();
  for ( int i = 0; i < fromPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = fromPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    FromPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }

  vtkSmartPointer< vtkLORPositionBuffer > ToPositions = vtkSmartPointer< vtkLORPositionBuffer >::New();
  for ( int i = 0; i < toPointCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPointCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toLineCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    std::vector<double> basePoint = toPlaneCollection->GetLinearObject(i)->ProjectVector( BlankVector );
    ToPositions->AddPosition( vtkLORPosition::New( basePoint ) );
  }


  // Then, add the direction vector to the final point observation vectors
  /*
  for ( int i = 0; i < fromLineCollection->Size(); i++ )
  {
    vtkLORLine* CurrentGeometryObject = vtkLORLine::SafeDownCast( fromLineCollection->GetLinearObject(i) );
    std::vector<double> scaledDirection = LORMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() );
    FromPositions->AddPosition( vtkLORPosition::New( scaledDirection ) );
  }
  for ( int i = 0; i < fromPlaneCollection->Size(); i++ )
  {
    vtkLORPlane* CurrentGeometryObject = vtkLORPlane::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    std::vector<double> scaledNormal = LORMath::Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() );
    FromPositions->AddPosition( vtkLORPosition::New( scaledNormal ) );
  }
  */

  // TODO: Fix memory leaks
  for ( int i = 0; i < toLineCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    

    vtkLORLine* CurrentToObject = vtkLORLine::SafeDownCast( toLineCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = LORMath::Add( CurrentToObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkLORPoint::New( testVector ) ) ;

    // Subtract direction vector to projection of origin
    testVector = LORMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentToObject->GetDirection() ) );
    TempToCollection->AddLinearObject( vtkLORPoint::New( testVector ) );
    
    TempToCollection->Concatenate( toReferenceCollection );


    vtkLORLine* CurrentFromObject = vtkLORLine::SafeDownCast( fromLineCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = LORMath::Add( CurrentFromObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentFromObject->GetDirection() ) );
    TempFromCollection->AddLinearObject( vtkLORPoint::New( testVector ) );
    
    TempFromCollection->Concatenate( fromReferenceCollection );

    // Do the matching
    this->MatchCollections( TempToCollection, TempFromCollection, linearObjectRegistrationNode->GetMatchingThreshold(), true );

    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempToNonReferenceCollection = this->GetNonReferences( TempToCollection );
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempFromNonReferenceCollection = this->GetNonReferences( TempFromCollection );
    
    if( TempToNonReferenceCollection->Size() > 0 && TempFromNonReferenceCollection->Size() > 0 && fromReferenceCollection->Size() > 0 && toReferenceCollection->Size() > 0 )
    {
      // subtract off the projection of the origin to get just the direction vector back
      std::vector<double> scaledToDirection = LORMath::Subtract( TempToNonReferenceCollection->GetLinearObject(0)->GetBasePoint(), CurrentToObject->ProjectVector( BlankVector ) );
      ToPositions->AddPosition( vtkLORPosition::New( scaledToDirection ) );
      std::vector<double> scaledFromDirection = LORMath::Subtract( TempFromNonReferenceCollection->GetLinearObject(0)->GetBasePoint(), CurrentFromObject->ProjectVector( BlankVector ) );
      FromPositions->AddPosition( vtkLORPosition::New( scaledFromDirection ) );
    }
  }
  for ( int i = 0; i < toPlaneCollection->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempToCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempFromCollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
    std::vector<double> testVector;
    
    vtkLORPlane* CurrentToObject = vtkLORPlane::SafeDownCast( toPlaneCollection->GetLinearObject(i) );

    // Add direction vector to projection of origin
    testVector = LORMath::Add( CurrentToObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkLORPoint::New( testVector ) );
    
    // Subtract direction vector to projection of origin
    testVector = LORMath::Subtract( CurrentToObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentToObject->GetNormal() ) );
    TempToCollection->AddLinearObject( vtkLORPoint::New( testVector ) );

    TempToCollection->Concatenate( toReferenceCollection );


    vtkLORPlane* CurrentFromObject = vtkLORPlane::SafeDownCast( fromPlaneCollection->GetLinearObject(i) );
    
    // Add direction vector to projection of origin
    testVector = LORMath::Add( CurrentFromObject->ProjectVector( BlankVector ), LORMath::Multiply( LORConstants::DIRECTION_SCALE, CurrentFromObject->GetNormal() ) );
    TempFromCollection->AddLinearObject( vtkLORPoint::New( testVector ) );
    
    TempFromCollection->Concatenate( fromReferenceCollection );

    // Do the matching
    this->MatchCollections( TempToCollection, TempFromCollection, linearObjectRegistrationNode->GetMatchingThreshold(), true );

    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempToNonReferenceCollection = this->GetNonReferences( TempToCollection );
    vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > TempFromNonReferenceCollection = this->GetNonReferences( TempFromCollection );
    
    if( TempToNonReferenceCollection->Size() > 0 && TempFromNonReferenceCollection->Size() > 0 && fromReferenceCollection->Size() > 0 && toReferenceCollection->Size() > 0 )
    {
      // subtract off the projection of the origin to get just the direction vector back
      std::vector<double> scaledToNormal = LORMath::Subtract( TempToNonReferenceCollection->GetLinearObject( 0 )->GetBasePoint(), CurrentToObject->ProjectVector( BlankVector ) );
      ToPositions->AddPosition( vtkLORPosition::New( scaledToNormal ) );
      std::vector<double> scaledFromNormal = LORMath::Subtract( TempFromNonReferenceCollection->GetLinearObject( 0 )->GetBasePoint(), CurrentFromObject->ProjectVector( BlankVector ) );
      FromPositions->AddPosition( vtkLORPosition::New( scaledFromNormal ) );
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
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > fromICPTACollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  fromICPTACollection->Concatenate( fromPlaneCollection );
  fromICPTACollection->Concatenate( fromLineCollection );
  fromICPTACollection->Concatenate( fromPointCollection );

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > toICPTACollection = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  toICPTACollection->Concatenate( toPlaneCollection );
  toICPTACollection->Concatenate( toLineCollection );
  toICPTACollection->Concatenate( toPointCollection );

  double rmsError = this->LinearObjectICPTA( fromICPTACollection, toICPTACollection, FromToToRotation, FromToToTransform );
  this->MatrixRotationPart( FromToToTransform, FromToToRotation );
  FromToToTranslation = this->MatrixTranslationPart( FromToToTransform );
  FromToToTranslation = this->TranslationalRegistration( fromCentroid, LORMath::Add( toCentroid, FromToToTranslation ), FromToToRotation ); 
  this->RotationTranslationToMatrix( FromToToRotation, FromToToTranslation, FromToToTransform );

  this->UpdateOutputTransform( outputTransform, FromToToTransform );

  linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
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
::SphericalRegistration( vtkLORPositionBuffer* fromPoints, vtkLORPositionBuffer* toPoints, vtkMatrix4x4* currentFromToToTransform )
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
  vnl_matrix<double>* DataMatrix = new vnl_matrix<double>( vtkLORPosition::SIZE + 1, vtkLORPosition::SIZE + 1, 0.0 );

  if ( fromPoints->Size() != toPoints->Size() )
  {
    throw std::logic_error( "Inconsistent number of points!" ); // This should never happen
  }

  // Pick two dimensions, and find their data matrix entry
  for ( int d1 = 0; d1 < vtkLORPosition::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkLORPosition::SIZE; d2++ )
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
  vtkSmartPointer< vtkLORPosition > tempPosition = vtkSmartPointer< vtkLORPosition >::New();
  tempPosition->SetPositionVector( fromCentroid );
  tempPosition->Transform( rotation );

  std::vector<double> translation = LORMath::Subtract( toCentroid, tempPosition->GetPositionVector() );
  return translation;
}


std::vector<double> vtkSlicerLinearObjectRegistrationLogic
::TranslationalAdjustment( vtkLORPositionBuffer* fromPositions, vtkLORPositionBuffer* toPositions, vtkMatrix4x4* currentFromToToTransform )
{
  // Note: The linear objects should already be matched
  if ( fromPositions->Size() != toPositions->Size() )
  {
    return std::vector<double>( 0, 0.0 );
  }

  std::vector<double> sumTranslations( vtkLORPosition::SIZE, 0.0 );
  std::vector<double> sumMagnitudes( vtkLORPosition::SIZE, 0.0 );

  for ( int i = 0; i < fromPositions->Size(); i++ )
  {
    vtkSmartPointer< vtkLORPosition > transformedPosition = fromPositions->GetPosition( i )->DeepCopy();
    transformedPosition->Transform( currentFromToToTransform );

    std::vector<double> fromVector = transformedPosition->GetPositionVector();
    std::vector<double> toVector = toPositions->GetPosition( i )->GetPositionVector();

    std::vector<double> difference = LORMath::Subtract( fromVector, toVector );
    
    sumTranslations = LORMath::Add( sumTranslations, difference );
    sumMagnitudes = LORMath::Add( sumMagnitudes, LORMath::Abs( LORMath::Normalize( difference ) ) );
  }

  std::vector<double> newTranslation( vtkLORPosition::SIZE, 0.0 );
  for ( int i = 0; i < vtkLORPosition::SIZE; i++ )
  {
    newTranslation.at( i ) = sumTranslations.at( i ) / sumMagnitudes.at( i );
  }

  // Now put the translation back into the from coordinate system
  vtkSmartPointer< vtkMatrix4x4 > currentFromToToRotationInverse = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->MatrixRotationPart( currentFromToToTransform, currentFromToToRotationInverse );
  currentFromToToRotationInverse->Invert();

  vtkSmartPointer< vtkLORPosition > tempPosition = vtkSmartPointer< vtkLORPosition >::New();
  tempPosition->SetPositionVector( newTranslation );
  tempPosition->Transform( currentFromToToRotationInverse );
  newTranslation = tempPosition->GetPositionVector();

  return newTranslation;
}


void vtkSlicerLinearObjectRegistrationLogic
::FindClosestPositions( vtkMRMLLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* currentFromToToTransform,
                    vtkLORPositionBuffer* fromPositions, vtkLORPositionBuffer* toPositions )
{
  // Note: The linear objects should already be matched
  if ( fromLinearObjects->Size() != toLinearObjects->Size() )
  {
    return;
  }
  if ( ! fromLinearObjects->AllHavePositionBuffers() && ! toLinearObjects->AllHavePositionBuffers() )
  {
    return;
  }

  vtkSmartPointer< vtkMatrix4x4 > currentToToFromTransform = vtkSmartPointer< vtkMatrix4x4 >::New();
  currentToToFromTransform->DeepCopy( currentFromToToTransform );
  currentToToFromTransform->Invert();

  if ( fromLinearObjects->AllHavePositionBuffers() )
  {

    // Project the from linear objects onto the to linear objects
    for ( int i = 0; i < toLinearObjects->Size(); i++ )
    {

      vtkLORPositionBuffer* currentPositionBuffer = fromLinearObjects->GetLinearObject( i )->GetPositionBuffer();
      vtkLORLinearObject* currentLinearObject = toLinearObjects->GetLinearObject( i );

      for( int j = 0; j < currentPositionBuffer->Size(); j++ )
      {
        // Transform the point
        vtkSmartPointer< vtkLORPosition > currentPosition = currentPositionBuffer->GetPosition( j )->DeepCopy();
        vtkSmartPointer< vtkLORPosition > transformedPosition = currentPositionBuffer->GetPosition( j )->DeepCopy();
        transformedPosition->Transform( currentFromToToTransform );

        std::vector<double> projectedVector = currentLinearObject->ProjectVector( transformedPosition->GetPositionVector() );

        vtkSmartPointer< vtkLORPosition > closestPosition = vtkSmartPointer< vtkLORPosition >::New();
        closestPosition->SetPositionVector( projectedVector );

        // Add the points to the position buffers for registration
        fromPositions->AddPosition( currentPosition );
        toPositions->AddPosition( closestPosition );
      }
    }
  }


  if ( toLinearObjects->AllHavePositionBuffers() )
  {
    // Project the to linear objects onto the from linear objects
    for ( int i = 0; i < fromLinearObjects->Size(); i++ )
    {

      vtkLORPositionBuffer* currentPositionBuffer = toLinearObjects->GetLinearObject( i )->GetPositionBuffer();
      vtkLORLinearObject* currentLinearObject = fromLinearObjects->GetLinearObject( i );

      for( int j = 0; j < currentPositionBuffer->Size(); j++ )
      {
        // Transform the point
        vtkSmartPointer< vtkLORPosition > currentPosition = currentPositionBuffer->GetPosition( j )->DeepCopy();
        vtkSmartPointer< vtkLORPosition > transformedPosition = currentPositionBuffer->GetPosition( j )->DeepCopy();
        transformedPosition->Transform( currentToToFromTransform );

        std::vector<double> projectedVector = currentLinearObject->ProjectVector( transformedPosition->GetPositionVector() );

        vtkSmartPointer< vtkLORPosition > closestPosition = vtkSmartPointer< vtkLORPosition >::New();
        closestPosition->SetPositionVector( projectedVector );

        // Add the points to the position buffers for registration
        toPositions->AddPosition( currentPosition );
        fromPositions->AddPosition( closestPosition );
      }
    }
  }

}


vtkSmartPointer< vtkMatrix4x4 > vtkSlicerLinearObjectRegistrationLogic
::CombineRotationAndTranslation( vtkMatrix4x4* rotation, std::vector<double> translation )
{
  vtkSmartPointer< vtkLORPosition > tempPosition = vtkSmartPointer< vtkLORPosition >::New();
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
  std::vector<double> translation( vtkLORPosition::SIZE, 0.0 );

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
::LinearObjectICPTA( vtkMRMLLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* initialRotation, vtkMatrix4x4* calculatedMatrix )
{

  const double CONVERGENCE_THRESHOLD = 1e-3;
  double currError = std::numeric_limits<double>::max() / 2;
  double prevError = std::numeric_limits<double>::max();

  vtkSmartPointer< vtkMatrix4x4 > estimatedRotation = initialRotation;
  std::vector<double> estimatedTranslation( 3, 0.0 );
  vtkSmartPointer< vtkMatrix4x4 > estimatedMatrix;

  while ( abs( currError - prevError ) > CONVERGENCE_THRESHOLD )
  {
    vtkSmartPointer< vtkLORPositionBuffer > fromPositions = vtkSmartPointer< vtkLORPositionBuffer >::New();
    vtkSmartPointer< vtkLORPositionBuffer > toPositions = vtkSmartPointer< vtkLORPositionBuffer >::New();

	prevError = currError;

    // Translational adjustment
    estimatedMatrix = this->CombineRotationAndTranslation( estimatedRotation, estimatedTranslation );
    this->FindClosestPositions( fromLinearObjects, toLinearObjects, estimatedMatrix, fromPositions, toPositions );
    estimatedTranslation = LORMath::Add( estimatedTranslation, this->TranslationalAdjustment( fromPositions, toPositions, estimatedMatrix ) );

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
      vtkSmartPointer< vtkLORPosition > transformedFromPosition = fromPositions->GetPosition( i )->DeepCopy();
      transformedFromPosition->Transform( estimatedMatrix );
      vtkLORPosition* toPosition = toPositions->GetPosition( i );
      currError += LORMath::Norm( LORMath::Subtract( transformedFromPosition->GetPositionVector(), toPosition->GetPositionVector() ) );
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

    // Make sure all the models are updated (note that the collection model will have been created when the collection was added to the scene)
    vtkMRMLLinearObjectCollectionNode* fromCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( lorNode->GetFromCollectionID() ) );
    vtkMRMLLinearObjectCollectionNode* toCollectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID( lorNode->GetToCollectionID() ) );
    if ( fromCollectionNode != NULL )
    {
      this->CreateLinearObjectModelHierarchyNodes( fromCollectionNode );
    }
    if ( toCollectionNode != NULL )
    {
      this->CreateLinearObjectModelHierarchyNodes( toCollectionNode );
    }
  }

  // The position buffer must be ready for conversion to linear object
  if ( lorNode != NULL && event == vtkMRMLLinearObjectRegistrationNode::PositionBufferReady )
  {
    vtkSmartPointer< vtkLORLinearObject > currentLinearObject = NULL;
    vtkSmartPointer< vtkLORPositionBuffer > positionBufferCopy = lorNode->GetActivePositionBuffer()->DeepCopy(); // Note: This produces a non-real-time buffer (which is what we want)
    lorNode->GetActivePositionBuffer()->Clear();

    if ( lorNode->GetCollectionState().compare( LORConstants::REFERENCE_STRING ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::REFERENCE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( LORConstants::POINT_STRING ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::POINT_DOF );
    }
    if ( lorNode->GetCollectionState().compare( LORConstants::LINE_STRING ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::LINE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( LORConstants::PLANE_STRING ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::PLANE_DOF );
    }
    if ( lorNode->GetCollectionState().compare( LORConstants::COLLECT_STRING ) == 0 )
    {
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::UNKNOWN_DOF );
    }
    if ( lorNode->GetCollectionState().compare( LORConstants::AUTOMATIC_STRING ) == 0 )
    {
      positionBufferCopy->Trim( lorNode->GetTrimPositions() );
      currentLinearObject = this->PositionBufferToLinearObject( positionBufferCopy, lorNode->GetNoiseThreshold(), LORConstants::UNKNOWN_DOF );
    }

    if ( currentLinearObject != NULL && this->GetActiveCollectionNode() != NULL )
    {
      // Add a reference rather than a point if the other linear object
      currentLinearObject->SetPositionBuffer( positionBufferCopy );
      vtkSmartPointer< vtkLORLinearObject > convertedLinearObject = this->CorrespondPointToReference( currentLinearObject, lorNode );
      this->GetActiveCollectionNode()->AddLinearObject( convertedLinearObject );
    }

  }

}


void vtkSlicerLinearObjectRegistrationLogic
::ProcessMRMLSceneEvents( vtkObject* caller, unsigned long event, void* callData )
{
  vtkMRMLScene* callerNode = vtkMRMLScene::SafeDownCast( caller );

  // If the added node was a linear object registration node then observe it
  vtkMRMLNode* addedNode = reinterpret_cast< vtkMRMLNode* >( callData );
  vtkMRMLLinearObjectRegistrationNode* linearObjectRegistrationNode = vtkMRMLLinearObjectRegistrationNode::SafeDownCast( addedNode );

  if ( event == vtkMRMLScene::NodeAddedEvent && linearObjectRegistrationNode != NULL )
  {
    // This will get called exactly once
    linearObjectRegistrationNode->AddObserver( vtkCommand::ModifiedEvent, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    linearObjectRegistrationNode->AddObserver( vtkMRMLLinearObjectRegistrationNode::PositionBufferReady, ( vtkCommand* ) this->GetMRMLNodesCallbackCommand() );
    linearObjectRegistrationNode->UpdateScene( this->GetMRMLScene() );
    linearObjectRegistrationNode->ObserveAllReferenceNodes(); // This will update
    this->CalculateTransform( linearObjectRegistrationNode ); // Will create modified event to update widget
  }

  vtkMRMLLinearObjectCollectionNode* collectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( addedNode );

  if ( event == vtkMRMLScene::NodeAddedEvent && collectionNode != NULL )
  {
    if ( collectionNode->GetModelHierarchyNodeID().compare( "" ) == 0 )
    {
      this->CreateLinearObjectCollectionModelHierarchyNode( collectionNode );
      this->CreateLinearObjectModelHierarchyNodes( collectionNode );
    }
  }

}