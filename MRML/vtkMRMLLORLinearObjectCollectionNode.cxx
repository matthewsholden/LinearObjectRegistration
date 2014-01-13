
#include "vtkMRMLLORLinearObjectCollectionNode.h"

// Constructors and Destructors
// ----------------------------------------------------------------------------

vtkMRMLLORLinearObjectCollectionNode* vtkMRMLLORLinearObjectCollectionNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLORLinearObjectCollectionNode" );
  if( ret )
    {
      return ( vtkMRMLLORLinearObjectCollectionNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLORLinearObjectCollectionNode;
}


vtkMRMLLORLinearObjectCollectionNode
::vtkMRMLLORLinearObjectCollectionNode()
{
}


vtkMRMLLORLinearObjectCollectionNode
::~vtkMRMLLORLinearObjectCollectionNode()
{
  this->Clear();
}

vtkMRMLNode* vtkMRMLLORLinearObjectCollectionNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLORLinearObjectCollectionNode" );
  if( ret )
    {
      return ( vtkMRMLLORLinearObjectCollectionNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLORLinearObjectCollectionNode;
}


// Scene: Save and load
// ----------------------------------------------------------------------------


void vtkMRMLLORLinearObjectCollectionNode::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML(of, nIndent);
  // TODO: Implement this method
}



void vtkMRMLLORLinearObjectCollectionNode::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
  {
    attName  = *(atts++);
    attValue = *(atts++);
    
    // Nothing to implement - this is covered by the linear object collection storage node
  }

  this->Modified();
}



// Slicer Scene
// ----------------------------------------------------------------------------

void vtkMRMLLORLinearObjectCollectionNode::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode );
  vtkMRMLLORLinearObjectCollectionNode *node = ( vtkMRMLLORLinearObjectCollectionNode* ) anode;
  
  // Note: It seems that the WriteXML function copies the node then writes the copied node to file
  // So, anything we want in the MRML file we must copy here (I don't think we need to copy other things)
  this->Clear();

  for ( int i = 0; i < node->Size(); i++ )
  {
    this->AddLinearObject( node->GetLinearObject( i ) );
  }

  this->Modified();
}



void vtkMRMLLORLinearObjectCollectionNode::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent);
  // TODO: Implement this
}




// Linear Object Collection node specific methods
// ----------------------------------------------------------------------------

int vtkMRMLLORLinearObjectCollectionNode
::Size()
{
  return this->LinearObjects.size();
}


vtkMRMLLORLinearObjectNode* vtkMRMLLORLinearObjectCollectionNode
::GetLinearObject( int index )
{
  if ( index < 0 || index >= this->Size() )
  {
    return NULL;
  }

  return this->LinearObjects.at(index);
}


vtkMRMLLORLinearObjectNode* vtkMRMLLORLinearObjectCollectionNode
::GetLinearObject( std::string name )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( strcmp( this->GetLinearObject(i)->GetName().c_str(), name.c_str() ) == 0 )
	{
      return this->GetLinearObject(i);
	}
  }

  return NULL;
}


