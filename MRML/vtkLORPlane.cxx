
#include "vtkLORPlane.h"

vtkStandardNewMacro( vtkLORPlane );


vtkLORPlane* vtkLORPlane
::New( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 )
{
  vtkLORPlane* newPlane = vtkLORPlane::New();
  newPlane->BasePoint = newBasePoint;
  newPlane->EndPoint1 = newEndPoint1;
  newPlane->EndPoint2 = newEndPoint2;
  return newPlane;
}


vtkLORPlane
::vtkLORPlane()
{
  this->Type = "Plane";
}


vtkLORPlane
::~vtkLORPlane()
{
  this->EndPoint1.clear();
  this->EndPoint2.clear();
}


vtkSmartPointer< vtkLORLinearObject > vtkLORPlane
::DeepCopy()
{
  vtkSmartPointer< vtkLORPlane > objectNodeCopy = vtkSmartPointer< vtkLORPlane >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
  objectNodeCopy->EndPoint1 = this->EndPoint1;
  objectNodeCopy->EndPoint2 = this->EndPoint2;
  objectNodeCopy->Signature = this->Signature;

  objectNodeCopy->SetName( this->GetName() );
  objectNodeCopy->SetType( this->GetType() );
  objectNodeCopy->SetModelHierarchyNodeID( this->GetModelHierarchyNodeID() );

  if ( this->GetPositionBuffer() != NULL )
  {
    objectNodeCopy->SetPositionBuffer( this->GetPositionBuffer()->DeepCopy() );
  }

  return objectNodeCopy;
}


std::vector<double> vtkLORPlane
::GetNormal()
{
  std::vector<double> vector = LORMath::Cross( LORMath::Subtract( this->EndPoint1, this->BasePoint ), LORMath::Subtract( this->EndPoint2, this->BasePoint ) );
  vector = LORMath::Multiply( 1 / LORMath::Norm( vector ), vector );
  return vector;
}


bool vtkLORPlane
::IsCoincident( vtkLORLinearObject* testLinearObject, double threshold )
{
  vtkLORPlane* testPlane = vtkLORPlane::SafeDownCast( testLinearObject );
  if ( testPlane == NULL )
  {
    return false; 
  }
  
  // Test the same direction vector
  if ( 1 - abs( LORMath::Dot( this->GetNormal(), testPlane->GetNormal() ) ) > threshold )
  {
    return false;
  }

  // Test the base points lie on each other
  std::vector<double> baseVector = LORMath::Normalize( LORMath::Subtract( this->GetBasePoint(), testPlane->GetBasePoint() ) );
  if ( abs( LORMath::Dot( this->GetNormal(), baseVector ) ) > threshold )
  {
    return false;
  }
  if ( abs( LORMath::Dot( testPlane->GetNormal(), baseVector ) ) > threshold )
  {
    return false;
  }

  return true;
}


std::vector<double> vtkLORPlane
::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = LORMath::Subtract( vector, this->BasePoint );
  return LORMath::Subtract( vector, LORMath::Multiply( LORMath::Dot( this->GetNormal(), outVec ), this->GetNormal() ) );
}


void vtkLORPlane
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
	this->EndPoint1.at(i) = this->EndPoint1.at(i) + vector.at(i);
	this->EndPoint2.at(i) = this->EndPoint2.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkLORPlane
::CreateModelPolyData()
{
  vtkPlaneSource* planeSource = vtkPlaneSource::New();

  if ( this->GetPositionBuffer() == NULL )
  {
    planeSource->SetOrigin( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
    planeSource->SetPoint1( this->EndPoint1.at(0), this->EndPoint1.at(1), this->EndPoint1.at(2) );
    planeSource->SetPoint2( this->EndPoint2.at(0), this->EndPoint2.at(1), this->EndPoint2.at(2) );
    planeSource->Update();
    return planeSource->GetOutput();
  }

  // Project point onto line
  // Dot product to find parameterization
  // Use biggest and smallest parameters
  double maxParameter1 = 0;
  double minParameter1 = 0;
  double maxParameter2 = 0;
  double minParameter2 = 0;
  std::vector<double> directionVector1 = LORMath::Normalize( LORMath::Subtract( this->EndPoint1, this->BasePoint ) );
  std::vector<double> directionVector2 = LORMath::Normalize( LORMath::Subtract( this->EndPoint2, this->BasePoint ) );

  for ( int i = 0; i < this->GetPositionBuffer()->Size(); i++ )
  {
    // Project onto line
    std::vector<double> outVec = LORMath::Subtract( this->GetPositionBuffer()->GetPosition( i )->GetPositionVector(), this->BasePoint );

    double parameter1 = LORMath::Dot( directionVector1, outVec );
    double parameter2 = LORMath::Dot( directionVector2, outVec );

    if ( parameter1 > maxParameter1 )
    {
      maxParameter1 = parameter1;
    }
    if ( parameter1 < minParameter1 )
    {
      minParameter1 = parameter1;
    }
    if ( parameter2 > maxParameter2 )
    {
      maxParameter2 = parameter2;
    }
    if ( parameter2 < minParameter2 )
    {
      minParameter2 = parameter2;
    }
  }

  std::vector<double> maxVector1 = LORMath::Multiply( maxParameter1, directionVector1 );
  std::vector<double> minVector1 = LORMath::Multiply( minParameter1, directionVector1 );
  std::vector<double> maxVector2 = LORMath::Multiply( maxParameter2, directionVector2 );
  std::vector<double> minVector2 = LORMath::Multiply( minParameter2, directionVector2 );

  std::vector<double> origin = LORMath::Add( this->BasePoint, LORMath::Add( minVector1, minVector2 ) );
  std::vector<double> point1 = LORMath::Add( origin, LORMath::Subtract( maxVector1, minVector1 ) );
  std::vector<double> point2 = LORMath::Add( origin, LORMath::Subtract( maxVector2, minVector2 ) );

  planeSource->SetOrigin( origin.at(0), origin.at(1), origin.at(2) );
  planeSource->SetPoint1( point1.at(0), point1.at(1), point1.at(2) );
  planeSource->SetPoint2( point2.at(0), point2.at(1), point2.at(2) );
  planeSource->Update();
  return planeSource->GetOutput();
}


std::string vtkLORPlane
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Plane Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << LORMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint1 Value=\"" << LORMath::VectorToString( this->EndPoint1 ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint2 Value=\"" << LORMath::VectorToString( this->EndPoint2 ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Plane>" << std::endl;

  return xmlstring.str();
}


void vtkLORPlane
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Plane" ) != 0 )
  {
    return;  // If it's not a "Plane" jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkSmartPointer< vtkXMLDataElement > noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "BasePoint" ) == 0 )
	{
      this->BasePoint = LORMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkLORLinearObject::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint1" ) == 0 )
	{
      this->EndPoint1 = LORMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkLORLinearObject::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint2" ) == 0 )
	{
      this->EndPoint2 = LORMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkLORLinearObject::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkLORPositionBuffer > bufferNode = vtkSmartPointer< vtkLORPositionBuffer >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}