
// LinearObjectRegistration Logic includes
#include "vtkSlicerLinearObjectRegistrationLogic.h"

// MRML includes
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLTransformNode.h"

// VTK includes
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkModifiedBSPTree.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkSmartPointer.h>

// STD includes
#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>



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
  this->GeometryBuffer = new LinearObjectBuffer();
  this->GeometryPointBuffer = new LinearObjectBuffer();
  this->GeometryLineBuffer = new LinearObjectBuffer();
  this->GeometryPlaneBuffer = new LinearObjectBuffer();
  this->GeometryReferenceBuffer = new LinearObjectBuffer();

  this->RecordBuffer = new LinearObjectBuffer();
  this->RecordPointBuffer = new LinearObjectBuffer();
  this->RecordLineBuffer = new LinearObjectBuffer();
  this->RecordPlaneBuffer = new LinearObjectBuffer();
  this->RecordReferenceBuffer = new LinearObjectBuffer();

  this->RegistrationTransformNode = NULL;
}



vtkSlicerLinearObjectRegistrationLogic::
~vtkSlicerLinearObjectRegistrationLogic()
{
  delete this->GeometryBuffer;
  delete this->GeometryPointBuffer;
  delete this->GeometryLineBuffer;
  delete this->GeometryPlaneBuffer;
  delete this->GeometryReferenceBuffer;

  delete this->RecordBuffer;
  delete this->RecordPointBuffer;
  delete this->RecordLineBuffer;
  delete this->RecordPlaneBuffer;
  delete this->RecordReferenceBuffer;
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
::ImportGeometry( std::string fileName )
{

  // Read the record log from XML file
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.c_str() );
  parser->Parse();
  vtkXMLDataElement* rootElement = parser->GetRootElement();

  GeometryBuffer->FromXMLElement( rootElement );

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
::ImportRecord( std::string fileName )
{

  // Read the record log from XML file
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.c_str() );
  parser->Parse();
  vtkXMLDataElement* rootElement = parser->GetRootElement();

  int numElements = rootElement->GetNumberOfNestedElements();  // Number of saved records (including transforms and messages).

  // We will store the times at which things start
  std::vector<double> onTimes;
  std::vector<double> offTimes;
  std::vector<double> refTimes;

  PointObservationBuffer* blank = new PointObservationBuffer();
  this->LinearObjectPoints = std::vector<PointObservationBuffer*>( 0, blank );
  this->ReferencePoints = std::vector<PointObservationBuffer*>( 0, blank );
  
  for ( int i = 0; i < numElements; i++ )
  {
    vtkXMLDataElement* noteElement = rootElement->GetNestedElement( i );

    if ( strcmp( noteElement->GetName(), "log" ) != 0 )
    {
      continue;  // If it's not a "log", jump to the next.
    }

	if ( strcmp( noteElement->GetAttribute( "type" ), "message" ) == 0 )
	{
	  double time = atoi( noteElement->GetAttribute( "TimeStampSec" ) ) + 1.0e-9 * atoi( noteElement->GetAttribute( "TimeStampNSec" ) );

	  if ( strcmp( noteElement->GetAttribute( "message" ), "on" ) == 0 )
	  {
        onTimes.push_back( time );
		LinearObjectPoints.push_back( new PointObservationBuffer() );
	  }
      if ( strcmp( noteElement->GetAttribute( "message" ), "off" ) == 0 )
	  {
        offTimes.push_back( time );
	  }
      if ( strcmp( noteElement->GetAttribute( "message" ), "reference" ) == 0 )
	  {
        refTimes.push_back( time );
		ReferencePoints.push_back( new PointObservationBuffer() );
	  }


	}
  }

  // Now, for each time stamp, determine if it was on, off, or reference
  for ( int i = 0; i < numElements; i++ )
  {
    vtkXMLDataElement* noteElement = rootElement->GetNestedElement( i );

    if ( strcmp( noteElement->GetName(), "log" ) != 0 || strcmp( noteElement->GetAttribute( "type" ), "transform" ) != 0 )
    {
      continue;  // If it's not a "log", jump to the next.
    }
	
	double time = atoi( noteElement->GetAttribute( "TimeStampSec" ) ) + 1.0e-9 * atoi( noteElement->GetAttribute( "TimeStampNSec" ) );
	int bestOnIndex = 0;
	int bestOffIndex = 0;
	int bestRefIndex = 0;

	for ( int i = 0; i < onTimes.size(); i++ )
	{
      if ( onTimes.at(i) <= time && onTimes.at(i) > onTimes.at(bestOnIndex) )
	  {
        bestOnIndex = i;
	  }
	}

    for ( int i = 0; i < offTimes.size(); i++ )
	{
      if ( offTimes.at(i) <= time && offTimes.at(i) > offTimes.at(bestOffIndex) )
	  {
        bestOffIndex = i;
	  }
	}

	for ( int i = 0; i < refTimes.size(); i++ )
	{
      if ( refTimes.at(i) <= time && refTimes.at(i) > refTimes.at(bestRefIndex) )
	  {
        bestRefIndex = i;
	  }
	}

    PointObservation* currentObservation = new PointObservation();
	currentObservation->FromXMLElement( noteElement );

	if ( ( onTimes.at(bestOnIndex) >= offTimes.at(bestOffIndex) || offTimes.at(bestOffIndex) > time ) && ( onTimes.at(bestOnIndex) >= refTimes.at(bestRefIndex) || refTimes.at(bestRefIndex) > time ) && onTimes.at(bestOnIndex) <= time )
	{
      LinearObjectPoints.at( bestOnIndex )->AddObservation( currentObservation );
	}
	if ( ( refTimes.at(bestRefIndex) >= offTimes.at(bestOffIndex) || offTimes.at(bestOffIndex) > time ) && ( refTimes.at(bestRefIndex) >= onTimes.at(bestOnIndex) || onTimes.at(bestOnIndex) > time ) && refTimes.at(bestRefIndex) <= time )
	{
      ReferencePoints.at( bestRefIndex )->AddObservation( currentObservation );
	}
	// If its "off" then don't do anything

  }

  // Sort the linear objects into their appropriate classes
  for ( int i = 0; i < LinearObjectPoints.size(); i++ )
  {

    // TODO: Calculate the noise properly
    LinearObject* currentObject = LinearObjectPoints.at(i)->LeastSquaresLinearObject( LinearObjectPoints.at(i)->CalculateNoise() );

    if( strcmp( currentObject->Type.c_str(), "Point" ) == 0 )
	{
      this->RecordPointBuffer->AddLinearObject( currentObject );
	}
    if( strcmp( currentObject->Type.c_str(), "Line" ) == 0 )
	{
      this->RecordLineBuffer->AddLinearObject( currentObject );
	}
	if( strcmp( currentObject->Type.c_str(), "Plane" ) == 0 )
	{
      this->RecordPlaneBuffer->AddLinearObject( currentObject );
	}

  }

  for ( int i = 0; i < ReferencePoints.size(); i++ )
  {
    Reference* currentReference = new Reference( ReferencePoints.at(i)->LeastSquaresLinearObject( ReferencePoints.at(i)->CalculateNoise() )->BasePoint );
    this->RecordReferenceBuffer->AddLinearObject( currentReference );
  }

}



