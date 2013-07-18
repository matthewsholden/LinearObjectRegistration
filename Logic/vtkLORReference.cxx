
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
  this->Type = "Reference";
}


vtkLORReference
::~vtkLORReference()
{
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


std::string vtkLORReference
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Reference";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkLORReference
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Reference" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}