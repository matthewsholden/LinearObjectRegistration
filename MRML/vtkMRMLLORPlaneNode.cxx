
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


vtkSmartPointer< vtkMRMLLORLinearObjectNode > vtkMRMLLORPlaneNode
::DeepCopy()
{
  vtkSmartPointer< vtkMRMLLORPlaneNode > objectNodeCopy = vtkSmartPointer< vtkMRMLLORPlaneNode >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
  objectNodeCopy->EndPoint1 = this->EndPoint1;
  objectNodeCopy->EndPoint2 = this->EndPoint2;
  objectNodeCopy->Signature = this->Signature;

  objectNodeCopy->SetName( this->GetName() );
  objectNodeCopy->SetType( this->GetType() );

  if ( this->GetPositionBuffer() != NULL )
  {
    objectNodeCopy->SetPositionBuffer( this->GetPositionBuffer()->DeepCopy() );
  }

  return objectNodeCopy;
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


vtkPolyData* vtkMRMLLORPlaneNode
::CreateModelPolyData()
{
  vtkPlaneSource* planeSource = vtkPlaneSource::New();

  if ( this->GetPositionBuffer() == NULL )
  {
    planeSource->SetOrigin( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
    planeSource->SetPoint1( this->EndPoint1.at(0), this->EndPoint1.at(1), this->EndPoint1.at(2) );
    planeSource->SetPoint2( this->EndPoint2.at(0), this->EndPoint2.at(1), this->EndPoint2.at(2) );
    planeSource->Update();
    return planeSource->GetOutput();
  }

  // Project point onto line
  // Dot product to find parameterization
  // Use biggest and smallest parameters
  double maxParameter1 = 0;
  double minParameter1 = 0;
  double maxParameter2 = 0;
  double minParameter2 = 0;
  std::vector<double> directionVector1 = vtkMRMLLORVectorMath::Normalize( vtkMRMLLORVectorMath::Subtract( this->EndPoint1, this->BasePoint ) );
  std::vector<double> directionVector2 = vtkMRMLLORVectorMath::Normalize( vtkMRMLLORVectorMath::Subtract( this->EndPoint2, this->BasePoint ) );

  for ( int i = 0; i < this->GetPositionBuffer()->Size(); i++ )
  {
    // Project onto line
    std::vector<double> outVec = vtkMRMLLORVectorMath::Subtract( this->GetPositionBuffer()->GetPosition( i )->GetPositionVector(), this->BasePoint );

    double parameter1 = vtkMRMLLORVectorMath::Dot( directionVector1, outVec );
    double parameter2 = vtkMRMLLORVectorMath::Dot( directionVector2, outVec );

    if ( parameter1 > maxParameter1 )
    {
      maxParameter1 = parameter1;
    }
    if ( parameter1 < minParameter1 )
    {
      minParameter1 = parameter1;
    }
    if ( parameter2 > maxParameter2 )
    {
      maxParameter2 = parameter2;
    }
    if ( parameter2 < minParameter2 )
    {
      minParameter2 = parameter2;
    }
  }

  std::vector<double> maxVector1 = vtkMRMLLORVectorMath::Multiply( maxParameter1, directionVector1 );
  std::vector<double> minVector1 = vtkMRMLLORVectorMath::Multiply( minParameter1, directionVector1 );
  std::vector<double> maxVector2 = vtkMRMLLORVectorMath::Multiply( maxParameter2, directionVector2 );
  std::vector<double> minVector2 = vtkMRMLLORVectorMath::Multiply( minParameter2, directionVector2 );

  std::vector<double> origin = vtkMRMLLORVectorMath::Add( this->BasePoint, vtkMRMLLORVectorMath::Add( minVector1, minVector2 ) );
  std::vector<double> point1 = vtkMRMLLORVectorMath::Add( origin, vtkMRMLLORVectorMath::Subtract( maxVector1, minVector1 ) );
  std::vector<double> point2 = vtkMRMLLORVectorMath::Add( origin, vtkMRMLLORVectorMath::Subtract( maxVector2, minVector2 ) );

  planeSource->SetOrigin( origin.at(0), origin.at(1), origin.at(2) );
  planeSource->SetPoint1( point1.at(0), point1.at(1), point1.at(2) );
  planeSource->SetPoint2( point2.at(0), point2.at(1), point2.at(2) );
  planeSource->Update();
  return planeSource->GetOutput();
}


std::string vtkMRMLLORPlaneNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Plane Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint1 Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->EndPoint1 ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint2 Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->EndPoint2 ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Plane>" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORPlaneNode
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Plane" ) != 0 )
  {
    return;  // If it's not a "Plane" jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkSmartPointer< vtkXMLDataElement > noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "BasePoint" ) == 0 )
	{
      this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint1" ) == 0 )
	{
      this->EndPoint1 = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint2" ) == 0 )
	{
      this->EndPoint2 = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPositionBufferNode > bufferNode = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}