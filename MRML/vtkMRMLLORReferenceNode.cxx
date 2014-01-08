
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


std::string vtkMRMLLORReferenceNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Reference";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORReferenceNode
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{

  if ( strcmp( element->GetName(), "Reference" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}