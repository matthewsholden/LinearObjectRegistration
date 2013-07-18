
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


std::vector<double> vtkLORPlane
::GetNormal()
{
  std::vector<double> vector = Cross( Subtract( this->EndPoint1, this->BasePoint ), Subtract( this->EndPoint2, this->BasePoint ) );
  vector = Multiply( 1 / Norm( vector ), vector );
  return vector;
}


std::vector<double> vtkLORPlane
::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = Subtract( vector, this->BasePoint );
  return Subtract( vector, Multiply( Dot( this->GetNormal(), outVec ), this->GetNormal() ) );
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


std::string vtkLORPlane
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Plane";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " EndPoint1=\"" << VectorToString( this->EndPoint1 ) << "\"";
  xmlstring << " EndPoint2=\"" << VectorToString( this->EndPoint2 ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkLORPlane
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Plane" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );
  this->EndPoint1 = StringToVector( std::string( element->GetAttribute( "EndPoint1" ) ), 3 );
  this->EndPoint2 = StringToVector( std::string( element->GetAttribute( "EndPoint2" ) ), 3 );

}