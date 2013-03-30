
#include "PointObservation.h"

PointObservation
::PointObservation()
{
}


PointObservation
::PointObservation( std::vector<double> newObervation )
{
  this->Observation = newObervation;
}


PointObservation
::~PointObservation()
{
  this->Observation.clear();
}



std::string PointObservation
::ToXMLString()
{
  std::stringstream xmlstring;
  std::stringstream matrixstring;
  matrixstring << "0 0 0 " << this->Observation.at(0) << " ";
  matrixstring << "0 0 0 " << this->Observation.at(1) << " ";
  matrixstring << "0 0 0 " << this->Observation.at(2) << " ";
  matrixstring << "0 0 0 1";

  xmlstring << "  <log";
  xmlstring << " TimeStampSec=\"" << 0 << "\"";
  xmlstring << " TimeStampNSec=\"" << 0 << "\"";
  xmlstring << " type=\"transform\"";
  xmlstring << " DeviceName=\"" << "Pointer" << "\"";
  xmlstring << " transform=\"" << matrixstring << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void PointObservation
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "log" ) != 0 || strcmp( element->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Observation = std::vector<double>( SIZE, 0.0 );

  std::stringstream matrixstring( std::string( element->GetAttribute( "transform" ) ) );
  double value;

  for ( int i = 0; i < 16; i++ )
  {
    matrixstring >> value;
	if ( i == 3 )
	{
	  this->Observation.at(0) = value;
	}
	if ( i == 7 )
	{
	  this->Observation.at(1) = value;
	}
	if ( i == 11 )
	{
	  this->Observation.at(2) = value;
	}
  }

}