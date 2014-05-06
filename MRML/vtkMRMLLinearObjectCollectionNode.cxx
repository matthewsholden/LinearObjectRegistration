
#include "vtkMRMLLinearObjectCollectionNode.h"

// Constructors and Destructors
// ----------------------------------------------------------------------------

vtkMRMLLinearObjectCollectionNode* vtkMRMLLinearObjectCollectionNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectCollectionNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectCollectionNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectCollectionNode;
}


vtkMRMLLinearObjectCollectionNode
::vtkMRMLLinearObjectCollectionNode()
{
  this->ModelHierarchyNodeID = "";
}


vtkMRMLLinearObjectCollectionNode
::~vtkMRMLLinearObjectCollectionNode()
{
  this->Clear();
}

vtkMRMLNode* vtkMRMLLinearObjectCollectionNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectCollectionNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectCollectionNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectCollectionNode;
}


// Scene: Save and load
// ----------------------------------------------------------------------------


void vtkMRMLLinearObjectCollectionNode::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML(of, nIndent);
  // TODO: Implement this method
}



void vtkMRMLLinearObjectCollectionNode::ReadXMLAttributes( const char** atts )
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

void vtkMRMLLinearObjectCollectionNode::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode );
  vtkMRMLLinearObjectCollectionNode *node = ( vtkMRMLLinearObjectCollectionNode* ) anode;
  
  // Note: It seems that the WriteXML function copies the node then writes the copied node to file
  // So, anything we want in the MRML file we must copy here (I don't think we need to copy other things)
  this->Clear();

  for ( int i = 0; i < node->Size(); i++ )
  {
    if ( node->GetLinearObject( i ) != NULL )
    {
      this->AddLinearObject( node->GetLinearObject( i )->DeepCopy() );
    }
    else
    {
      this->AddLinearObject( NULL );
    }
  }
  this->SetModelHierarchyNodeID( node->GetModelHierarchyNodeID() );

  this->Modified();
}



void vtkMRMLLinearObjectCollectionNode::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent);
  // TODO: Implement this
}




// Linear Object Collection node specific methods
// ----------------------------------------------------------------------------

int vtkMRMLLinearObjectCollectionNode
::Size()
{
  return this->LinearObjects.size();
}


vtkLORLinearObject* vtkMRMLLinearObjectCollectionNode
::GetLinearObject( int index )
{
  if ( index < 0 || index >= this->Size() )
  {
    return NULL;
  }

  return this->LinearObjects.at(index);
}


vtkLORLinearObject* vtkMRMLLinearObjectCollectionNode
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


int vtkMRMLLinearObjectCollectionNode
::GetLinearObjectPosition( vtkLORLinearObject* node )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( node == this->GetLinearObject( i ) )  // Indeed, do pointer comparison - test if they are the same object
	{
      return i;
	}
  }

  return -1;
}


void vtkMRMLLinearObjectCollectionNode
::AddLinearObject( vtkLORLinearObject* newObject )
{
  this->InvokeEvent( LinearObjectAboutToBeAddedEvent, newObject );
  this->LinearObjects.push_back( newObject );
  this->Modified();
}


void vtkMRMLLinearObjectCollectionNode
::Concatenate( vtkMRMLLinearObjectCollectionNode* catBuffer )
{
  for ( int i = 0; i < catBuffer->Size(); i++ )
  {
    this->AddLinearObject( catBuffer->GetLinearObject(i) );
  }
}


void vtkMRMLLinearObjectCollectionNode
::SetLinearObject( int index, vtkLORLinearObject* newObject )
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
  if ( this->GetLinearObject( index ) != NULL )
  {
    this->RemoveLinearObject( index );
  }

  this->InvokeEvent( LinearObjectAboutToBeAddedEvent, newObject );
  this->LinearObjects.at( index ) = newObject;

  this->Modified();
}


void vtkMRMLLinearObjectCollectionNode
::RemoveLinearObject( int index )
{
  // The exact slot is important, so don't change anything else
  if ( index < 0 || index >= this->Size() )
  {
    return;
  }

  this->InvokeEvent( LinearObjectAboutToBeRemovedEvent, this->GetLinearObject( index ) );
  this->LinearObjects.at( index ) = NULL;
  this->Modified();
}


