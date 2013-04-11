
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


void PointObservation
::Translate( std::vector<double> translation )
{
  this->Observation = LinearObject::Add( this->Observation, translation );
}


void PointObservation
::Rotate( vnl_matrix<double>* rotation )
{
  vnl_matrix<double>* currPoint = new vnl_matrix<double>( PointObservation::SIZE, 1, 0.0 );
  currPoint->put( 0, 0, this->Observation.at(0) );
  currPoint->put( 1, 0, this->Observation.at(1) );
  currPoint->put( 2, 0, this->Observation.at(2) );

  vnl_matrix<double>* rotPoint = new vnl_matrix<double>( ( *rotation ) * ( *currPoint ) );
  this->Observation.at(0) = rotPoint->get( 0, 0 );
  this->Observation.at(1) = rotPoint->get( 1, 0 );
  this->Observation.at(2) = rotPoint->get( 2, 0 );
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