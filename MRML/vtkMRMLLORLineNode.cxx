
#include "vtkMRMLLORLineNode.h"

vtkStandardNewMacro( vtkMRMLLORLineNode )


vtkMRMLLORLineNode* vtkMRMLLORLineNode
::New( std::vector<double> newBasePoint, std::vector<double> newEndPoint )
{
  vtkMRMLLORLineNode* newLine = vtkMRMLLORLineNode::New();
  newLine->BasePoint = newBasePoint;
  newLine->EndPoint = newEndPoint;
  return newLine;
}


vtkMRMLLORLineNode
::vtkMRMLLORLineNode()
{
  this->Type = "Line";
}


vtkMRMLLORLineNode
::~vtkMRMLLORLineNode()
{
  this->EndPoint.clear();
}


vtkSmartPointer< vtkMRMLLORLinearObjectNode > vtkMRMLLORLineNode
::DeepCopy()
{
  vtkSmartPointer< vtkMRMLLORLineNode > objectNodeCopy = vtkSmartPointer< vtkMRMLLORLineNode >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
  objectNodeCopy->EndPoint = this->EndPoint;
  objectNodeCopy->Signature = this->Signature;

  objectNodeCopy->SetName( this->GetName() );
  objectNodeCopy->SetType( this->GetType() );

  if ( this->GetPositionBuffer() != NULL )
  {
    objectNodeCopy->SetPositionBuffer( this->GetPositionBuffer()->DeepCopy() );
  }

  return objectNodeCopy;
}


std::vector<double> vtkMRMLLORLineNode
::GetDirection()
{
  std::vector<double> vector = vtkMRMLLORVectorMath::Subtract( this->EndPoint, this->BasePoint );
  return vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( vector ), vector );
}


std::vector<double> vtkMRMLLORLineNode
::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = vtkMRMLLORVectorMath::Subtract( vector, this->BasePoint );
  return vtkMRMLLORVectorMath::Add( vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( this->GetDirection(), outVec ), this->GetDirection() ), this->BasePoint );
}


void vtkMRMLLORLineNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
	this->EndPoint.at(i) = this->EndPoint.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkMRMLLORLineNode
::CreateModelPolyData()
{
  vtkLineSource* lineSource = vtkLineSource::New();

  if ( this->GetPositionBuffer() == NULL )
  {
    lineSource->SetPoint1( this->BasePoint.at(0), this->BasePoint.at(1), this->BasePoint.at(2) );
    lineSource->SetPoint2( this->EndPoint.at(0), this->EndPoint.at(1), this->EndPoint.at(2) );
    lineSource->Update();
    return lineSource->GetOutput();
  }

   // Project point onto line
  // Dot product to find parameterization
  // Use biggest and smallest parameters
  double maxParameter = 0;
  double minParameter = 0;

  for ( int i = 0; i < this->GetPositionBuffer()->Size(); i++ )
  {
    // Project onto line
    std::vector<double> outVec = vtkMRMLLORVectorMath::Subtract( this->GetPositionBuffer()->GetPosition( i )->GetPositionVector(), this->BasePoint );

    double parameter = vtkMRMLLORVectorMath::Dot( this->GetDirection(), outVec );

    if ( parameter > maxParameter )
    {
      maxParameter = parameter;
    }
    if ( parameter < minParameter )
    {
      minParameter = parameter;
    }
  }

  std::vector<double> maxVector = vtkMRMLLORVectorMath::Multiply( maxParameter, this->GetDirection() );
  std::vector<double> minVector = vtkMRMLLORVectorMath::Multiply( minParameter, this->GetDirection() );

  std::vector<double> point1 = vtkMRMLLORVectorMath::Add( this->BasePoint, minVector );
  std::vector<double> point2 = vtkMRMLLORVectorMath::Add( this->BasePoint, maxVector );

  lineSource->SetPoint1( point1.at(0), point1.at(1), point1.at(2) );
  lineSource->SetPoint2( point2.at(0), point2.at(1), point2.at(2) );
  lineSource->Update();
  return lineSource->GetOutput();
}


std::vector<double> vtkMRMLLORLineNode
::GetOrthogonalNormal1()
{
  // Find the two axis unit vectors least parallel with the direction vector
  std::vector<double> e1( 3, 0.0 );
  std::vector<double> e2( 3, 0.0 );
  if ( abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(0) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(0) ) )
  {
    e1.at(0) = 0; e1.at(1) = 1; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(1) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(1) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(2) ) && abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(2) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 1; e2.at(2) = 0;
  }

  std::vector<double> Normal1 = vtkMRMLLORVectorMath::Subtract( e1, vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = vtkMRMLLORVectorMath::Subtract( e2, vtkMRMLLORVectorMath::Add( vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e2, this->GetDirection() ), this->GetDirection() ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( Normal2 ), Normal2 );

  return Normal1;
}


std::vector<double> vtkMRMLLORLineNode
::GetOrthogonalNormal2()
{
  // Find the two axis unit vectors least parallel with the direction vector
  std::vector<double> e1( 3, 0.0 );
  std::vector<double> e2( 3, 0.0 );
  if ( abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(0) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(0) ) )
  {
    e1.at(0) = 0; e1.at(1) = 1; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(1) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(1) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(2) ) && abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(2) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
	e2.at(0) = 0; e2.at(1) = 1; e2.at(2) = 0;
  }

  std::vector<double> Normal1 = vtkMRMLLORVectorMath::Subtract( e1, vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = vtkMRMLLORVectorMath::Subtract( e2, vtkMRMLLORVectorMath::Add( vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e2, this->GetDirection() ), this->GetDirection() ), vtkMRMLLORVectorMath::Multiply( vtkMRMLLORVectorMath::Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = vtkMRMLLORVectorMath::Multiply( 1 / vtkMRMLLORVectorMath::Norm( Normal2 ), Normal2 );

  return Normal2;
}


std::string vtkMRMLLORLineNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Line Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint Value=\"" << vtkMRMLLORVectorMath::VectorToString( this->EndPoint ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Line>" << std::endl;

  return xmlstring.str();
}


void vtkMRMLLORLineNode
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Line" ) != 0 )
  {
    return;  // If it's not a "Line" jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "BasePoint" ) == 0 )
	{
      this->BasePoint = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint" ) == 0 )
	{
      this->EndPoint = vtkMRMLLORVectorMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkMRMLLORLinearObjectNode::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPositionBufferNode > bufferNode = vtkSmartPointer< vtkMRMLLORPositionBufferNode >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}