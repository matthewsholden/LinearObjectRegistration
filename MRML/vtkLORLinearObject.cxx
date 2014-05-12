
#include "vtkLORLinearObject.h"


vtkLORLinearObject
::vtkLORLinearObject()
{
  this->Name = "LinearObject";
  this->Type = "LinearObject";
  this->PositionBuffer = NULL;
  this->ModelHierarchyNodeID = "";
}


vtkLORLinearObject
::~vtkLORLinearObject()
{
  this->BasePoint.clear();
  this->Signature.clear();
}


vtkLORPositionBuffer* vtkLORLinearObject
::GetPositionBuffer()
{
  return this->PositionBuffer;
}


void vtkLORLinearObject
::SetPositionBuffer( vtkLORPositionBuffer* newPositionBuffer )
{
  this->PositionBuffer = newPositionBuffer;
  this->Modified();
}


double vtkLORLinearObject
::DistanceToVector( std::vector<double> vector )
{
  return LORMath::Distance( vector, this->ProjectVector( vector ) );
}


void vtkLORLinearObject
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
    vtkSmartPointer< vtkLORPositionBuffer > newPositionBuffer = vtkSmartPointer< vtkLORPositionBuffer >::New();
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


std::string vtkLORLinearObject
::GetName()
{
  return this->Name;
}


void vtkLORLinearObject
::SetName( std::string newName )
{
  this->Name = newName;
  this->Modified();
}


std::string vtkLORLinearObject
::GetType()
{
  return this->Type;
}


void vtkLORLinearObject
::SetType( std::string newType )
{
  this->Type = newType;
  this->Modified();
}


std::string vtkLORLinearObject
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


std::vector<double> vtkLORLinearObject
::GetSignature()
{
  return this->Signature;
}


void vtkLORLinearObject
::SetSignature( std::vector<double> newSignature )
{
  this->Signature = newSignature;
  this->Modified();
}


std::vector<double> vtkLORLinearObject
::GetBasePoint()
{
  return this->BasePoint;
}


void vtkLORLinearObject
::SetBasePoint( std::vector<double> newBasePoint )
{
  this->BasePoint = newBasePoint;
  this->Modified();
}


std::string vtkLORLinearObject
::GetModelHierarchyNodeID()
{
  return this->ModelHierarchyNodeID;
}


void vtkLORLinearObject
::SetModelHierarchyNodeID( std::string newModelHierarchyNodeID )
{
  this->ModelHierarchyNodeID = newModelHierarchyNodeID;
  this->Modified();
}

