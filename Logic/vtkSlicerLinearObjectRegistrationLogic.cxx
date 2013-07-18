
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
  this->GeometryBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryPointBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryLineBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryPlaneBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryReferenceBuffer = vtkLORLinearObjectBuffer::New();

  this->RecordPointBuffer = vtkLORLinearObjectBuffer::New();
  this->RecordLineBuffer = vtkLORLinearObjectBuffer::New();
  this->RecordPlaneBuffer =  vtkLORLinearObjectBuffer::New();
  this->RecordReferenceBuffer = vtkLORLinearObjectBuffer::New();

  this->RegistrationTransformNode = NULL;
  this->ErrorRMS = 0.0;
  this->Status = "";
}


vtkSlicerLinearObjectRegistrationLogic::
~vtkSlicerLinearObjectRegistrationLogic()
{
  // Do not delete from the specific buffers - this general buffer has references to all the geometry linear objects
  this->GeometryBuffer->Delete();

  this->RecordPointBuffer->Delete();
  this->RecordLineBuffer->Delete();
  this->RecordPlaneBuffer->Delete();
  this->RecordReferenceBuffer->Delete();
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
  this->GeometryBuffer->Delete(); // This should delete everything

  this->GeometryBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryPointBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryLineBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryPlaneBuffer = vtkLORLinearObjectBuffer::New();
  this->GeometryReferenceBuffer = vtkLORLinearObjectBuffer::New();
}



