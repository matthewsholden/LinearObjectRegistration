
#include "vtkMRMLLORPositionBufferNode.h"

vtkStandardNewMacro( vtkMRMLLORPositionBufferNode );


vtkMRMLLORPositionBufferNode* vtkMRMLLORPositionBufferNode
::DeepCopy()
{
  vtkMRMLLORPositionBufferNode* positionBufferNodeCopy = vtkMRMLLORPositionBufferNode::New();

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


vtkSmartPointer< vtkMRMLLORPositionNode > vtkMRMLLORPositionBufferNode
::GetPosition( int index )
{
  return this->Positions.at(index);
}


void vtkMRMLLORPositionBufferNode
::AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode > newPosition )
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

  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetPosition(i)->ToXMLString();
  }

  return xmlstring.str();
}


void vtkMRMLLORPositionBufferNode
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{
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
    std::vector<double> newPositionVector( vtkMRMLLORPositionNode::SIZE, 0.0 );
    for( int d = 0; d < vtkMRMLLORPositionNode::SIZE; d++ )
	{
	  newPositionVector.push_back( this->GetPosition(i)->GetPositionVector().at(d) - centroid.at(d) );
	}
    newPosition->SetPositionVector( newPositionVector );
	zeroMeanBuffer->AddPosition( newPosition );
  }

  // Pick two dimensions, and find their covariance
  for ( int d1 = 0; d1 < vtkMRMLLORPositionNode::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkMRMLLORPositionNode::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < this->Size(); i++ )
	  {
	    cov->put( d1, d2, cov->get( d1, d2 ) + zeroMeanBuffer->GetPosition(i)->GetPositionVector().at(d1) * zeroMeanBuffer->GetPosition(i)->GetPositionVector().at(d2) );
	  }
	  // Divide by the number of records
	  cov->put( d1, d2, cov->get( d1, d2 ) / this->Size() );
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