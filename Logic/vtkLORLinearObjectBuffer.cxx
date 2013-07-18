
#include "vtkLORLinearObjectBuffer.h"

vtkStandardNewMacro( vtkLORLinearObjectBuffer );


vtkLORLinearObjectBuffer
::vtkLORLinearObjectBuffer()
{
}


vtkLORLinearObjectBuffer
::~vtkLORLinearObjectBuffer()
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->objects.at(i)->Delete();
  }
  this->objects.clear();
}


int vtkLORLinearObjectBuffer
::Size()
{
  return this->objects.size();
}


vtkLORLinearObject* vtkLORLinearObjectBuffer
::GetLinearObject( int index )
{
  return this->objects.at(index);
}


vtkLORLinearObject* vtkLORLinearObjectBuffer
::GetLinearObject( std::string name )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( strcmp( this->GetLinearObject(i)->Name.c_str(), name.c_str() ) == 0 )
	{
      return this->GetLinearObject(i);
	}
  }

  return NULL;
}


void vtkLORLinearObjectBuffer
::AddLinearObject( vtkLORLinearObject* newObject )
{
  this->objects.push_back( newObject );
}


void vtkLORLinearObjectBuffer
::Concatenate( vtkLORLinearObjectBuffer* catBuffer )
{
  for ( int i = 0; i < catBuffer->Size(); i++ )
  {
    this->AddLinearObject( catBuffer->GetLinearObject(i) );
  }
}


void vtkLORLinearObjectBuffer
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetLinearObject(i)->Translate( vector );
  }
}


void vtkLORLinearObjectBuffer
::CalculateSignature( vtkLORLinearObjectBuffer* refBuffer )
{
  // Calculate the signature of everything in this, assume the inputted object is a buffer of references
  for ( int i = 0; i < this->Size(); i++ )
  {
    // Do not move this method to the LinearObject class, since LinearObjects should not know about LinearObjectBuffers
    std::vector<double> sig( refBuffer->Size(), 0.0 );
    for ( int j = 0; j < refBuffer->Size(); j++ )
    {
      sig.at(j) = this->GetLinearObject(i)->DistanceToVector( refBuffer->GetLinearObject(j)->BasePoint );
    }
	this->GetLinearObject(i)->Signature = sig;
  }

}


vtkLORLinearObjectBuffer* vtkLORLinearObjectBuffer
::GetMatches( vtkLORLinearObjectBuffer* candidates, double matchingThreshold )
{
  // For each object in this, find the object in candidates that has the closest signature
  vtkLORLinearObjectBuffer* matchedCandidates = new vtkLORLinearObjectBuffer();
  std::vector<vtkLORLinearObject*> matchedObjects;

  if ( this->Size() == 0 || candidates->Size() == 0 )
  {
    this->objects = matchedObjects;
    return matchedCandidates;
  }

  for ( int i = 0; i < this->Size(); i++ )
  {

    vtkLORLinearObject* closestObject = candidates->GetLinearObject(0);
	double closestDistance = Distance( this->GetLinearObject(i)->Signature, closestObject->Signature );

    for ( int j = 0; j < candidates->Size(); j++ )
	{
      double currDistance = Distance( this->GetLinearObject(i)->Signature, candidates->GetLinearObject(j)->Signature );
	  if ( currDistance < closestDistance )
	  {
	    closestObject = candidates->GetLinearObject(j);
		closestDistance = currDistance;
	  }
	}

	// Only accept the matching if it is sufficiently good (this throws away potentially wrongly identified collected objects)
	if ( closestDistance < matchingThreshold )
	{
	  matchedObjects.push_back( this->GetLinearObject(i) );
	  matchedCandidates->AddLinearObject( closestObject );
	}

  }

  // Note that this modifies both this buffer and creates a new candidate buffer
  this->objects = matchedObjects; 
  return matchedCandidates;
}


