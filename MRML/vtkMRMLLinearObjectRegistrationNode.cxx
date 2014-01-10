
// LinearObjectRegistration MRML includes
#include "vtkMRMLLinearObjectRegistrationNode.h"


// Constants ------------------------------------------------------------------
static const char* COLLECT_TRANSFORM_REFERENCE_ROLE = "CollectTransform";
static const char* FROM_COLLECTION_REFERENCE_ROLE = "FromCollection";
static const char* TO_COLLECTION_REFERENCE_ROLE = "ToCollection";
static const char* OUTPUT_TRANSFORM_REFERENCE_ROLE = "OutputTransform";


// MACROS ---------------------------------------------------------------------

#define DELETE_IF_NOT_NULL(x) \
  if ( x != NULL ) \
    { \
    x->Delete(); \
    x = NULL; \
    }

#define WRITE_STRING_XML(x) \
  if ( this->x != NULL ) \
  { \
    of << indent << " "#x"=\"" << this->x << "\"\n"; \
  }

#define READ_AND_SET_STRING_XML(x) \
    if ( !strcmp( attName, #x ) ) \
      { \
      this->SetAndObserve##x( NULL ); \
      this->Set##x( attValue ); \
      }


// Constructors and Destructors
// ----------------------------------------------------------------------------

vtkMRMLLinearObjectRegistrationNode* vtkMRMLLinearObjectRegistrationNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectRegistrationNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectRegistrationNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectRegistrationNode;
}



vtkMRMLLinearObjectRegistrationNode
::vtkMRMLLinearObjectRegistrationNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  // this->SetModifiedSinceRead( true );

  this->AddNodeReferenceRole( COLLECT_TRANSFORM_REFERENCE_ROLE );
  this->AddNodeReferenceRole( FROM_COLLECTION_REFERENCE_ROLE );
  this->AddNodeReferenceRole( TO_COLLECTION_REFERENCE_ROLE );
  this->AddNodeReferenceRole( OUTPUT_TRANSFORM_REFERENCE_ROLE );
  this->CollectionMode = "ManualDOF";
  this->AutomaticMatch = "True";

  this->Modified();
}



vtkMRMLLinearObjectRegistrationNode
::~vtkMRMLLinearObjectRegistrationNode()
{
}



vtkMRMLNode* vtkMRMLLinearObjectRegistrationNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectRegistrationNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectRegistrationNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectRegistrationNode;
}



// Scene: Save and load
// ----------------------------------------------------------------------------


void vtkMRMLLinearObjectRegistrationNode
::WriteXML( ostream& of, int nIndent )
{

  Superclass::WriteXML(of, nIndent); // This will take care of referenced nodes

  vtkIndent indent(nIndent);
  
  of << indent << " CollectionMode=\"" << this->CollectionMode << "\"";
  of << indent << " AutomaticMatch=\"" << this->AutomaticMatch << "\"";
}



void vtkMRMLLinearObjectRegistrationNode::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes(atts); // This will take care of referenced nodes

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
  {
    attName  = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "CollectionMode" ) )
    {
      this->CollectionMode = std::string( attValue );
    }
    if ( ! strcmp( attName, "AutomaticMath" ) )
    {
      this->AutomaticMatch = std::string( attValue );
    }

  }

  this->Modified();
}



// Slicer Scene
// ----------------------------------------------------------------------------

void vtkMRMLLinearObjectRegistrationNode
::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode ); // This will take care of referenced nodes
  vtkMRMLLinearObjectRegistrationNode *node = ( vtkMRMLLinearObjectRegistrationNode* ) anode;
  
  // Note: It seems that the WriteXML function copies the node then writes the copied node to file
  // So, anything we want in the MRML file we must copy here (I don't think we need to copy other things)
  this->CollectionMode = node->CollectionMode;
  this->AutomaticMatch = node->AutomaticMatch;
  this->Modified();
}



void vtkMRMLLinearObjectRegistrationNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent); // This will take care of referenced nodes
  os << indent << "CollectionMode: " << this->CollectionMode << "\n";
  os << indent << "AutomaticMatch: " << this->AutomaticMatch << "\n";
}


void vtkMRMLLinearObjectRegistrationNode
::ObserveAllReferenceNodes()
{
  this->SetCollectTransformID( this->GetCollectTransformID(), NeverModify );
  this->SetFromCollectionID( this->GetFromCollectionID(), NeverModify );
  this->SetToCollectionID( this->GetToCollectionID(), NeverModify );
  this->SetOutputTransformID( this->GetOutputTransformID(), NeverModify );

  this->UpdateNodeReferences();
  this->Modified();
}


