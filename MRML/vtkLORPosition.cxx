
#include "vtkLORPosition.h"

vtkStandardNewMacro( vtkLORPosition );


vtkLORPosition* vtkLORPosition
::New( std::vector< double > newPositionVector )
{
  vtkLORPosition* newPositionNode = vtkLORPosition::New();
  newPositionNode->PositionVector = newPositionVector;
  return newPositionNode;
}

vtkLORPosition* vtkLORPosition
::New( vtkMatrix4x4* newMatrix )
{
  vtkLORPosition* newPositionNode = vtkLORPosition::New();
  std::vector< double > newPosition( SIZE, 0.0 );
  newPosition.at(0) = newMatrix->GetElement( 0, 3 );
  newPosition.at(1) = newMatrix->GetElement( 1, 3 );
  newPosition.at(2) = newMatrix->GetElement( 2, 3 );
  newPositionNode->PositionVector = newPosition;
  return newPositionNode;
}


vtkSmartPointer< vtkLORPosition > vtkLORPosition
::DeepCopy()
{
  vtkSmartPointer< vtkLORPosition > positionNodeCopy = vtkSmartPointer< vtkLORPosition >::New();
  std::vector< double > positionCopy( SIZE, 0.0 );
  positionCopy.at(0) = this->GetPositionVector().at( 0 );
  positionCopy.at(1) = this->GetPositionVector().at( 1 );
  positionCopy.at(2) = this->GetPositionVector().at( 2 );
  positionNodeCopy->SetPositionVector( positionCopy );
  return positionNodeCopy;
}


vtkLORPosition
::vtkLORPosition()
{
}


vtkLORPosition
::~vtkLORPosition()
{
  this->PositionVector.clear();
}


void vtkLORPosition
::Translate( std::vector<double> translation )
{
  this->SetPositionVector( LORMath::Add( this->GetPositionVector(), translation ) );
}


void vtkLORPosition
::Rotate( vnl_matrix<double>* rotation )
{
  vnl_matrix<double>* currPoint = new vnl_matrix<double>( vtkLORPosition::SIZE, 1, 0.0 );
  currPoint->put( 0, 0, this->GetPositionVector().at(0) );
  currPoint->put( 1, 0, this->GetPositionVector().at(1) );
  currPoint->put( 2, 0, this->GetPositionVector().at(2) );

  vnl_matrix<double>* rotPoint = new vnl_matrix<double>( ( *rotation ) * ( *currPoint ) );
  std::vector<double> newPositionVector( SIZE, 0.0 );
  newPositionVector.at(0) = rotPoint->get( 0, 0 );
  newPositionVector.at(1) = rotPoint->get( 1, 0 );
  newPositionVector.at(2) = rotPoint->get( 2, 0 );
  this->SetPositionVector( newPositionVector );
}


void vtkLORPosition
::Transform( vtkMatrix4x4* matrix )
{
  double untransformedPoint[ 4 ] = { 0.0, 0.0, 0.0, 1.0 };
  double transformedPoint[ 4 ] = { 0.0, 0.0, 0.0, 1.0 };

  untransformedPoint[ 0 ] = this->PositionVector.at( 0 );
  untransformedPoint[ 1 ] = this->PositionVector.at( 1 );
  untransformedPoint[ 2 ] = this->PositionVector.at( 2 );

  matrix->MultiplyPoint( untransformedPoint, transformedPoint );

  // Project onto linear object to find closest point
  std::vector<double> transformedVector;
  transformedVector.push_back( transformedPoint[ 0 ] );
  transformedVector.push_back( transformedPoint[ 1 ] );
  transformedVector.push_back( transformedPoint[ 2 ] );

  this->SetPositionVector( transformedVector );
}


std::vector<double> vtkLORPosition
::GetPositionVector()
{
  return this->PositionVector;
}


void vtkLORPosition
::SetPositionVector( std::vector<double> newPosition )
{
  this->PositionVector = newPosition;
  this->Modified();
}


std::string vtkLORPosition
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "      <Position";
  xmlstring << " Value=\"" << LORMath::VectorToString( this->GetPositionVector() ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkLORPosition
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Position" ) != 0 )
  {
    return;  // If it's not a "Position" jump to the next.
  }

  this->SetPositionVector( LORMath::StringToVector( std::string( element->GetAttribute( "Value" ) ), SIZE ) );
}


/*
bool vtkLORPosition
::FromXMLElement( vtkXMLDataElement* currElement, vtkXMLDataElement* prevElement )
{
  const double ROTATION_THRESHOLD = 0.005;
  const double TRANSLATION_THRESHOLD = 0.5;

  // Check to ensure that the transformation has changed since the previously collected element
  if ( strcmp( currElement->GetName(), "log" ) != 0 || strcmp( currElement->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return false;  // If it's not a "log" or is the wrong tool jump to the next.
  }
  if ( strcmp( prevElement->GetName(), "log" ) != 0 || strcmp( prevElement->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return false;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  std::stringstream currmatrixstring( std::string( currElement->GetAttribute( "transform" ) ) );
  std::stringstream prevmatrixstring( std::string( prevElement->GetAttribute( "transform" ) ) );
  double currValue, prevValue;

  std::vector<double> currRotation, prevRotation;
  std::vector<double> currTranslation, prevTranslation;

  for ( int i = 0; i < this->MATRIX_ELEMENTS; i++ )
  {
    currmatrixstring >> currValue;
	prevmatrixstring >> prevValue;
	if ( i == 0 || i == 1 || i == 2 || i == 4 || i == 5 || i == 6 || i == 8 || i == 9 || i == 10 ) // Rotation components
	{
	  currRotation.push_back( currValue );
	  prevRotation.push_back( prevValue );
	}
	if ( i == 3 || i == 7 || i == 11 ) // Translation components
	{
	  currTranslation.push_back( currValue );
	  prevTranslation.push_back( prevValue );
	}
  }

  // Check to ensure that the current position is appreciably different from the previous
  // If not, don't record (because the stylus was likely just lying around, not collecting)
  if ( LORMath::Distance( currRotation, prevRotation ) > ROTATION_THRESHOLD || LORMath::Distance( currTranslation, prevTranslation ) > TRANSLATION_THRESHOLD )
  {
    this->FromXMLElement( currElement );
	return true;
  }

  return false;
}
*/