
#include "vtkMRMLLORRealTimePositionBufferNode.h"

vtkStandardNewMacro( vtkMRMLLORRealTimePositionBufferNode );


vtkMRMLLORRealTimePositionBufferNode
::vtkMRMLLORRealTimePositionBufferNode()
{
  this->CurrentOuterProductSum = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE, 0.0 );
  this->CurrentVectorSum = std::vector<double>( vtkMRMLLORPositionNode::SIZE, 0.0 );
}


vtkMRMLLORRealTimePositionBufferNode
::~vtkMRMLLORRealTimePositionBufferNode()
{
  this->Clear();
}


void vtkMRMLLORRealTimePositionBufferNode
::AddPosition( vtkMRMLLORPositionNode* newPosition )
{
  this->CurrentVectorSum = vtkMRMLLORVectorMath::Add( this->CurrentVectorSum, newPosition->GetPositionVector() );

  // Find the outer product matrix and add
  vnl_matrix<double> verticalMatrix( vtkMRMLLORPositionNode::SIZE, 1, 0.0 );
  vnl_matrix<double> horizontalMatrix( 1, vtkMRMLLORPositionNode::SIZE, 0.0 );

  verticalMatrix.put( 0, 0, newPosition->GetPositionVector().at( 0 ) );
  verticalMatrix.put( 1, 0, newPosition->GetPositionVector().at( 1 ) );
  verticalMatrix.put( 2, 0, newPosition->GetPositionVector().at( 2 ) );

  horizontalMatrix.put( 0, 0, newPosition->GetPositionVector().at( 0 ) );
  horizontalMatrix.put( 0, 1, newPosition->GetPositionVector().at( 1 ) );
  horizontalMatrix.put( 0, 2, newPosition->GetPositionVector().at( 2 ) );

  this->CurrentOuterProductSum = new vnl_matrix<double>( *( this->CurrentOuterProductSum ) + verticalMatrix * horizontalMatrix );

  this->vtkMRMLLORPositionBufferNode::AddPosition( newPosition );
}


vnl_matrix<double>* vtkMRMLLORRealTimePositionBufferNode
::CovarianceMatrix( std::vector<double> centroid )
{
  vnl_matrix<double>* cov = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE, 0.0 );

  for ( int i = 0; i < vtkMRMLLORPositionNode::SIZE; i++ )
  {
    for ( int j = 0; j < vtkMRMLLORPositionNode::SIZE; j++ )
    {
      cov->put( i, j, this->CurrentOuterProductSum->get( i, j ) / this->Size() - centroid.at( i ) * centroid.at( j ) );
    }
  }

  return cov;
}


std::vector<double> vtkMRMLLORRealTimePositionBufferNode
::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( vtkMRMLLORPositionNode::SIZE, 0.0 );
  for ( int d = 0; d < vtkMRMLLORPositionNode::SIZE; d++ )
  {
    centroid.at(d) = this->CurrentVectorSum.at(d) / this->Size();
  }

  return centroid;
}