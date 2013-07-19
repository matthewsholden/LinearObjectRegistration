
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
  this->GeometryBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryPointBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryLineBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryPlaneBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryReferenceBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();

  this->RecordPointBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordLineBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordPlaneBuffer =  vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordReferenceBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();

  this->RegistrationTransformNode = NULL;
  this->ErrorRMS = 0.0;
  this->Status = "";
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
  assert(this->GetMRMLScene() != 0);
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


void vtkSlicerLinearObjectRegistrationLogic
::ResetGeometry()
{
  this->GeometryBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryPointBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryLineBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryPlaneBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->GeometryReferenceBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
}



void vtkSlicerLinearObjectRegistrationLogic
::ImportGeometry( std::string fileName )
{
  this->ResetGeometry();

  // Read the record log from XML file
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.c_str() );
  parser->Parse();
  vtkSmartPointer< vtkXMLDataElement > rootElement = parser->GetRootElement();

  this->GeometryBuffer->FromXMLElement( rootElement );

  // Sort the linear objects into their appropriate classes
  for ( int i = 0; i < GeometryBuffer->Size(); i++ )
  {
    if( strcmp( GeometryBuffer->GetLinearObject(i)->Type.c_str(), "Point" ) == 0 )
	{
      this->GeometryPointBuffer->AddLinearObject( GeometryBuffer->GetLinearObject(i) );
	}
    if( strcmp( GeometryBuffer->GetLinearObject(i)->Type.c_str(), "Line" ) == 0 )
	{
      this->GeometryLineBuffer->AddLinearObject( GeometryBuffer->GetLinearObject(i) );
	}
	if( strcmp( GeometryBuffer->GetLinearObject(i)->Type.c_str(), "Plane" ) == 0 )
	{
      this->GeometryPlaneBuffer->AddLinearObject( GeometryBuffer->GetLinearObject(i) );
	}
    if( strcmp( GeometryBuffer->GetLinearObject(i)->Type.c_str(), "Reference" ) == 0 )
	{
      this->GeometryReferenceBuffer->AddLinearObject( GeometryBuffer->GetLinearObject(i) );
	}
  }

}



void vtkSlicerLinearObjectRegistrationLogic
::ResetRecord()
{
  this->RecordPointBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordLineBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordPlaneBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  this->RecordReferenceBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();

  // TODO: Destruct each object in the vector
  this->LinearObjectPoints.clear();
  this->ReferencePoints.clear();
  this->PointPoints.clear();
  this->LinePoints.clear();
  this->PlanePoints.clear();
}




void vtkSlicerLinearObjectRegistrationLogic
::ImportRecord( std::string fileName, int filterWidth, int collectionFrames, double extractionThreshold )
{
  this->ResetRecord();

  // Read the record log from XML file
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.c_str() );
  parser->Parse();
  vtkSmartPointer< vtkXMLDataElement > rootElement = parser->GetRootElement();

  int numElements = rootElement->GetNumberOfNestedElements();  // Number of saved records (including transforms and messages).

  vtkSmartPointer< vtkLORPointObservationBuffer > CollectedPoints = vtkSmartPointer< vtkLORPointObservationBuffer >::New();

  vtkSmartPointer< vtkXMLDataElement > currElement = NULL;
  vtkSmartPointer< vtkXMLDataElement > prevElement = rootElement->GetNestedElement( 0 ); // We don't really need the first element (or care)

  for ( int i = 0; i < numElements; i++ )
  {
    vtkSmartPointer< vtkXMLDataElement > currElement = rootElement->GetNestedElement( i );

    vtkSmartPointer< vtkLORPointObservation > currentObservation = vtkSmartPointer< vtkLORPointObservation >::New();
	if ( currentObservation->FromXMLElement( currElement, prevElement ) )
	{
	  CollectedPoints->AddObservation( currentObservation );
	}

	prevElement = currElement;
  }

  std::vector<int>* dof = new std::vector<int>();
  this->LinearObjectPoints = CollectedPoints->ExtractLinearObjects( collectionFrames, extractionThreshold, dof );

  // Sort the linear objects into their appropriate classes
  for ( int i = 0; i < this->LinearObjectPoints.size(); i++ )
  {

    // TODO: Calculate the noise properly
    vtkSmartPointer< vtkLORLinearObject > currentObject = LinearObjectPoints.at(i)->LeastSquaresLinearObject( dof->at(i) );

	// May be what we thought was linear actually isn't linear
	if ( currentObject == NULL )
	{
      continue;
	}

    if( strcmp( currentObject->Type.c_str(), "Point" ) == 0 )
	{
      this->RecordPointBuffer->AddLinearObject( currentObject );
	  this->PointPoints.push_back( LinearObjectPoints.at(i) );
	  this->PointPoints.at( this->PointPoints.size() - 1 )->Filter( this->RecordPointBuffer->GetLinearObject( this->RecordPointBuffer->Size() - 1 ), filterWidth );
	}
    if( strcmp( currentObject->Type.c_str(), "Line" ) == 0 )
	{
      this->RecordLineBuffer->AddLinearObject( currentObject );
	  this->LinePoints.push_back( LinearObjectPoints.at(i) );
      this->LinePoints.at( this->LinePoints.size() - 1 )->Filter( this->RecordLineBuffer->GetLinearObject( this->RecordLineBuffer->Size() - 1 ), filterWidth );
	}
	if( strcmp( currentObject->Type.c_str(), "Plane" ) == 0 )
	{
      this->RecordPlaneBuffer->AddLinearObject( currentObject );
	  this->PlanePoints.push_back( LinearObjectPoints.at(i) );
	  this->PlanePoints.at( this->PlanePoints.size() - 1 )->Filter( this->RecordPlaneBuffer->GetLinearObject( this->RecordPlaneBuffer->Size() - 1 ), filterWidth );
	}

  }

}




