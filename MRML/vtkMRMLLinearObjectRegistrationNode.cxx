
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

  this->CollectionState = "";
  this->ActivePositionBuffer = vtkSmartPointer< vtkLORRealTimePositionBuffer >::New();

  //TODO: Should these be zero or "default" values
  this->NoiseThreshold = 0.5;
  this->MatchingThreshold = 10.0;
  this->MinimumCollectionPositions = 100;
  this->TrimPositions = 10;

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
  of << indent << "NoiseThreshold=\"" << this->NoiseThreshold << "\"";
  of << indent << "MatchingThreshold=\"" << this->MatchingThreshold << "\"";
  of << indent << "MinimumCollectionPositions=\"" << this->MinimumCollectionPositions << "\"";
  of << indent << "TrimPositions=\"" << this->TrimPositions << "\"";
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
    if ( ! strcmp( attName, "NoiseThreshold" ) )
    {
      this->NoiseThreshold = atof( attValue );
    }
    if ( ! strcmp( attName, "MatchingThreshold" ) )
    {
      this->MatchingThreshold = atof( attValue );
    }
    if ( ! strcmp( attName, "MinimumCollectionPositions" ) )
    {
      this->MinimumCollectionPositions = atoi( attValue );
    }
    if ( ! strcmp( attName, "TrimPositions" ) )
    {
      this->TrimPositions = atoi( attValue );
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
  this->NoiseThreshold = node->NoiseThreshold;
  this->MatchingThreshold = node->MatchingThreshold;
  this->MinimumCollectionPositions = node->MinimumCollectionPositions;
  this->TrimPositions = node->TrimPositions;
  this->Modified();
}



void vtkMRMLLinearObjectRegistrationNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent); // This will take care of referenced nodes
  os << indent << "CollectionMode: " << this->CollectionMode << "\n";
  os << indent << "AutomaticMatch: " << this->AutomaticMatch << "\n";
  os << indent << "NoiseThreshold: " << this->NoiseThreshold << "\n";
  os << indent << "MatchingThreshold: " << this->MatchingThreshold << "\n";
  os << indent << "MinimumCollectionPositions: " << this->MinimumCollectionPositions << "\n";
  os << indent << "TrimPositions: " << this->TrimPositions << "\n";
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
  if ( this->GetCollectTransformID().compare( newCollectTransformID ) != 0 )
  {
    vtkNew<vtkIntArray> events;
    events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
    this->SetAndObserveNodeReferenceID( COLLECT_TRANSFORM_REFERENCE_ROLE, newCollectTransformID.c_str(), events.GetPointer() );
  }
  if ( this->GetCollectTransformID().compare( newCollectTransformID ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify )
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
  if ( this->GetFromCollectionID().compare( newFromCollectionID ) != 0  )
  {
    this->SetAndObserveNodeReferenceID( FROM_COLLECTION_REFERENCE_ROLE, newFromCollectionID.c_str() );
  }
  if ( this->GetFromCollectionID().compare( newFromCollectionID ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify )
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
  if ( this->GetToCollectionID().compare( newToCollectionID ) != 0 )
  {
    this->SetAndObserveNodeReferenceID( TO_COLLECTION_REFERENCE_ROLE, newToCollectionID.c_str() );
  }
  if ( this->GetToCollectionID().compare( newToCollectionID ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify )
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
  else if ( this->GetOutputTransformID().compare( newOutputTransformID ) != 0 )
  {
    this->SetNodeReferenceID( OUTPUT_TRANSFORM_REFERENCE_ROLE, newOutputTransformID.c_str() );
  }
  if ( this->GetOutputTransformID().compare( newOutputTransformID ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify )
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
  if ( this->GetCollectionMode().compare( newCollectionMode ) != 0 )
  {
    this->CollectionMode = newCollectionMode;
  }
  if ( this->GetCollectionMode().compare( newCollectionMode ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
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
  if ( this->GetAutomaticMatch().compare( newAutomaticMatch ) != 0 )
  {
    this->AutomaticMatch = newAutomaticMatch;
  }
  if ( this->GetAutomaticMatch().compare( newAutomaticMatch ) != 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


double vtkMRMLLinearObjectRegistrationNode
::GetNoiseThreshold()
{
  return this->NoiseThreshold;
}


void vtkMRMLLinearObjectRegistrationNode
::SetNoiseThreshold( double newNoiseThreshold, int modifyType )
{
  if ( this->GetNoiseThreshold() != newNoiseThreshold && newNoiseThreshold > 0 )
  {
    this->NoiseThreshold = newNoiseThreshold;
  }
  if ( this->GetNoiseThreshold() != newNoiseThreshold && newNoiseThreshold > 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


double vtkMRMLLinearObjectRegistrationNode
::GetMatchingThreshold()
{
  return this->MatchingThreshold;
}


void vtkMRMLLinearObjectRegistrationNode
::SetMatchingThreshold( double newMatchingThreshold, int modifyType )
{
  if ( this->GetMatchingThreshold() != newMatchingThreshold && newMatchingThreshold > 0 )
  {
    this->MatchingThreshold = newMatchingThreshold;
  }
  if ( this->GetMatchingThreshold() != newMatchingThreshold && newMatchingThreshold > 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


int vtkMRMLLinearObjectRegistrationNode
::GetMinimumCollectionPositions()
{
  return this->MinimumCollectionPositions;
}


void vtkMRMLLinearObjectRegistrationNode
::SetMinimumCollectionPositions( int newMinimumCollectionPositions, int modifyType )
{
  if ( this->GetMinimumCollectionPositions() != newMinimumCollectionPositions && newMinimumCollectionPositions > 0 )
  {
    this->MinimumCollectionPositions = newMinimumCollectionPositions;
  }
  if ( this->GetMinimumCollectionPositions() != newMinimumCollectionPositions && newMinimumCollectionPositions > 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
  {
    this->Modified();
  }
}


int vtkMRMLLinearObjectRegistrationNode
::GetTrimPositions()
{
  return this->TrimPositions;
}


void vtkMRMLLinearObjectRegistrationNode
::SetTrimPositions( int newTrimPositions, int modifyType )
{
  if ( this->GetTrimPositions() != newTrimPositions && newTrimPositions > 0 )
  {
    this->TrimPositions = newTrimPositions;
  }
  if ( this->GetTrimPositions() != newTrimPositions && newTrimPositions > 0 && modifyType == DefaultModify || modifyType == AlwaysModify ) 
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


// These methods are related to collecting data
// ----------------------------------------------------------------------------------------------------------


vtkLORPositionBuffer* vtkMRMLLinearObjectRegistrationNode
::GetActivePositionBuffer()
{
  return this->ActivePositionBuffer;
}


std::string vtkMRMLLinearObjectRegistrationNode
::GetCollectionState()
{
  return this->CollectionState;
}



void vtkMRMLLinearObjectRegistrationNode
::StartCollecting( std::string newCollectionState )
{
  this->GetActivePositionBuffer()->Clear();

  // Don't collect if the collect transform is not specified
  if ( this->GetNodeReferenceIDString( COLLECT_TRANSFORM_REFERENCE_ROLE ).compare( "" ) != 0 )
  {
    this->CollectionState = newCollectionState;
  }
}


void vtkMRMLLinearObjectRegistrationNode
::StopCollecting()
{
  // Emit an event indicating that the position buffer is ready to be converted to a linear object
  if ( this->CollectionState.compare( "Automatic" ) != 0 || this->GetActivePositionBuffer()->Size() > this->GetMinimumCollectionPositions() )
  {
    this->InvokeEvent( PositionBufferReady );
  }

  this->GetActivePositionBuffer()->Clear(); // TODO: Should the clearing be done in the logic which catches the event
  this->CollectionState = "";
}


void vtkMRMLLinearObjectRegistrationNode
::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  // In case the observed transform node is updated
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( caller );

  // If it wasn't the observed transform
  if ( transformNode == NULL || this->GetNodeReferenceIDString( COLLECT_TRANSFORM_REFERENCE_ROLE ).compare( transformNode->GetID() ) != 0 || event != vtkMRMLLinearTransformNode::TransformModifiedEvent )
  {
    this->Modified(); // Recalculate the transform
    return;
  }
  if ( this->CollectionState.compare( "" ) == 0 )
  {
    return;
  }

  vtkLORPosition* tempPosition = vtkLORPosition::New( transformNode->GetMatrixTransformToParent() );
  vtkSmartPointer< vtkLORPosition > newPosition = vtkSmartPointer< vtkLORPosition >::Take( tempPosition );

  this->GetActivePositionBuffer()->AddPosition( newPosition );

  if ( this->CollectionState.compare( "Automatic" ) != 0 )
  {
    return;
  }

  // Check if the position buffer is still linear
  int positionBufferDOF = this->GetActivePositionBuffer()->GetDOF( this->GetNoiseThreshold() );

  // If it is, then keep going
  if ( positionBufferDOF <= LORConstants::PLANE_DOF )
  {
    return;
  }

  // If it is not, then emit a ready event if we have the required number of collected frames
  if ( this->GetActivePositionBuffer()->Size() > this->GetMinimumCollectionPositions() )
  {
    this->InvokeEvent( PositionBufferReady );
    this->Modified(); // TODO: For some reason, the collection node modified event is never caught when it is called from the processmrmlevents method
  }
  this->GetActivePositionBuffer()->Clear(); // TODO: Should the clearing be done in the logic which catches the event

}
