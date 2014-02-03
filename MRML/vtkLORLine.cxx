
#include "vtkLORLine.h"

vtkStandardNewMacro( vtkLORLine )


vtkLORLine* vtkLORLine
::New( std::vector<double> newBasePoint, std::vector<double> newEndPoint )
{
  vtkLORLine* newLine = vtkLORLine::New();
  newLine->BasePoint = newBasePoint;
  newLine->EndPoint = newEndPoint;
  return newLine;
}


vtkLORLine
::vtkLORLine()
{
  this->Type = "Line";
}


vtkLORLine
::~vtkLORLine()
{
  this->EndPoint.clear();
}


vtkSmartPointer< vtkLORLinearObject > vtkLORLine
::DeepCopy()
{
  vtkSmartPointer< vtkLORLine > objectNodeCopy = vtkSmartPointer< vtkLORLine >::New();

  objectNodeCopy->BasePoint = this->BasePoint;
  objectNodeCopy->EndPoint = this->EndPoint;
  objectNodeCopy->Signature = this->Signature;

  objectNodeCopy->SetName( this->GetName() );
  objectNodeCopy->SetType( this->GetType() );
  objectNodeCopy->SetModelHierarchyNodeID( this->GetModelHierarchyNodeID() );

  if ( this->GetPositionBuffer() != NULL )
  {
    objectNodeCopy->SetPositionBuffer( this->GetPositionBuffer()->DeepCopy() );
  }

  return objectNodeCopy;
}


std::vector<double> vtkLORLine
::GetDirection()
{
  std::vector<double> vector = LORMath::Subtract( this->EndPoint, this->BasePoint );
  return LORMath::Multiply( 1 / LORMath::Norm( vector ), vector );
}


std::vector<double> vtkLORLine
::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = LORMath::Subtract( vector, this->BasePoint );
  return LORMath::Add( LORMath::Multiply( LORMath::Dot( this->GetDirection(), outVec ), this->GetDirection() ), this->BasePoint );
}


void vtkLORLine
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
	this->EndPoint.at(i) = this->EndPoint.at(i) + vector.at(i);
  }
}


vtkPolyData* vtkLORLine
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
    std::vector<double> outVec = LORMath::Subtract( this->GetPositionBuffer()->GetPosition( i )->GetPositionVector(), this->BasePoint );

    double parameter = LORMath::Dot( this->GetDirection(), outVec );

    if ( parameter > maxParameter )
    {
      maxParameter = parameter;
    }
    if ( parameter < minParameter )
    {
      minParameter = parameter;
    }
  }

  std::vector<double> maxVector = LORMath::Multiply( maxParameter, this->GetDirection() );
  std::vector<double> minVector = LORMath::Multiply( minParameter, this->GetDirection() );

  std::vector<double> point1 = LORMath::Add( this->BasePoint, minVector );
  std::vector<double> point2 = LORMath::Add( this->BasePoint, maxVector );

  lineSource->SetPoint1( point1.at(0), point1.at(1), point1.at(2) );
  lineSource->SetPoint2( point2.at(0), point2.at(1), point2.at(2) );
  lineSource->Update();
  return lineSource->GetOutput();
}


std::vector<double> vtkLORLine
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

  std::vector<double> Normal1 = LORMath::Subtract( e1, LORMath::Multiply( LORMath::Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = LORMath::Multiply( 1 / LORMath::Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = LORMath::Subtract( e2, LORMath::Add( LORMath::Multiply( LORMath::Dot( e2, this->GetDirection() ), this->GetDirection() ), LORMath::Multiply( LORMath::Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = LORMath::Multiply( 1 / LORMath::Norm( Normal2 ), Normal2 );

  return Normal1;
}


std::vector<double> vtkLORLine
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

  std::vector<double> Normal1 = LORMath::Subtract( e1, LORMath::Multiply( LORMath::Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = LORMath::Multiply( 1 / LORMath::Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = LORMath::Subtract( e2, LORMath::Add( LORMath::Multiply( LORMath::Dot( e2, this->GetDirection() ), this->GetDirection() ), LORMath::Multiply( LORMath::Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = LORMath::Multiply( 1 / LORMath::Norm( Normal2 ), Normal2 );

  return Normal2;
}


std::string vtkLORLine
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "  <Line Name=\"" << this->Name << "\">" << std::endl;
  xmlstring << "    <BasePoint Value=\"" << LORMath::VectorToString( this->BasePoint ) << "\"/>" << std::endl;
  xmlstring << "    <EndPoint Value=\"" << LORMath::VectorToString( this->EndPoint ) << "\"/>" << std::endl;

  if ( this->GetPositionBuffer() != NULL )
  {
    xmlstring << this->GetPositionBuffer()->ToXMLString();
  }

  xmlstring << "  </Line>" << std::endl;

  return xmlstring.str();
}


void vtkLORLine
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
      this->BasePoint = LORMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkLORLinearObject::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "EndPoint" ) == 0 )
	{
      this->EndPoint = LORMath::StringToVector( std::string( noteElement->GetAttribute( "Value" ) ), vtkLORLinearObject::DIMENSION );
	}
	if ( strcmp( noteElement->GetName(), "Buffer" ) == 0 )
	{
      vtkSmartPointer< vtkLORPositionBuffer > bufferNode = vtkSmartPointer< vtkLORPositionBuffer >::New();
	  bufferNode->FromXMLElement( noteElement );
      this->SetPositionBuffer( bufferNode );
	}

  }

}