void vtkSlicerLinearObjectRegistrationLogic
::SetRegistrationTransformNode( vtkSmartPointer< vtkMRMLLinearTransformNode > newRegistrationTransformNode )
{
  this->RegistrationTransformNode = newRegistrationTransformNode;
}



double vtkSlicerLinearObjectRegistrationLogic
::GetError()
{
  return this->ErrorRMS;
}



std::string vtkSlicerLinearObjectRegistrationLogic
::GetStatus()
{
  return this->Status;
}



void vtkSlicerLinearObjectRegistrationLogic
::Register( double matchingThreshold )
{
  // Grab the collected references
  vtkSmartPointer< vtkLORLinearObjectBuffer > tempPointBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > tempPointPoints;
  for ( int i = 0; i < this->RecordPointBuffer->Size(); i++ )
  {
    if ( i < this->GeometryReferenceBuffer->Size() )
	{
      this->RecordReferenceBuffer->AddLinearObject( this->RecordPointBuffer->GetLinearObject(i) );
	}
	else
	{
      tempPointBuffer->AddLinearObject( this->RecordPointBuffer->GetLinearObject(i) );
	  tempPointPoints.push_back( this->PointPoints.at(i) );
	}
  }
  this->RecordPointBuffer = tempPointBuffer;
  this->PointPoints = tempPointPoints;

  if ( this->RecordReferenceBuffer->Size() < this->GeometryReferenceBuffer->Size() )
  {
    this->Status = "Failed - Could not identify reference(s)!";
	this->ErrorRMS = 0.0;
	return;
  }

  // Calculate the signature for all objects
  this->GeometryPointBuffer->CalculateSignature( this->GeometryReferenceBuffer );
  this->GeometryLineBuffer->CalculateSignature( this->GeometryReferenceBuffer );
  this->GeometryPlaneBuffer->CalculateSignature( this->GeometryReferenceBuffer );

  this->RecordPointBuffer->CalculateSignature( this->RecordReferenceBuffer );
  this->RecordLineBuffer->CalculateSignature( this->RecordReferenceBuffer );
  this->RecordPlaneBuffer->CalculateSignature( this->RecordReferenceBuffer );


  // Next, match objects based on their signatures
  // TODO: Check for memory leak here
  this->GeometryPointBuffer = this->RecordPointBuffer->GetMatches( this->GeometryPointBuffer, matchingThreshold );
  this->GeometryLineBuffer = this->RecordLineBuffer->GetMatches( this->GeometryLineBuffer, matchingThreshold );
  this->GeometryPlaneBuffer = this->RecordPlaneBuffer->GetMatches( this->GeometryPlaneBuffer, matchingThreshold );


  // Calculate the centroids
  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryCentroidBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  GeometryCentroidBuffer->Concatenate( this->GeometryPointBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryLineBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryPlaneBuffer );

  vtkSmartPointer< vtkLORLinearObjectBuffer > RecordCentroidBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
  RecordCentroidBuffer->Concatenate( this->RecordPointBuffer );
  RecordCentroidBuffer->Concatenate( this->RecordLineBuffer );
  RecordCentroidBuffer->Concatenate( this->RecordPlaneBuffer );

  std::vector<double> GeometryCentroid, RecordCentroid; 
  try
  {
    GeometryCentroid = GeometryCentroidBuffer->CalculateCentroid();
	RecordCentroid = RecordCentroidBuffer->CalculateCentroid();
  }
  catch( std::logic_error e )
  {
    this->Status = e.what();
	this->ErrorRMS = 0.0;
	return;
  }

  std::vector<double> NegativeGeometryCentroid( GeometryCentroid.size(), 0.0 );
  NegativeGeometryCentroid = Subtract( NegativeGeometryCentroid, GeometryCentroid );

  std::vector<double> NegativeRecordCentroid( RecordCentroid.size(), 0.0 );
  NegativeRecordCentroid = Subtract( NegativeRecordCentroid, RecordCentroid );


  // Now, translate everything by the negative centroid
  this->GeometryPointBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryLineBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryPlaneBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryReferenceBuffer->Translate( NegativeGeometryCentroid );

  this->RecordPointBuffer->Translate( NegativeRecordCentroid );
  this->RecordLineBuffer->Translate( NegativeRecordCentroid );
  this->RecordPlaneBuffer->Translate( NegativeRecordCentroid );
  this->RecordReferenceBuffer->Translate( NegativeRecordCentroid );

  for ( int i = 0; i < this->PointPoints.size(); i++ )
  {
    this->PointPoints.at(i)->Translate( NegativeRecordCentroid );
  }
  for ( int i = 0; i < this->LinePoints.size(); i++ )
  {
    this->LinePoints.at(i)->Translate( NegativeRecordCentroid );
  }
  for ( int i = 0; i < this->PlanePoints.size(); i++ )
  {
    this->PlanePoints.at(i)->Translate( NegativeRecordCentroid );
  }


  // Next, add the base points to the final point observation vectors
  std::vector<double> BlankVector( GeometryCentroid.size(), 0.0 );

  vtkSmartPointer< vtkLORPointObservationBuffer > GeometryPoints = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
  for ( int i = 0; i < this->GeometryPointBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->GeometryPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->GeometryLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->GeometryPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }

  vtkSmartPointer< vtkLORPointObservationBuffer > RecordPoints = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
  for ( int i = 0; i < this->RecordPointBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->RecordPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    RecordPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->RecordLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    RecordPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
    std::vector<double> basePoint = this->RecordPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector );
    RecordPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( basePoint ) ) );
  }


  // Then, add the direction vector to the final point observation vectors
  const int DIRECTION_SCALE = 100;
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    vtkSmartPointer< vtkLORLine > CurrentGeometryObject = vtkLORLine::SafeDownCast( this->GeometryLineBuffer->GetLinearObject(i) );
    std::vector<double> scaledDirection = Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() );
    GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( scaledDirection ) ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    vtkSmartPointer< vtkLORPlane > CurrentGeometryObject = vtkLORPlane::SafeDownCast( this->GeometryPlaneBuffer->GetLinearObject(i) );
    std::vector<double> scaledNormal = Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() );
    GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( scaledNormal ) ) );
  }

  // TODO: Fix memory leaks
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
    vtkSmartPointer< vtkLORLinearObjectBuffer > TempRecordBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
    vtkSmartPointer< vtkLORLinearObjectBuffer > TempGeometryBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
    std::vector<double> testVector;
    

    vtkSmartPointer< vtkLORLine > CurrentRecordObject = vtkLORLine::SafeDownCast( this->RecordLineBuffer->GetLinearObject(i) );

    testVector = Add( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentRecordObject->GetDirection() ) );
    TempRecordBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );

    testVector = Subtract( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentRecordObject->GetDirection() ) );
    TempRecordBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );
    
    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );


    vtkSmartPointer< vtkLORLine > CurrentGeometryObject = vtkLORLine::SafeDownCast( this->GeometryLineBuffer->GetLinearObject(i) );

    testVector = Add( CurrentGeometryObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() ) );
    TempGeometryBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );
    
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );


	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer, matchingThreshold );

    std::vector<double> scaledDirection = Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->ProjectVector( BlankVector ) );
    RecordPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( scaledDirection ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
    vtkSmartPointer< vtkLORLinearObjectBuffer > TempRecordBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
    vtkSmartPointer< vtkLORLinearObjectBuffer > TempGeometryBuffer = vtkSmartPointer< vtkLORLinearObjectBuffer >::New();
    std::vector<double> testVector;
    
    vtkSmartPointer< vtkLORPlane > CurrentRecordObject = vtkLORPlane::SafeDownCast( this->RecordPlaneBuffer->GetLinearObject(i) );

    testVector = Add( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE,  CurrentRecordObject->GetNormal() ) );
    TempRecordBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );
    
    testVector = Subtract( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE,  CurrentRecordObject->GetNormal() ) );
    TempRecordBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );

    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );


    vtkSmartPointer< vtkLORPlane > CurrentGeometryObject = vtkLORPlane::SafeDownCast( this->GeometryPlaneBuffer->GetLinearObject(i) );
    
    testVector = Add( CurrentGeometryObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() ) );
    TempGeometryBuffer->AddLinearObject( vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( testVector ) ) );
    
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );

	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer, matchingThreshold );

    std::vector<double> scaledNormal = Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->ProjectVector( BlankVector ) );
    RecordPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( scaledNormal ) ) );
  }


  // Finally, calculate the registration
  vnl_matrix<double>* RecordToGeometryRotation;

  try
  {
    RecordToGeometryRotation = GeometryPoints->SphericalRegistration( RecordPoints );
	RecordToGeometryRotation = this->LinearObjectICP( RecordToGeometryRotation );
  }
  catch( std::logic_error e )
  {
    this->Status = e.what();
	this->ErrorRMS = 0.0;
	return;
  }

  vnl_matrix<double>* RecordToGeometryTranslation = GeometryPoints->TranslationalRegistration( GeometryCentroid, RecordCentroid, RecordToGeometryRotation ); 


  // And set the output matrix
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 0, 0, RecordToGeometryRotation->get( 0,0 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 0, 1, RecordToGeometryRotation->get( 0,1 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 0, 2, RecordToGeometryRotation->get( 0,2 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 0, 3, RecordToGeometryTranslation->get( 0,0 ) );

  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 1, 0, RecordToGeometryRotation->get( 1,0 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 1, 1, RecordToGeometryRotation->get( 1,1 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 1, 2, RecordToGeometryRotation->get( 1,2 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 1, 3, RecordToGeometryTranslation->get( 1,0 ) );

  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 2, 0, RecordToGeometryRotation->get( 2,0 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 2, 1, RecordToGeometryRotation->get( 2,1 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 2, 2, RecordToGeometryRotation->get( 2,2 ) );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 2, 3, RecordToGeometryTranslation->get( 2,0 ) );

  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 3, 0, 0 );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 3, 1, 0 );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 3, 2, 0 );
  this->RegistrationTransformNode->GetMatrixTransformToParent()->SetElement( 3, 3, 1 );

  this->Status = "Success!";

}



