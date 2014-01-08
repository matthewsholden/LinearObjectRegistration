
#include "vtkMRMLLORPlaneNode.h"

vtkStandardNewMacro( vtkMRMLLORPlaneNode );


vtkMRMLLORPlaneNode* vtkMRMLLORPlaneNode
::New( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 )
{
  vtkMRMLLORPlaneNode* newPlane = vtkMRMLLORPlaneNode::New();
  newPlane->BasePoint = newBasePoint;
  newPlane->EndPoint1 = newEndPoint1;
  newPlane->EndPoint2 = newEndPoint2;
  return newPlane;
}


vtkMRMLLORPlaneNode
::vtkMRMLLORPlaneNode()
{
  this->Type = "Plane";
}


vtkMRMLLORPlaneNode
::~vtkMRMLLORPlaneNode()
{
  this->EndPoint1.clear();
  this->EndPoint2.clear();
}


std::vector<double> vtkMRMLLORPlaneNode
::GetNormal()
{
  std::vector<double> vector = vtkMRMLLORVectorMath::Cross( vtkMRMLLORVectorMath::Subtract( this->EndPoint1, this->BasePoint ), vtkMRMLLORVectorMath::Subtract( this->EndPoint2, this->BasePoint ) );
  vector = vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( vector ), vector );
  return vector;
}


std::vector<double> vtkMRMLLORPlaneNode
::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = vtkMRMLLORVectorMath::Subtract( vector, this->BasePoint );
  return vtkMRMLLORVectorMath::Subtract( vector, vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( this->GetNormal(), outVec ), this->GetNormal() ) );
}


void vtkMRMLLORPlaneNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
	this->EndPoint1.at(i) = this->EndPoint1.at(i) + vector.at(i);
	this->EndPoint2.at(i) = this->EndPoint2.at(i) + vector.at(i);
  }
}


std::string vtkMRMLLORPlaneNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Plane";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"";
  xmlstring << " EndPoint1=\"" << vtkMRMLLORVectorMath::VectorToString( this->EndPoint1 ) << "\"";
  xmlstring << " EndPoint2=\"" << vtkMRMLLORVectorMath::VectorToString( this->EndPoint2 ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORPlaneNode
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{

  if ( strcmp( element->GetName(), "Plane" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );
  this->EndPoint1 = vtkMRMLLORVectorMath::StringToVector( std::string( element->GetAttribute( "EndPoint1" ) ), 3 );
  this->EndPoint2 = vtkMRMLLORVectorMath::StringToVector( std::string( element->GetAttribute( "EndPoint2" ) ), 3 );

}