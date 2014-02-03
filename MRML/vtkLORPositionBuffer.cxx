
#include "vtkLORPositionBuffer.h"

vtkStandardNewMacro( vtkLORPositionBuffer );


// Return smart pointer since we created the object here but do not hold onto it
vtkSmartPointer< vtkLORPositionBuffer > vtkLORPositionBuffer
::DeepCopy()
{
  vtkSmartPointer< vtkLORPositionBuffer > positionBufferNodeCopy = vtkSmartPointer< vtkLORPositionBuffer >::New();

  for ( int i = 0; i < this->Size(); i++ )
  {
    positionBufferNodeCopy->AddPosition( this->GetPosition( i )->DeepCopy() );
  }
  
  return positionBufferNodeCopy;
}


vtkLORPositionBuffer
::vtkLORPositionBuffer()
{
}


vtkLORPositionBuffer
::~vtkLORPositionBuffer()
{
  this->Clear();
}


int vtkLORPositionBuffer
::Size()
{
  return this->Positions.size();
}


vtkLORPosition* vtkLORPositionBuffer
::GetPosition( int index )
{
  return this->Positions.at(index);
}


void vtkLORPositionBuffer
::AddPosition( vtkLORPosition* newPosition )
{
  this->Positions.push_back( newPosition );
  this->Modified();
}


void vtkLORPositionBuffer
::Clear()
{
  this->Positions.clear();
  this->Modified();
}


void vtkLORPositionBuffer
::Trim( int trimSize )
{
  std::vector< vtkSmartPointer< vtkLORPosition > > newPositions;

  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( i - trimSize >= 0 && i + trimSize < this->Size() )
    {
      newPositions.push_back( this->GetPosition( i ) );
    }
  }

  this->Positions = newPositions;
  this->Modified();
}


void vtkLORPositionBuffer
::Translate( std::vector<double> translation )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetPosition(i)->Translate( translation );
  }
}


std::string vtkLORPositionBuffer
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "    <Buffer>" << std::endl;
  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetPosition(i)->ToXMLString();
  }
  xmlstring << "    </Buffer>" << std::endl;

  return xmlstring.str();
}


void vtkLORPositionBuffer
::FromXMLElement( vtkXMLDataElement* element )
{
  if ( strcmp( element->GetName(), "Buffer" ) != 0 && strcmp( element->GetName(), "PositionBuffer" ) != 0 )
  {
    return;
  }

  vtkSmartPointer< vtkLORPosition > blankPosition;
  this->Positions = std::vector< vtkSmartPointer< vtkLORPosition > >( 0, blankPosition );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {
    vtkSmartPointer< vtkXMLDataElement > noteElement = element->GetNestedElement( i );

    vtkSmartPointer< vtkLORPosition > newPosition = vtkSmartPointer< vtkLORPosition >::New();
	newPosition->FromXMLElement( noteElement );
    this->AddPosition( newPosition );
  }

}


vnl_matrix<double>* vtkLORPositionBuffer
::CovarianceMatrix( std::vector<double> centroid )
{
  // Construct a buffer for the zero mean data; initialize covariance matrix
  vtkSmartPointer< vtkLORPositionBuffer > zeroMeanBuffer = vtkSmartPointer< vtkLORPositionBuffer >::New();
  vnl_matrix<double> *cov = new vnl_matrix<double>( vtkLORPosition::SIZE, vtkLORPosition::SIZE );
  cov->fill( 0.0 );

  // Subtract the mean from each Position
  for ( int i = 0; i < this->Size(); i++ )
  {
    vtkSmartPointer< vtkLORPosition > newPosition = vtkSmartPointer< vtkLORPosition >::New();
    std::vector<double> newPositionVector = LORMath::Subtract( this->GetPosition(i)->GetPositionVector(), centroid );
    newPosition->SetPositionVector( newPositionVector );
	zeroMeanBuffer->AddPosition( newPosition );
  }

  // Pick two dimensions, and find their covariance
  for ( int d1 = 0; d1 < vtkLORPosition::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkLORPosition::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < zeroMeanBuffer->Size(); i++ )
	  {
	    cov->put( d1, d2, cov->get( d1, d2 ) + zeroMeanBuffer->GetPosition(i)->GetPositionVector().at(d1) * zeroMeanBuffer->GetPosition(i)->GetPositionVector().at(d2) );
	  }
	  // Divide by the number of records
	  cov->put( d1, d2, cov->get( d1, d2 ) / zeroMeanBuffer->Size() );
	}
  }

  return cov;

}


std::vector<double> vtkLORPositionBuffer
::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( vtkLORPosition::SIZE, 0.0 );
  for ( int i = 0; i < this->Size(); i++ )
  {
	  for ( int d = 0; d < vtkLORPosition::SIZE; d++ )
	{
      centroid.at(d) = centroid.at(d) + this->GetPosition(i)->GetPositionVector().at(d);
	}
  }
  for ( int d = 0; d < vtkLORPosition::SIZE; d++ )
  {
    centroid.at(d) = centroid.at(d) / this->Size();
  }

  return centroid;
}


int vtkLORPositionBuffer
::GetDOF( double noiseThreshold )
{
  std::vector<double> centroid = this->CalculateCentroid();
  vnl_matrix<double>* cov = this->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( vtkLORPosition::SIZE, vtkLORPosition::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( vtkLORPosition::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Get number of eigenvectors with eigenvalues larger than the threshold
  int calculatedDOF = 0;
  for ( int i = 0; i < vtkLORPosition::SIZE; i++ )
  {
    if ( abs( eigenvalues.get( i ) ) > LORConstants::NOISE_THRESHOLD )
    {
      calculatedDOF++;
    }
  }

  return calculatedDOF; 
}