vnl_matrix<double>* vtkSlicerLinearObjectRegistrationLogic
::LinearObjectICP( vnl_matrix<double>* initialRotation )
{

  const int CONVERGENCE_THRESHOLD = 1e-6;
  double currError = -1;
  double prevError = -1;
  vnl_matrix<double>* currRotation = initialRotation;

  while ( prevError < 0 || abs( currError - prevError ) < CONVERGENCE_THRESHOLD )
  {
    vtkSmartPointer< vtkLORPointObservationBuffer > GeometryPoints = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
    vtkSmartPointer< vtkLORPointObservationBuffer > RecordPoints = vtkSmartPointer< vtkLORPointObservationBuffer >::New();

	prevError = currError;

	// Find the closest point on each linear object to each point so we can do spherical registration
    for ( int i = 0; i < this->PointPoints.size(); i++ )
	{
      for ( int j = 0; j < this->PointPoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->PointPoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->PointPoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkLORPointObservation > rotPoint = vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryPointBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( projection ) ) );

	  }
	}

    for ( int i = 0; i < this->LinePoints.size(); i++ )
	{
      for ( int j = 0; j < this->LinePoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->LinePoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->LinePoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkLORPointObservation > rotPoint = vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryLineBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( projection ) ) );

	  }
	}

    for ( int i = 0; i < this->PlanePoints.size(); i++ )
	{
      for ( int j = 0; j < this->PlanePoints.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( this->PlanePoints.at(i)->GetObservation(j) );

		// Rotate the observed point
        std::vector<double> currObservation = this->PlanePoints.at(i)->GetObservation(j)->Observation;
        vtkSmartPointer< vtkLORPointObservation > rotPoint = vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( currObservation ) );
        rotPoint->Rotate( currRotation );

        std::vector<double> projection = this->GeometryPlaneBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation );
        GeometryPoints->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( projection ) ) );

	  }
	}

	// Now perform the spherical registration and calculate the rms error
    currRotation = GeometryPoints->SphericalRegistration( RecordPoints );

    currError = 0;
	for ( int i = 0; i < GeometryPoints->Size(); i++ )
	{
	  // Rotate the observed point
      std::vector<double> currObservation = RecordPoints->GetObservation(i)->Observation;
      vtkSmartPointer< vtkLORPointObservation > rotPoint = vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( currObservation ) );
      rotPoint->Rotate( currRotation );

	  currError = currError + Distance( GeometryPoints->GetObservation(i)->Observation, rotPoint->Observation );
	}
	currError = sqrt( currError / GeometryPoints->Size() );

  }

  this->ErrorRMS = currError;
  return currRotation;

}