// Variable getters and setters -----------------------------------------------------

std::string vtkMRMLLinearObjectRegistrationNode
::GetCollectTransformID()
{
  return this->GetNodeReferenceIDString( COLLECT_TRANSFORM_REFERENCE_ROLE );
}


void vtkMRMLLinearObjectRegistrationNode
::SetCollectTransformID( std::string newCollectTransformID, int modifyType )
{
  if ( newCollectTransformID.compare( "" ) == 0 )
  {
    this->RemoveAllNodeReferenceIDs( COLLECT_TRANSFORM_REFERENCE_ROLE );
  }
  else if ( this->GetCollectTransformID() != newCollectTransformID )
  {
    this->SetNodeReferenceID( COLLECT_TRANSFORM_REFERENCE_ROLE, newCollectTransformID.c_str() );
  }
  if ( this->GetCollectTransformID() != newCollectTransformID && modifyType == DefaultModify || modifyType == AlwaysModify )
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetFromCollectionID()
{
  return this->GetNodeReferenceIDString( FROM_COLLECTION_REFERENCE_ROLE );
}


void vtkMRMLLinearObjectRegistrationNode
::SetFromCollectionID( std::string newFromCollectionID, int modifyType )
{
  if ( this->GetFromCollectionID() != newFromCollectionID )
  {
    this->SetAndObserveNodeReferenceID( FROM_COLLECTION_REFERENCE_ROLE, newFromCollectionID.c_str() );
  }
  if ( this->GetFromCollectionID() != newFromCollectionID && modifyType == DefaultModify || modifyType == AlwaysModify )
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetToCollectionID()
{
  return this->GetNodeReferenceIDString( TO_COLLECTION_REFERENCE_ROLE );
}


void vtkMRMLLinearObjectRegistrationNode
::SetToCollectionID( std::string newToCollectionID, int modifyType )
{
  if ( this->GetToCollectionID() != newToCollectionID )
  {
    this->SetAndObserveNodeReferenceID( TO_COLLECTION_REFERENCE_ROLE, newToCollectionID.c_str() );
  }
  if ( this->GetToCollectionID() != newToCollectionID && modifyType == DefaultModify || modifyType == AlwaysModify )
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetOutputTransformID()
{
  return this->GetNodeReferenceIDString( OUTPUT_TRANSFORM_REFERENCE_ROLE );
}


void vtkMRMLLinearObjectRegistrationNode
::SetOutputTransformID( std::string newOutputTransformID, int modifyType )
{
  if ( newOutputTransformID.compare( "" ) == 0 )
  {
    this->RemoveAllNodeReferenceIDs( OUTPUT_TRANSFORM_REFERENCE_ROLE );
  }
  else if ( this->GetOutputTransformID() != newOutputTransformID )
  {
    this->SetNodeReferenceID( OUTPUT_TRANSFORM_REFERENCE_ROLE, newOutputTransformID.c_str() );
  }
  if ( this->GetOutputTransformID() != newOutputTransformID && modifyType == DefaultModify || modifyType == AlwaysModify )
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetCollectionMode()
{
  return this->CollectionMode;
}


void vtkMRMLLinearObjectRegistrationNode
::SetCollectionMode( std::string newCollectionMode, int modifyType )
{
  if ( this->GetCollectionMode() != newCollectionMode )
  {
    this->CollectionMode = newCollectionMode;
  }
  if ( this->GetCollectionMode() != newCollectionMode && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetAutomaticMatch()
{
  return this->AutomaticMatch;
}


void vtkMRMLLinearObjectRegistrationNode
::SetAutomaticMatch( std::string newAutomaticMatch, int modifyType )
{
  if ( this->GetAutomaticMatch() != newAutomaticMatch )
  {
    this->AutomaticMatch = newAutomaticMatch;
  }
  if ( this->GetAutomaticMatch() != newAutomaticMatch && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetNodeReferenceIDString( std::string referenceRole )
{
  const char* refID = this->GetNodeReferenceID( referenceRole.c_str() );
  std::string refIDString;

  if ( refID == NULL )
  {
    refIDString = "";
  }
  else
  {
    refIDString = refID;
  }

  return refIDString;
}


void vtkMRMLLinearObjectRegistrationNode
::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  this->Modified(); // This will tell the logic to update
}