void vtkSlicerLinearObjectRegistrationLogic
::ImportGeometry( std::string fileName )
{
  this->ResetGeometry();

  // Read the record log from XML file
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.c_str() );
  parser->Parse();
  vtkXMLDataElement* rootElement = parser->GetRootElement();

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
  this->RecordPointBuffer->Delete();
  this->RecordLineBuffer->Delete();
  this->RecordPlaneBuffer->Delete();
  this->RecordReferenceBuffer->Delete();

  this->RecordPointBuffer = vtkLORLinearObjectBuffer::New();
  this->RecordLineBuffer = vtkLORLinearObjectBuffer::New();
  this->RecordPlaneBuffer = vtkLORLinearObjectBuffer::New();
  this->RecordReferenceBuffer = vtkLORLinearObjectBuffer::New();

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
  vtkXMLDataElement* rootElement = parser->GetRootElement();

  int numElements = rootElement->GetNumberOfNestedElements();  // Number of saved records (including transforms and messages).

  vtkLORPointObservationBuffer* CollectedPoints = vtkLORPointObservationBuffer::New();

  vtkXMLDataElement* currElement = NULL;
  vtkXMLDataElement* prevElement = rootElement->GetNestedElement( 0 ); // We don't really need the first element (or care)

  for ( int i = 0; i < numElements; i++ )
  {
    vtkXMLDataElement* currElement = rootElement->GetNestedElement( i );

    vtkLORPointObservation* currentObservation = vtkLORPointObservation::New();
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
    vtkLORLinearObject* currentObject = LinearObjectPoints.at(i)->LeastSquaresLinearObject( dof->at(i) );

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
::SetRegistrationTransformNode( vtkMRMLLinearTransformNode* newRegistrationTransformNode )
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
  vtkLORLinearObjectBuffer* tempPointBuffer = vtkLORLinearObjectBuffer::New();
  std::vector<vtkLORPointObservationBuffer*> tempPointPoints;
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
  vtkLORLinearObjectBuffer* GeometryCentroidBuffer = vtkLORLinearObjectBuffer::New();
  GeometryCentroidBuffer->Concatenate( this->GeometryPointBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryLineBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryPlaneBuffer );

  vtkLORLinearObjectBuffer* RecordCentroidBuffer = vtkLORLinearObjectBuffer::New();
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

  vtkLORPointObservationBuffer* GeometryPoints = vtkLORPointObservationBuffer::New();
  for ( int i = 0; i < this->GeometryPointBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( vtkLORPointObservation::New( this->GeometryPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( vtkLORPointObservation::New( this->GeometryLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( vtkLORPointObservation::New( this->GeometryPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }

  vtkLORPointObservationBuffer* RecordPoints = vtkLORPointObservationBuffer::New();
  for ( int i = 0; i < this->RecordPointBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( vtkLORPointObservation::New( this->RecordPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( vtkLORPointObservation::New( this->RecordLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( vtkLORPointObservation::New( this->RecordPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }


  // Then, add the direction vector to the final point observation vectors
  const int DIRECTION_SCALE = 100;
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    vtkLORLine* CurrentGeometryObject = (vtkLORLine*) this->GeometryLineBuffer->GetLinearObject(i);
    GeometryPoints->AddObservation( vtkLORPointObservation::New( Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() ) ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    vtkLORPlane* CurrentGeometryObject = (vtkLORPlane*) this->GeometryPlaneBuffer->GetLinearObject(i);
    GeometryPoints->AddObservation( vtkLORPointObservation::New( Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() ) ) );
  }

  // TODO: Fix memory leaks
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
    vtkLORLinearObjectBuffer* TempRecordBuffer = vtkLORLinearObjectBuffer::New();
    vtkLORLinearObjectBuffer* TempGeometryBuffer = vtkLORLinearObjectBuffer::New();
    
	vtkLORLine* CurrentRecordObject = (vtkLORLine*) this->RecordLineBuffer->GetLinearObject(i);
    TempRecordBuffer->AddLinearObject( vtkLORPoint::New( Add( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentRecordObject->GetDirection() ) ) ) );
    TempRecordBuffer->AddLinearObject( vtkLORPoint::New( Subtract( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentRecordObject->GetDirection() ) ) ) );
    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );

	vtkLORLine* CurrentGeometryObject = (vtkLORLine*) this->GeometryLineBuffer->GetLinearObject(i);
    TempGeometryBuffer->AddLinearObject( vtkLORPoint::New( Add( CurrentGeometryObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetDirection() ) ) ) );
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );

	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer, matchingThreshold );

    RecordPoints->AddObservation( vtkLORPointObservation::New( Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->ProjectVector( BlankVector ) ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
    vtkLORLinearObjectBuffer* TempRecordBuffer = vtkLORLinearObjectBuffer::New();
    vtkLORLinearObjectBuffer* TempGeometryBuffer = vtkLORLinearObjectBuffer::New();
    
	vtkLORPlane* CurrentRecordObject = (vtkLORPlane*) this->RecordPlaneBuffer->GetLinearObject(i);
    TempRecordBuffer->AddLinearObject( vtkLORPoint::New( Add( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE,  CurrentRecordObject->GetNormal() ) ) ) );
    TempRecordBuffer->AddLinearObject( vtkLORPoint::New( Subtract( CurrentRecordObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentRecordObject->GetNormal() ) ) ) );
    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );

	vtkLORPlane* CurrentGeometryObject = (vtkLORPlane*) this->GeometryPlaneBuffer->GetLinearObject(i);
    TempGeometryBuffer->AddLinearObject( vtkLORPoint::New( Add( CurrentGeometryObject->ProjectVector( BlankVector ), Multiply( DIRECTION_SCALE, CurrentGeometryObject->GetNormal() ) ) ) );
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );

	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer, matchingThreshold );

    RecordPoints->AddObservation( vtkLORPointObservation::New( Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->ProjectVector( BlankVector ) ) ) );
  }


  // Finally, calculate the registration
  vnl_matrix<double>* RecordToGeometryRotation;

  try
  {
    RecordToGeometryRotation = GeometryPoints->SphericalRegistration( RecordPoints );
	RecordToGeometryRotation = this->LinearObjectICP( this->GeometryPointBuffer, this->GeometryLineBuffer, this->GeometryPlaneBuffer, this->PointPoints, this->LinePoints, this->PlanePoints, RecordToGeometryRotation );
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
::LinearObjectICP( vtkLORLinearObjectBuffer* pointBuffer, vtkLORLinearObjectBuffer* lineBuffer, vtkLORLinearObjectBuffer* planeBuffer,
				  std::vector<vtkLORPointObservationBuffer*> pointObservations, std::vector<vtkLORPointObservationBuffer*> lineObservations, std::vector<vtkLORPointObservationBuffer*> planeObservations,
				  vnl_matrix<double>* initialRotation )
{

  const int CONVERGENCE_THRESHOLD = 1e-6;
  double currError = -1;
  double prevError = -1;
  vnl_matrix<double>* currRotation = initialRotation;

  while ( prevError < 0 || abs( currError - prevError ) < CONVERGENCE_THRESHOLD )
  {
    vtkLORPointObservationBuffer* GeometryPoints = vtkLORPointObservationBuffer::New();
    vtkLORPointObservationBuffer* RecordPoints = vtkLORPointObservationBuffer::New();

	prevError = currError;

	// Find the closest point on each linear object to each point so we can do spherical registration
    for ( int i = 0; i < pointObservations.size(); i++ )
	{
      for ( int j = 0; j < pointObservations.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( pointObservations.at(i)->GetObservation(j) );

		// Rotate the observed point
        vtkLORPointObservation* rotPoint = vtkLORPointObservation::New( pointObservations.at(i)->GetObservation(j)->Observation );
        rotPoint->Rotate( currRotation );

        GeometryPoints->AddObservation( vtkLORPointObservation::New( pointBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation ) ) );

	  }
	}

    for ( int i = 0; i < lineObservations.size(); i++ )
	{
      for ( int j = 0; j < lineObservations.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( lineObservations.at(i)->GetObservation(j) );

		// Rotate the observed point
        vtkLORPointObservation* rotPoint = vtkLORPointObservation::New( lineObservations.at(i)->GetObservation(j)->Observation );
        rotPoint->Rotate( currRotation );

        GeometryPoints->AddObservation( vtkLORPointObservation::New( lineBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation ) ) );

	  }
	}

    for ( int i = 0; i < planeObservations.size(); i++ )
	{
      for ( int j = 0; j < planeObservations.at(i)->Size(); j++ )
	  {

        RecordPoints->AddObservation( planeObservations.at(i)->GetObservation(j) );

		// Rotate the observed point
        vtkLORPointObservation* rotPoint = vtkLORPointObservation::New( planeObservations.at(i)->GetObservation(j)->Observation );
        rotPoint->Rotate( currRotation );

        GeometryPoints->AddObservation( vtkLORPointObservation::New( planeBuffer->GetLinearObject(i)->ProjectVector( rotPoint->Observation ) ) );

	  }
	}

	// Now perform the spherical registration and calculate the rms error
    currRotation = GeometryPoints->SphericalRegistration( RecordPoints );

    currError = 0;
	for ( int i = 0; i < GeometryPoints->Size(); i++ )
	{
	  // Rotate the observed point
      vtkLORPointObservation* rotPoint = vtkLORPointObservation::New( RecordPoints->GetObservation(i)->Observation );
      rotPoint->Rotate( currRotation );

	  currError = currError + Distance( GeometryPoints->GetObservation(i)->Observation, rotPoint->Observation );
	}
	currError = sqrt( currError / GeometryPoints->Size() );

  }

  this->ErrorRMS = currError;
  return currRotation;

}