std::vector<double> vtkLORLinearObjectBuffer
::CalculateCentroid()
{
  const double CONDITION_THRESHOLD = 1e-3;

  // Assume each will take 3 rows. If it doesn't leaving them blank won't affect the result
  vnl_matrix<double>* A = new vnl_matrix<double>( vtkLORLinearObject::DIMENSION * this->Size(), vtkLORLinearObject::DIMENSION, 0.0 );
  vnl_matrix<double>* B = new vnl_matrix<double>( vtkLORLinearObject::DIMENSION * this->Size(), 1, 0.0 );

  // We wish to solve the system A * X = B
  for ( int i = 0; i < this->Size(); i++ )
  {
    int row = vtkLORLinearObject::DIMENSION * i; 
    // A = I for point, B = coordinates
    if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Point" ) == 0 )
	{
	  vtkLORPoint* PointObject = (vtkLORPoint*) this->GetLinearObject(i);
      A->put( row + 0, 0, 1.0 ); 
      A->put( row + 1, 1, 1.0 );
	  A->put( row + 2, 2, 1.0 );
	  B->put( row + 0, 0, PointObject->BasePoint.at(0) );
	  B->put( row + 1, 0, PointObject->BasePoint.at(1) );
	  B->put( row + 2, 0, PointObject->BasePoint.at(2) );
	}

	// A = Normal 1, Normal 2, B = Dot( Normal 1, BasePoint ), Dot( Normal 2, BasePoint )
	if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Line" ) == 0 )
	{
	  vtkLORLine* LineObject = (vtkLORLine*) this->GetLinearObject(i);
      A->put( row + 0, 0, LineObject->GetOrthogonalNormal1().at(0) );
	  A->put( row + 0, 1, LineObject->GetOrthogonalNormal1().at(1) );
	  A->put( row + 0, 2, LineObject->GetOrthogonalNormal1().at(2) );
      A->put( row + 1, 0, LineObject->GetOrthogonalNormal2().at(0) );
	  A->put( row + 1, 1, LineObject->GetOrthogonalNormal2().at(1) );
	  A->put( row + 1, 2, LineObject->GetOrthogonalNormal2().at(2) );
	  B->put( row + 0, 0, Dot( LineObject->GetOrthogonalNormal1(), LineObject->BasePoint ) );
	  B->put( row + 1, 0, Dot( LineObject->GetOrthogonalNormal2(), LineObject->BasePoint ) );
	}

	// A = Normal, B = Dot( Normal, BasePoint )
	if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Plane" ) == 0 )
	{
	  vtkLORPlane* PlaneObject = (vtkLORPlane*) this->GetLinearObject(i);
      A->put( row + 0, 0, PlaneObject->GetNormal().at(0) );
	  A->put( row + 0, 1, PlaneObject->GetNormal().at(1) );
	  A->put( row + 0, 2, PlaneObject->GetNormal().at(2) );
	  B->put( row + 0, 0, Dot( PlaneObject->GetNormal(), PlaneObject->BasePoint ) );
	}

  }

  // Now, calculate X
  vnl_matrix_inverse<double>* X = new vnl_matrix_inverse<double>( A->transpose() * (*A) );
  if ( X->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error("Failed - centroid calculation is ill-conditioned!");
  }
  vnl_matrix<double>* Y = new vnl_matrix<double>( X->inverse() * A->transpose() * (*B) );

  std::vector<double> centroid( vtkLORLinearObject::DIMENSION, 0.0 );
  centroid.at(0) = Y->get( 0, 0 );
  centroid.at(1) = Y->get( 1, 0 );
  centroid.at(2) = Y->get( 2, 0 );

  return centroid;
}


std::string vtkLORLinearObjectBuffer
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "<Geometry>" << std::endl;
  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetLinearObject(i)->ToXMLString();
  }
  xmlstring << "</Geometry>";

  return xmlstring.str();
}


void vtkLORLinearObjectBuffer
::FromXMLElement( vtkXMLDataElement* element )
{
  vtkLORLinearObject* blankObject = NULL;
  this->objects = std::vector<vtkLORLinearObject*>( 0, blankObject );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "Reference" ) == 0 )
	{
      vtkLORReference* newObject = vtkLORReference::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Point" ) == 0 )
	{
      vtkLORPoint* newObject = vtkLORPoint::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Line" ) == 0 )
	{
      vtkLORLine* newObject = vtkLORLine::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Plane" ) == 0 )
	{
      vtkLORPlane* newObject = vtkLORPlane::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}

  }

}