void vtkMRMLLinearObjectCollectionNode
::ShuffleOutNull()
{
  std::vector< vtkSmartPointer< vtkLORLinearObject > > newObjects;

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


void vtkMRMLLinearObjectCollectionNode
::Swap( int index0, int index1 )
{
  // Allow indices bigger than the size - this will just add more objects to the collection
  if ( index0 < 0 || index1 < 0 )
  {
    return;
  }

  // These should be vtkSmartPointers so the underlying objects are not deleted
  vtkSmartPointer< vtkLORLinearObject > linearObject0 = this->GetLinearObject( index0 );
  vtkSmartPointer< vtkLORLinearObject > linearObject1 = this->GetLinearObject( index1 );

  this->SetLinearObject( index0, linearObject1 );
  this->SetLinearObject( index1, linearObject0 );

  this->Modified();
}


void vtkMRMLLinearObjectCollectionNode
::Clear()
{
  this->LinearObjects.clear();
  this->Modified();
}


void vtkMRMLLinearObjectCollectionNode
::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    vtkLORLinearObject* currentLinearObject = this->GetLinearObject( i );

    if ( currentLinearObject == NULL )
    {
      continue;
    }

    this->GetLinearObject(i)->Translate( vector );

    if ( currentLinearObject->GetPositionBuffer() != NULL )
    {
      currentLinearObject->GetPositionBuffer()->Translate( vector );
    }
  }
}


