
#include "vtkLORRealTimePositionBuffer.h"

vtkStandardNewMacro( vtkLORRealTimePositionBuffer );


vtkLORRealTimePositionBuffer
::vtkLORRealTimePositionBuffer()
{
  this->CurrentOuterProductSum = new vnl_matrix<double>( vtkLORPosition::SIZE, vtkLORPosition::SIZE, 0.0 );
  this->CurrentVectorSum = std::vector<double>( vtkLORPosition::SIZE, 0.0 );
}


vtkLORRealTimePositionBuffer
::~vtkLORRealTimePositionBuffer()
{
  this->Clear();
}


void vtkLORRealTimePositionBuffer
::AddPosition( vtkLORPosition* newPosition )
{
  this->CurrentVectorSum = LORMath::Add( this->CurrentVectorSum, newPosition->GetPositionVector() );

  // Find the outer product matrix and add
  vnl_matrix<double> verticalMatrix( vtkLORPosition::SIZE, 1, 0.0 );
  vnl_matrix<double> horizontalMatrix( 1, vtkLORPosition::SIZE, 0.0 );

  verticalMatrix.put( 0, 0, newPosition->GetPositionVector().at( 0 ) );
  verticalMatrix.put( 1, 0, newPosition->GetPositionVector().at( 1 ) );
  verticalMatrix.put( 2, 0, newPosition->GetPositionVector().at( 2 ) );

  horizontalMatrix.put( 0, 0, newPosition->GetPositionVector().at( 0 ) );
  horizontalMatrix.put( 0, 1, newPosition->GetPositionVector().at( 1 ) );
  horizontalMatrix.put( 0, 2, newPosition->GetPositionVector().at( 2 ) );

  this->CurrentOuterProductSum = new vnl_matrix<double>( *( this->CurrentOuterProductSum ) + verticalMatrix * horizontalMatrix );

  this->vtkLORPositionBuffer::AddPosition( newPosition );
}


vnl_matrix<double>* vtkLORRealTimePositionBuffer
::CovarianceMatrix( std::vector<double> centroid )
{
  vnl_matrix<double>* cov = new vnl_matrix<double>( vtkLORPosition::SIZE, vtkLORPosition::SIZE, 0.0 );

  for ( int i = 0; i < vtkLORPosition::SIZE; i++ )
  {
    for ( int j = 0; j < vtkLORPosition::SIZE; j++ )
    {
      cov->put( i, j, this->CurrentOuterProductSum->get( i, j ) / this->Size() - centroid.at( i ) * centroid.at( j ) );
    }
  }

  return cov;
}


std::vector<double> vtkLORRealTimePositionBuffer
::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( vtkLORPosition::SIZE, 0.0 );
  for ( int d = 0; d < vtkLORPosition::SIZE; d++ )
  {
    centroid.at(d) = this->CurrentVectorSum.at(d) / this->Size();
  }

  return centroid;
}