void vtkMRMLLORLinearObjectCollectionNode
::AddLinearObject( vtkMRMLLORLinearObjectNode* newObject )
{
  this->LinearObjects.push_back( newObject );
  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::Concatenate( vtkMRMLLORLinearObjectCollectionNode* catBuffer )
{
  for ( int i = 0; i < catBuffer->Size(); i++ )
  {
    this->AddLinearObject( catBuffer->GetLinearObject(i) );
  }
}


void vtkMRMLLORLinearObjectCollectionNode
::SetLinearObject( int index, vtkMRMLLORLinearObjectNode* newObject )
{
  if ( index < 0 )
  {
    return;
  }
  // Add null linear object until we are the appropriate size
  while ( this->Size() <= index )
  {
    this->AddLinearObject( NULL );
  }
  this->LinearObjects.at( index ) = newObject;
  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::RemoveLinearObject( int index )
{
  // The exact slot is important, so don't change anything else
  if ( index <= this->Size() )
  {
    this->SetLinearObject( index, NULL );
  }
  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::ShuffleOutNull()
{
  std::vector< vtkSmartPointer< vtkMRMLLORLinearObjectNode > > newObjects;

  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( this->GetLinearObject( i ) != NULL )
    {
      newObjects.push_back( this->GetLinearObject( i ) );
    }
  }

  this->LinearObjects = newObjects;
  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::Swap( int index0, int index1 )
{
  // Allow indices bigger than the size - this will just add more objects to the collection
  if ( index0 < 0 || index1 < 0 )
  {
    return;
  }

  vtkMRMLLORLinearObjectNode* linearObject0 = this->GetLinearObject( index0 );
  vtkMRMLLORLinearObjectNode* linearObject1 = this->GetLinearObject( index1 );

  this->SetLinearObject( index0, linearObject1 );
  this->SetLinearObject( index1, linearObject0 );

  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::Clear()
{
  this->LinearObjects.clear();
  this->Modified();
}


void vtkMRMLLORLinearObjectCollectionNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetLinearObject(i)->Translate( vector );
  }
}


void vtkMRMLLORLinearObjectCollectionNode
::CalculateSignature( vtkMRMLLORLinearObjectCollectionNode* refBuffer )
{
  // Calculate the signature of everything in this, assume the inputted object is a buffer of references
  for ( int i = 0; i < this->Size(); i++ )
  {
    // Do not move this method to the LinearObject class, since LinearObjects should not know about LinearObjectBuffers
    std::vector<double> newSignature( refBuffer->Size(), 0.0 );
    for ( int j = 0; j < refBuffer->Size(); j++ )
    {
      newSignature.at(j) = this->GetLinearObject(i)->DistanceToVector( refBuffer->GetLinearObject(j)->GetBasePoint() );
    }
	this->GetLinearObject(i)->SetSignature( newSignature );
  }

}


// TODO: This function may be should be moved to the logic
vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > vtkMRMLLORLinearObjectCollectionNode
::GetMatches( vtkMRMLLORLinearObjectCollectionNode* candidates, double matchingThreshold )
{
  // For each object in this, find the object in candidates that has the closest signature
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > matchedCandidates = vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode >::New();
  std::vector< vtkSmartPointer< vtkMRMLLORLinearObjectNode > > matchedObjects;

  if ( this->Size() == 0 || candidates->Size() == 0 )
  {
    this->LinearObjects = matchedObjects;
    return matchedCandidates;
  }

  for ( int i = 0; i < this->Size(); i++ )
  {

    vtkMRMLLORLinearObjectNode* closestObject = candidates->GetLinearObject(0);
    double closestDistance = vtkMRMLLORVectorMath::Distance( this->GetLinearObject(i)->GetSignature(), closestObject->GetSignature() );

    for ( int j = 0; j < candidates->Size(); j++ )
	{
      double currDistance = vtkMRMLLORVectorMath::Distance( this->GetLinearObject(i)->GetSignature(), candidates->GetLinearObject(j)->GetSignature() );
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
  this->LinearObjects = matchedObjects;
  this->Modified();

  return matchedCandidates;
}


std::vector<double> vtkMRMLLORLinearObjectCollectionNode
::CalculateCentroid()
{
  const double CONDITION_THRESHOLD = 1e-3;

  if ( this->Size() == 0 )
  {
    throw std::logic_error("Failed - centroid calculation is ill-conditioned!");
  }

  // Assume each will take 3 rows. If it doesn't leaving them blank won't affect the result
  vnl_matrix<double>* A = new vnl_matrix<double>( vtkMRMLLORLinearObjectNode::DIMENSION * this->Size(), vtkMRMLLORLinearObjectNode::DIMENSION, 0.0 );
  vnl_matrix<double>* B = new vnl_matrix<double>( vtkMRMLLORLinearObjectNode::DIMENSION * this->Size(), 1, 0.0 );

  // We wish to solve the system A * X = B
  for ( int i = 0; i < this->Size(); i++ )
  {
    int row = vtkMRMLLORLinearObjectNode::DIMENSION * i; 
    // A = I for point, B = coordinates
    if ( strcmp( this->GetLinearObject(i)->GetType().c_str(), "Point" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPointNode > PointObject = vtkMRMLLORPointNode::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, 1.0 ); 
      A->put( row + 1, 1, 1.0 );
	  A->put( row + 2, 2, 1.0 );
	  B->put( row + 0, 0, PointObject->GetBasePoint().at(0) );
	  B->put( row + 1, 0, PointObject->GetBasePoint().at(1) );
	  B->put( row + 2, 0, PointObject->GetBasePoint().at(2) );
	}

	// A = Normal 1, Normal 2, B = Dot( Normal 1, BasePoint ), Dot( Normal 2, BasePoint )
	if ( strcmp( this->GetLinearObject(i)->GetType().c_str(), "Line" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORLineNode > LineObject = vtkMRMLLORLineNode::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, LineObject->GetOrthogonalNormal1().at(0) );
	  A->put( row + 0, 1, LineObject->GetOrthogonalNormal1().at(1) );
	  A->put( row + 0, 2, LineObject->GetOrthogonalNormal1().at(2) );
      A->put( row + 1, 0, LineObject->GetOrthogonalNormal2().at(0) );
	  A->put( row + 1, 1, LineObject->GetOrthogonalNormal2().at(1) );
	  A->put( row + 1, 2, LineObject->GetOrthogonalNormal2().at(2) );
      B->put( row + 0, 0, vtkMRMLLORVectorMath::Dot( LineObject->GetOrthogonalNormal1(), LineObject->GetBasePoint() ) );
      B->put( row + 1, 0, vtkMRMLLORVectorMath::Dot( LineObject->GetOrthogonalNormal2(), LineObject->GetBasePoint() ) );
	}

	// A = Normal, B = Dot( Normal, BasePoint )
	if ( strcmp( this->GetLinearObject(i)->GetType().c_str(), "Plane" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPlaneNode > PlaneObject = vtkMRMLLORPlaneNode::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, PlaneObject->GetNormal().at(0) );
	  A->put( row + 0, 1, PlaneObject->GetNormal().at(1) );
	  A->put( row + 0, 2, PlaneObject->GetNormal().at(2) );
      B->put( row + 0, 0, vtkMRMLLORVectorMath::Dot( PlaneObject->GetNormal(), PlaneObject->GetBasePoint() ) );
	}

  }

  // Now, calculate X
  vnl_matrix_inverse<double>* X = new vnl_matrix_inverse<double>( A->transpose() * (*A) );
  if ( X->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error("Failed - centroid calculation is ill-conditioned!");
  }
  vnl_matrix<double>* Y = new vnl_matrix<double>( X->inverse() * A->transpose() * (*B) );

  std::vector<double> centroid( vtkMRMLLORLinearObjectNode::DIMENSION, 0.0 );
  centroid.at(0) = Y->get( 0, 0 );
  centroid.at(1) = Y->get( 1, 0 );
  centroid.at(2) = Y->get( 2, 0 );

  return centroid;
}


std::string vtkMRMLLORLinearObjectCollectionNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "<LinearObjectCollection>" << std::endl;
  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetLinearObject( i )->ToXMLString();
  }
  xmlstring << "</LinearObjectCollection>";

  return xmlstring.str();
}


void vtkMRMLLORLinearObjectCollectionNode
::FromXMLElement( vtkXMLDataElement* element )
{
  vtkSmartPointer< vtkMRMLLORLinearObjectNode > blankObject = NULL;
  this->LinearObjects = std::vector< vtkSmartPointer< vtkMRMLLORLinearObjectNode > >( 0, blankObject );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "Reference" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORReferenceNode > newObject = vtkSmartPointer< vtkMRMLLORReferenceNode >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Point" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPointNode > newObject = vtkSmartPointer< vtkMRMLLORPointNode >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Line" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORLineNode > newObject = vtkSmartPointer< vtkMRMLLORLineNode >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Plane" ) == 0 )
	{
      vtkSmartPointer< vtkMRMLLORPlaneNode > newObject = vtkSmartPointer< vtkMRMLLORPlaneNode >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}

  }

  this->Modified();
}