void vtkMRMLLinearObjectCollectionNode
::CalculateSignature( vtkMRMLLinearObjectCollectionNode* refBuffer )
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
vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > vtkMRMLLinearObjectCollectionNode
::GetMatches( vtkMRMLLinearObjectCollectionNode* candidates, double matchingThreshold )
{
  // For each object in this, find the object in candidates that has the closest signature
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > matchedCandidates = vtkSmartPointer< vtkMRMLLinearObjectCollectionNode >::New();
  std::vector< vtkSmartPointer< vtkLORLinearObject > > matchedObjects;

  if ( this->Size() == 0 || candidates->Size() == 0 )
  {
    this->LinearObjects = matchedObjects;
    return matchedCandidates;
  }

  for ( int i = 0; i < this->Size(); i++ )
  {

    vtkLORLinearObject* closestObject = candidates->GetLinearObject(0);
    double closestDistance = LORMath::Distance( this->GetLinearObject(i)->GetSignature(), closestObject->GetSignature() );

    for ( int j = 0; j < candidates->Size(); j++ )
	{
      double currDistance = LORMath::Distance( this->GetLinearObject(i)->GetSignature(), candidates->GetLinearObject(j)->GetSignature() );
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


bool vtkMRMLLinearObjectCollectionNode
::AllHavePositionBuffers()
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( this->GetLinearObject( i ) != NULL && this->GetLinearObject( i )->GetPositionBuffer() == NULL )
    {
      return false;
    }
  }
  return true;
}


std::vector<double> vtkMRMLLinearObjectCollectionNode
::CalculateCentroid()
{
  const double CONDITION_THRESHOLD = 1e-3;

  if ( this->Size() == 0 )
  {
    throw std::logic_error("Centroid calculation is ill-conditioned!");
  }

  // Assume each will take 3 rows. If it doesn't leaving them blank won't affect the result
  vnl_matrix<double>* A = new vnl_matrix<double>( vtkLORLinearObject::DIMENSION * this->Size(), vtkLORLinearObject::DIMENSION, 0.0 );
  vnl_matrix<double>* B = new vnl_matrix<double>( vtkLORLinearObject::DIMENSION * this->Size(), 1, 0.0 );

  // We wish to solve the system A * X = B
  for ( int i = 0; i < this->Size(); i++ )
  {
    int row = vtkLORLinearObject::DIMENSION * i; 
    // A = I for point, B = coordinates
    if ( this->GetLinearObject(i)->GetType().compare( LORConstants::POINT_STRING ) == 0 )
	{
      vtkSmartPointer< vtkLORPoint > PointObject = vtkLORPoint::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, 1.0 ); 
      A->put( row + 1, 1, 1.0 );
	  A->put( row + 2, 2, 1.0 );
	  B->put( row + 0, 0, PointObject->GetBasePoint().at(0) );
	  B->put( row + 1, 0, PointObject->GetBasePoint().at(1) );
	  B->put( row + 2, 0, PointObject->GetBasePoint().at(2) );
	}

	// A = Normal 1, Normal 2, B = Dot( Normal 1, BasePoint ), Dot( Normal 2, BasePoint )
	if ( this->GetLinearObject(i)->GetType().compare( LORConstants::LINE_STRING ) == 0 )
	{
      vtkSmartPointer< vtkLORLine > LineObject = vtkLORLine::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, LineObject->GetOrthogonalNormal1().at(0) );
	  A->put( row + 0, 1, LineObject->GetOrthogonalNormal1().at(1) );
	  A->put( row + 0, 2, LineObject->GetOrthogonalNormal1().at(2) );
      A->put( row + 1, 0, LineObject->GetOrthogonalNormal2().at(0) );
	  A->put( row + 1, 1, LineObject->GetOrthogonalNormal2().at(1) );
	  A->put( row + 1, 2, LineObject->GetOrthogonalNormal2().at(2) );
      B->put( row + 0, 0, LORMath::Dot( LineObject->GetOrthogonalNormal1(), LineObject->GetBasePoint() ) );
      B->put( row + 1, 0, LORMath::Dot( LineObject->GetOrthogonalNormal2(), LineObject->GetBasePoint() ) );
	}

	// A = Normal, B = Dot( Normal, BasePoint )
	if ( this->GetLinearObject(i)->GetType().compare( LORConstants::PLANE_STRING ) == 0 )
	{
      vtkSmartPointer< vtkLORPlane > PlaneObject = vtkLORPlane::SafeDownCast( this->GetLinearObject(i) );
      A->put( row + 0, 0, PlaneObject->GetNormal().at(0) );
	  A->put( row + 0, 1, PlaneObject->GetNormal().at(1) );
	  A->put( row + 0, 2, PlaneObject->GetNormal().at(2) );
      B->put( row + 0, 0, LORMath::Dot( PlaneObject->GetNormal(), PlaneObject->GetBasePoint() ) );
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


std::string vtkMRMLLinearObjectCollectionNode
::GetModelHierarchyNodeID()
{
  return this->ModelHierarchyNodeID;
}


void vtkMRMLLinearObjectCollectionNode
::SetModelHierarchyNodeID( std::string newModelHierarchyNodeID )
{
  this->ModelHierarchyNodeID = newModelHierarchyNodeID;
  this->Modified();
}


std::string vtkMRMLLinearObjectCollectionNode
::ToXMLString()
{
  std::stringstream xmlstring;

  xmlstring << "<LinearObjectCollection>" << std::endl;
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( this->GetLinearObject( i ) != NULL )
    {
      xmlstring << this->GetLinearObject( i )->ToXMLString();
    }
  }
  xmlstring << "</LinearObjectCollection>";

  return xmlstring.str();
}


void vtkMRMLLinearObjectCollectionNode
::FromXMLElement( vtkXMLDataElement* element )
{
  if ( strcmp( element->GetName(), "LinearObjectCollection" ) != 0 )
  {
    return;
  }

  vtkSmartPointer< vtkLORLinearObject > blankObject = NULL;
  this->LinearObjects = std::vector< vtkSmartPointer< vtkLORLinearObject > >( 0, blankObject );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    
	if ( strcmp( noteElement->GetName(), "Reference" ) == 0 )
	{
      vtkSmartPointer< vtkLORReference > newObject = vtkSmartPointer< vtkLORReference >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Point" ) == 0 )
	{
      vtkSmartPointer< vtkLORPoint > newObject = vtkSmartPointer< vtkLORPoint >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
    if ( strcmp( noteElement->GetName(), "Line" ) == 0 )
	{
      vtkSmartPointer< vtkLORLine > newObject = vtkSmartPointer< vtkLORLine >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}
	if ( strcmp( noteElement->GetName(), "Plane" ) == 0 )
	{
      vtkSmartPointer< vtkLORPlane > newObject = vtkSmartPointer< vtkLORPlane >::New();
	  newObject->FromXMLElement( noteElement );
	  this->AddLinearObject( newObject );
	}

  }

  this->Modified();
}