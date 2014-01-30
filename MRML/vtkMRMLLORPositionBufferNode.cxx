
#include "vtkMRMLLORPositionBufferNode.h"

vtkStandardNewMacro( vtkMRMLLORPositionBufferNode );


// Return smart pointer since we created the object here but do not hold onto it
vtkSmartPointer< vtkMRMLLORPositionBufferNode > vtkMRMLLORPositionBufferNode
::DeepCopy()
{
  vtkSmartPointer< vtkMRMLLORPositionBufferNode > positionBufferNodeCopy = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();

  for ( int i = 0; i < this->Size(); i++ )
  {
    positionBufferNodeCopy->AddPosition( this->GetPosition( i ) );
  }
  
  return positionBufferNodeCopy;
}


vtkMRMLLORPositionBufferNode
::vtkMRMLLORPositionBufferNode()
{
}


vtkMRMLLORPositionBufferNode
::~vtkMRMLLORPositionBufferNode()
{
  this->Clear();
}


int vtkMRMLLORPositionBufferNode
::Size()
{
  return this->Positions.size();
}


vtkMRMLLORPositionNode* vtkMRMLLORPositionBufferNode
::GetPosition( int index )
{
  return this->Positions.at(index);
}


void vtkMRMLLORPositionBufferNode
::AddPosition( vtkMRMLLORPositionNode* newPosition )
{
  this->Positions.push_back( newPosition );
  this->Modified();
}


void vtkMRMLLORPositionBufferNode
::Clear()
{
  this->Positions.clear();
  this->Modified();
}


void vtkMRMLLORPositionBufferNode
::Trim( int trimSize )
{
  std::vector< vtkSmartPointer< vtkMRMLLORPositionNode > > newPositions;

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


void vtkMRMLLORPositionBufferNode
::Translate( std::vector<double> translation )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetPosition(i)->Translate( translation );
  }
}


std::string vtkMRMLLORPositionBufferNode
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


void vtkMRMLLORPositionBufferNode
::FromXMLElement( vtkXMLDataElement* element )
{
  if ( strcmp( element->GetName(), "Buffer" ) != 0 && strcmp( element->GetName(), "PositionBuffer" ) != 0 )
  {
    return;
  }

  vtkSmartPointer< vtkMRMLLORPositionNode > blankPosition;
  this->Positions = std::vector< vtkSmartPointer< vtkMRMLLORPositionNode > >( 0, blankPosition );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {
    vtkSmartPointer< vtkXMLDataElement > noteElement = element->GetNestedElement( i );

    vtkSmartPointer< vtkMRMLLORPositionNode > newPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
	newPosition->FromXMLElement( noteElement );
    this->AddPosition( newPosition );
  }

}


vnl_matrix<double>* vtkMRMLLORPositionBufferNode
::CovarianceMatrix( std::vector<double> centroid )
{
  // Construct a buffer for the zero mean data; initialize covariance matrix
  vtkSmartPointer< vtkMRMLLORPositionBufferNode > zeroMeanBuffer = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
  vnl_matrix<double> *cov = new vnl_matrix<double>( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE );
  cov->fill( 0.0 );

  // Subtract the mean from each Position
  for ( int i = 0; i < this->Size(); i++ )
  {
    vtkSmartPointer< vtkMRMLLORPositionNode > newPosition = vtkSmartPointer< vtkMRMLLORPositionNode >::New();
    std::vector<double> newPositionVector = vtkMRMLLORVectorMath::Subtract( this->GetPosition(i)->GetPositionVector(), centroid );
    newPosition->SetPositionVector( newPositionVector );
	zeroMeanBuffer->AddPosition( newPosition );
  }

  // Pick two dimensions, and find their covariance
  for ( int d1 = 0; d1 < vtkMRMLLORPositionNode::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkMRMLLORPositionNode::SIZE; d2++ )
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


std::vector<double> vtkMRMLLORPositionBufferNode
::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( vtkMRMLLORPositionNode::SIZE, 0.0 );
  for ( int i = 0; i < this->Size(); i++ )
  {
	  for ( int d = 0; d < vtkMRMLLORPositionNode::SIZE; d++ )
	{
      centroid.at(d) = centroid.at(d) + this->GetPosition(i)->GetPositionVector().at(d);
	}
  }
  for ( int d = 0; d < vtkMRMLLORPositionNode::SIZE; d++ )
  {
    centroid.at(d) = centroid.at(d) / this->Size();
  }

  return centroid;
}


int vtkMRMLLORPositionBufferNode
::GetDOF( double noiseThreshold )
{
  std::vector<double> centroid = this->CalculateCentroid();
  vnl_matrix<double>* cov = this->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( vtkMRMLLORPositionNode::SIZE, vtkMRMLLORPositionNode::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( vtkMRMLLORPositionNode::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Get number of eigenvectors with eigenvalues larger than the threshold
  int calculatedDOF = 0;
  for ( int i = 0; i < vtkMRMLLORPositionNode::SIZE; i++ )
  {
    if ( abs( eigenvalues.get( i ) ) > vtkMRMLLORConstants::NOISE_THRESHOLD )
    {
      calculatedDOF++;
    }
  }

  return calculatedDOF; 
}