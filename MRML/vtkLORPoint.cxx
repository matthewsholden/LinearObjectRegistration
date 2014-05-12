
#include "vtkLORPoint.h"

vtkStandardNewMacro( vtkLORPoint );


vtkLORPoint* vtkLORPoint
::New( std::vector<double> newBasePoint )
{
  vtkLORPoint* newPoint = vtkLORPoint::New();
  newPoint->BasePoint = newBasePoint;
  return newPoint;
}


vtkLORPoint
::vtkLORPoint()
{
  this->Name = LORConstants::POINT_STRING;
  this->Type = LORConstants::POINT_STRING;
}


vtkLORPoint
::~vtkLORPoint()
{
}


vtkSmartPointer< vtkLORLinearObject > vtkLORPoint
::DeepCopy()
{
  vtkSmartPointer< vtkLORPoint > objectNodeCopy = vtkSmartPointer< vtkLORPoint >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
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


bool vtkLORPoint
::IsCoincident( vtkLORLinearObject* testLinearObject, double threshold )
{
  vtkLORPoint* testPoint = vtkLORPoint::SafeDownCast( testLinearObject );
  if ( testPoint == NULL )
  {
    return false; 
  }
  
  // Test the distance
  if ( LORMath::Norm( LORMath::Subtract( this->GetBasePoint(), testPoint->GetBasePoint() ) ) > threshold )
  {
    return false;
  }

  return true;
}


std::vector<double> vtkLORPoint
::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}


void vtkLORPoint
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkLORPoint
::CreateModelPolyData()
{
  vtkPointSource* pointSource = vtkPointSource::New();

  pointSource->SetNumberOfPoints( 1 );
  pointSource->SetRadius( 0 );
  pointSource->SetCenter( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
  pointSource->Update();

  return pointSource->GetOutput();
}


std::string vtkLORPoint
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Point Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << LORMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Point>" << std::endl;

  return xmlstring.str();
}


void vtkLORPoint
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Point" ) != 0 )
  {
    return;  // If it's not a "reference" jump to the next.
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
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkLORPositionBuffer > bufferNode = vtkSmartPointer< vtkLORPositionBuffer >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}