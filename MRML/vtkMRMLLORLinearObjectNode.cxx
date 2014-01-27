
#include "vtkMRMLLORLinearObjectNode.h"


vtkMRMLLORLinearObjectNode
::vtkMRMLLORLinearObjectNode()
{
  this->Name = "Unnamed";
  this->Type = "LinearObject";
  this->PositionBuffer = NULL;
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
  this->Modified();
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
      distances.at(i) = this->DistanceToVector( this->GetPositionBuffer()->GetPosition( i )->GetPositionVector() ); // The position attribute is a vector
      meanDistance = meanDistance + distances.at(i);
      stdev = stdev + distances.at(i) * distances.at(i);
    }
    
    meanDistance = meanDistance / this->GetPositionBuffer()->Size();
    stdev = stdev / this->GetPositionBuffer()->Size();
    stdev = sqrt( stdev - meanDistance * meanDistance );
    
    // Keep only the points that are within certain number of standard deviations
    vtkSmartPointer< vtkMRMLLORPositionBufferNode > newPositionBuffer = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
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


std::string vtkMRMLLORLinearObjectNode
::GetName()
{
  return this->Name;
}


void vtkMRMLLORLinearObjectNode
::SetName( std::string newName )
{
  this->Name = newName;
  this->Modified();
}


std::string vtkMRMLLORLinearObjectNode
::GetType()
{
  return this->Type;
}


void vtkMRMLLORLinearObjectNode
::SetType( std::string newType )
{
  this->Type = newType;
  this->Modified();
}


std::string vtkMRMLLORLinearObjectNode
::GetPositionBufferString()
{
  if ( this->GetPositionBuffer() == NULL )
  {
    return "None";
  }

  std::stringstream positionBufferString;
  positionBufferString << this->GetPositionBuffer()->Size();
  positionBufferString << " Positions";
  return positionBufferString.str();
}


std::vector<double> vtkMRMLLORLinearObjectNode
::GetSignature()
{
  return this->Signature;
}


void vtkMRMLLORLinearObjectNode
::SetSignature( std::vector<double> newSignature )
{
  this->Signature = newSignature;
  this->Modified();
}


std::vector<double> vtkMRMLLORLinearObjectNode
::GetBasePoint()
{
  return this->BasePoint;
}


void vtkMRMLLORLinearObjectNode
::SetBasePoint( std::vector<double> newBasePoint )
{
  this->BasePoint = newBasePoint;
  this->Modified();
}


std::string vtkMRMLLORLinearObjectNode
::GetModelID()
{
  return this->ModelID;
}


void vtkMRMLLORLinearObjectNode
::SetModelID( std::string newModelID )
{
  this->ModelID = newModelID;
  this->Modified();
}

