
#include "vtkMRMLLORReferenceNode.h"

vtkStandardNewMacro( vtkMRMLLORReferenceNode );


vtkMRMLLORReferenceNode* vtkMRMLLORReferenceNode
::New( std::vector<double> newBasePoint )
{
  vtkMRMLLORReferenceNode* newReference = vtkMRMLLORReferenceNode::New();
  newReference->BasePoint = newBasePoint;
  return newReference;
}


vtkMRMLLORReferenceNode
::vtkMRMLLORReferenceNode()
{
  this->Type = "Reference";
}


vtkMRMLLORReferenceNode
::~vtkMRMLLORReferenceNode()
{
}


vtkSmartPointer< vtkMRMLLORLinearObjectNode > vtkMRMLLORReferenceNode
::DeepCopy()
{
  vtkSmartPointer< vtkMRMLLORReferenceNode > objectNodeCopy = vtkSmartPointer< vtkMRMLLORReferenceNode >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
  objectNodeCopy->Signature = this->Signature;

  objectNodeCopy->SetName( this->GetName() );
  objectNodeCopy->SetType( this->GetType() );

  if ( this->GetPositionBuffer() != NULL )
  {
    objectNodeCopy->SetPositionBuffer( this->GetPositionBuffer()->DeepCopy() );
  }

  return objectNodeCopy;
}


std::vector<double> vtkMRMLLORReferenceNode
::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}


void vtkMRMLLORReferenceNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkMRMLLORReferenceNode
::CreateModelPolyData()
{
  vtkPointSource* pointSource = vtkPointSource::New();

  pointSource->SetNumberOfPoints( 1 );
  pointSource->SetRadius( 0 );
  pointSource->SetCenter( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
  pointSource->Update();

  return pointSource->GetOutput();
}


std::string vtkMRMLLORReferenceNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Reference Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Reference>" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORReferenceNode
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
      this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPositionBufferNode > bufferNode = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}