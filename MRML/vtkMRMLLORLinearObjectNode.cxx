
#include "vtkMRMLLORLinearObjectNode.h"


vtkMRMLLORLinearObjectNode
::vtkMRMLLORLinearObjectNode()
{
  this->Name = "";
  this->Type = "LinearObject";
}


vtkMRMLLORLinearObjectNode
::~vtkMRMLLORLinearObjectNode()
{
  this->BasePoint.clear();
  this->Signature.clear();
}


vtkMRMLLORPositionBufferNode* vtkMRMLLORLinearObjectNode
::GetPositionBuffer()
{
  return this->PositionBuffer;
}


void vtkMRMLLORLinearObjectNode
::SetPositionBuffer( vtkMRMLLORPositionBufferNode* newPositionBuffer )
{
  this->PositionBuffer = newPositionBuffer;
}


double vtkMRMLLORLinearObjectNode
::DistanceToVector( std::vector<double> vector )
{
  return vtkMRMLLORVectorMath::Distance( vector, this->ProjectVector( vector ) );
}


void vtkMRMLLORLinearObjectNode
::Filter( int filterWidth )
{
  const int THRESHOLD = 1e-3; // Deal with the case of very little noise
  bool changed = true;

  while ( changed )
  {
    std::vector<double> distances( this->GetPositionBuffer()->Size(), 0 );
    double meanDistance = 0;
    double stdev = 0;

    // Calculate the distance of each point to the linear object
    for ( int i = 0; i < this->GetPositionBuffer()->Size(); i++ )
    {
      distances.at(i) = this->DistanceToVector( this->GetPositionBuffer()->GetPosition( i )->Position ); // The position attribute is a vector
      meanDistance = meanDistance + distances.at(i);
      stdev = stdev + distances.at(i) * distances.at(i);
    }
    
    meanDistance = meanDistance / this->GetPositionBuffer()->Size();
    stdev = stdev / this->GetPositionBuffer()->Size();
    stdev = sqrt( stdev - meanDistance * meanDistance );
    
    // Keep only the points that are within certain number of standard deviations
    vtkMRMLLORPositionBufferNode* newPositionBuffer = vtkMRMLLORPositionBufferNode::New();
    for ( int i = 0; i < this->GetPositionBuffer()->Size(); i++ )
    {
      if ( distances.at(i) < filterWidth * stdev || distances.at(i) < THRESHOLD )
      {
        newPositionBuffer->AddPosition( this->GetPositionBuffer()->GetPosition( i ) );
      }
    }

    if ( newPositionBuffer->Size() < this->GetPositionBuffer()->Size() )
    {
      changed = true;
    }
    else
    {
      changed = false;
    }

    this->SetPositionBuffer( newPositionBuffer );
  }

}