void vtkSlicerLinearObjectRegistrationLogic
::SetRegistrationTransformNode( vtkMRMLLinearTransformNode* newRegistrationTransformNode )
{
  this->RegistrationTransformNode = newRegistrationTransformNode;
}



void vtkSlicerLinearObjectRegistrationLogic
::Register()
{
  // Calculate the signature for all objects
  this->GeometryPointBuffer->CalculateSignature( this->GeometryReferenceBuffer );
  this->GeometryLineBuffer->CalculateSignature( this->GeometryReferenceBuffer );
  this->GeometryPlaneBuffer->CalculateSignature( this->GeometryReferenceBuffer );

  this->RecordPointBuffer->CalculateSignature( this->RecordReferenceBuffer );
  this->RecordLineBuffer->CalculateSignature( this->RecordReferenceBuffer );
  this->RecordPlaneBuffer->CalculateSignature( this->RecordReferenceBuffer );


  // Next, match objects based on their signatures
  // TODO: Check for memory leak here
  this->GeometryPointBuffer = this->RecordPointBuffer->GetMatches( this->GeometryPointBuffer );
  this->GeometryLineBuffer = this->RecordLineBuffer->GetMatches( this->GeometryLineBuffer );
  this->GeometryPlaneBuffer = this->RecordPlaneBuffer->GetMatches( this->GeometryPlaneBuffer );


  // Calculate the centroids
  LinearObjectBuffer* GeometryCentroidBuffer = new LinearObjectBuffer();
  GeometryCentroidBuffer->Concatenate( this->GeometryPointBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryLineBuffer );
  GeometryCentroidBuffer->Concatenate( this->GeometryPlaneBuffer );
  std::vector<double> GeometryCentroid = GeometryCentroidBuffer->CalculateCentroid();
  std::vector<double> NegativeGeometryCentroid( GeometryCentroid.size(), 0.0 );
  NegativeGeometryCentroid = LinearObject::Subtract( NegativeGeometryCentroid, GeometryCentroid );

  LinearObjectBuffer* RecordCentroidBuffer = new LinearObjectBuffer();
  RecordCentroidBuffer->Concatenate( this->RecordPointBuffer );
  RecordCentroidBuffer->Concatenate( this->RecordLineBuffer );
  RecordCentroidBuffer->Concatenate( this->RecordPlaneBuffer );
  std::vector<double> RecordCentroid = RecordCentroidBuffer->CalculateCentroid();
  std::vector<double> NegativeRecordCentroid( RecordCentroid.size(), 0.0 );
  NegativeRecordCentroid = LinearObject::Subtract( NegativeRecordCentroid, RecordCentroid );


  // Now, translate everything by the negative centroid
  this->GeometryPointBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryLineBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryPlaneBuffer->Translate( NegativeGeometryCentroid );
  this->GeometryReferenceBuffer->Translate( NegativeGeometryCentroid );

  this->RecordPointBuffer->Translate( NegativeRecordCentroid );
  this->RecordLineBuffer->Translate( NegativeRecordCentroid );
  this->RecordPlaneBuffer->Translate( NegativeRecordCentroid );
  this->RecordReferenceBuffer->Translate( NegativeRecordCentroid );


  // Next, add the base points to the final point observation vectors
  std::vector<double> BlankVector( GeometryCentroid.size(), 0.0 );

  PointObservationBuffer* GeometryPoints = new PointObservationBuffer();
  for ( int i = 0; i < this->GeometryPointBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( new PointObservation( this->GeometryPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( new PointObservation( this->GeometryLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    GeometryPoints->AddObservation( new PointObservation( this->GeometryPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }

  PointObservationBuffer* RecordPoints = new PointObservationBuffer();
  for ( int i = 0; i < this->RecordPointBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( new PointObservation( this->RecordPointBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( new PointObservation( this->RecordLineBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
    RecordPoints->AddObservation( new PointObservation( this->RecordPlaneBuffer->GetLinearObject(i)->ProjectVector( BlankVector ) ) );
  }


  // Then, add the direction vector to the final point observation vectors
  for ( int i = 0; i < this->GeometryLineBuffer->Size(); i++ )
  {
    Line* CurrentGeometryObject = (Line*) this->GeometryLineBuffer->GetLinearObject(i);
    GeometryPoints->AddObservation( new PointObservation( CurrentGeometryObject->GetDirection() ) );
  }
  for ( int i = 0; i < this->GeometryPlaneBuffer->Size(); i++ )
  {
    Plane* CurrentGeometryObject = (Plane*) this->GeometryPlaneBuffer->GetLinearObject(i);
    GeometryPoints->AddObservation( new PointObservation( CurrentGeometryObject->GetNormal() ) );
  }

  // TODO: Fix memory leaks
  for ( int i = 0; i < this->RecordLineBuffer->Size(); i++ )
  {
	LinearObjectBuffer* TempRecordBuffer = new LinearObjectBuffer();
    LinearObjectBuffer* TempGeometryBuffer = new LinearObjectBuffer();
    
	Line* CurrentRecordObject = (Line*) this->RecordLineBuffer->GetLinearObject(i);
	TempRecordBuffer->AddLinearObject( new Point( LinearObject::Add( CurrentRecordObject->BasePoint, CurrentRecordObject->GetDirection() ) ) );
	TempRecordBuffer->AddLinearObject( new Point( LinearObject::Subtract( CurrentRecordObject->BasePoint, CurrentRecordObject->GetDirection() ) ) );
    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );

	Line* CurrentGeometryObject = (Line*) this->GeometryLineBuffer->GetLinearObject(i);
    TempGeometryBuffer->AddLinearObject( new Point( LinearObject::Add( CurrentGeometryObject->BasePoint, CurrentGeometryObject->GetDirection() ) ) );
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );

	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer );

	RecordPoints->AddObservation( new PointObservation( LinearObject::Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->BasePoint ) ) );
  }
  for ( int i = 0; i < this->RecordPlaneBuffer->Size(); i++ )
  {
	LinearObjectBuffer* TempRecordBuffer = new LinearObjectBuffer();
    LinearObjectBuffer* TempGeometryBuffer = new LinearObjectBuffer();
    
	Plane* CurrentRecordObject = (Plane*) this->RecordPlaneBuffer->GetLinearObject(i);
	TempRecordBuffer->AddLinearObject( new Point( LinearObject::Add( CurrentRecordObject->BasePoint, CurrentRecordObject->GetNormal() ) ) );
	TempRecordBuffer->AddLinearObject( new Point( LinearObject::Subtract( CurrentRecordObject->BasePoint, CurrentRecordObject->GetNormal() ) ) );
    TempRecordBuffer->CalculateSignature( this->RecordReferenceBuffer );

	Plane* CurrentGeometryObject = (Plane*) this->GeometryPlaneBuffer->GetLinearObject(i);
    TempGeometryBuffer->AddLinearObject( new Point( LinearObject::Add( CurrentGeometryObject->BasePoint, CurrentGeometryObject->GetNormal() ) ) );
    TempGeometryBuffer->CalculateSignature( this->GeometryReferenceBuffer );

	TempRecordBuffer = TempGeometryBuffer->GetMatches( TempRecordBuffer );

	RecordPoints->AddObservation( new PointObservation( LinearObject::Subtract( TempRecordBuffer->GetLinearObject(0)->BasePoint, CurrentRecordObject->BasePoint ) ) );
  }


  // Finally, calculate the registration
  vnl_matrix<double>* RecordToGeometryRotation = GeometryPoints->SphericalRegistration( RecordPoints );
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

}