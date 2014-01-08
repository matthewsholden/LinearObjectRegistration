
#include "vtkMRMLLORPointNode.h"

vtkStandardNewMacro( vtkMRMLLORPointNode );


vtkMRMLLORPointNode* vtkMRMLLORPointNode
::New( std::vector<double> newBasePoint )
{
  vtkMRMLLORPointNode* newPoint = vtkMRMLLORPointNode::New();
  newPoint->BasePoint = newBasePoint;
  return newPoint;
}


vtkMRMLLORPointNode
::vtkMRMLLORPointNode()
{
  this->Type = "Point";
}


vtkMRMLLORPointNode
::~vtkMRMLLORPointNode()
{
}


std::vector<double> vtkMRMLLORPointNode
::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}


void vtkMRMLLORPointNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}


std::string vtkMRMLLORPointNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Point";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORPointNode
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{

  if ( strcmp( element->GetName(), "Point" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}