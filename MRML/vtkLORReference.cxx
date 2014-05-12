
#include "vtkLORReference.h"

vtkStandardNewMacro( vtkLORReference );


vtkLORReference* vtkLORReference
::New( std::vector<double> newBasePoint )
{
  vtkLORReference* newReference = vtkLORReference::New();
  newReference->BasePoint = newBasePoint;
  return newReference;
}


vtkLORReference
::vtkLORReference()
{
  this->Name = LORConstants::REFERENCE_STRING;
  this->Type = LORConstants::REFERENCE_STRING;
}


vtkLORReference
::~vtkLORReference()
{
}


vtkSmartPointer< vtkLORLinearObject > vtkLORReference
::DeepCopy()
{
  vtkSmartPointer< vtkLORReference > objectNodeCopy = vtkSmartPointer< vtkLORReference >::New();

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


bool vtkLORReference
::IsCoincident( vtkLORLinearObject* testLinearObject, double threshold )
{
  vtkLORReference* testReference = vtkLORReference::SafeDownCast( testLinearObject );
  if ( testReference == NULL )
  {
    return false; 
  }
  
  // Test the distance
  if ( LORMath::Norm( LORMath::Subtract( this->GetBasePoint(), testReference->GetBasePoint() ) ) > threshold )
  {
    return false;
  }

  return true;
}


std::vector<double> vtkLORReference
::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}


void vtkLORReference
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkLORReference
::CreateModelPolyData()
{
  vtkPointSource* pointSource = vtkPointSource::New();

  pointSource->SetNumberOfPoints( 1 );
  pointSource->SetRadius( 0 );
  pointSource->SetCenter( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
  pointSource->Update();

  return pointSource->GetOutput();
}


std::string vtkLORReference
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Reference Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << LORMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Reference>" << std::endl;

  return xmlstring.str();
}


void vtkLORReference
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Reference" ) != 0 )
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