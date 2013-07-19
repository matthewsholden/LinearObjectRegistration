
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
  this->Type = "Point";
}


vtkLORPoint
::~vtkLORPoint()
{
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


std::string vtkLORPoint
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Point";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkLORPoint
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{

  if ( strcmp( element->GetName(), "Point" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}