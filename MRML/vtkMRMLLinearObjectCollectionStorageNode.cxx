/*=Auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTransformStorageNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkMRMLLinearObjectCollectionStorageNode.h"
#include "vtkMRMLLinearObjectCollectionNode.h"


// Standard MRML Node Methods ------------------------------------------------------------

vtkMRMLLinearObjectCollectionStorageNode* vtkMRMLLinearObjectCollectionStorageNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectCollectionStorageNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectCollectionStorageNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectCollectionStorageNode();
}


vtkMRMLNode* vtkMRMLLinearObjectCollectionStorageNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLinearObjectCollectionStorageNode" );
  if( ret )
    {
      return ( vtkMRMLLinearObjectCollectionStorageNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLinearObjectCollectionStorageNode();
}



void vtkMRMLLinearObjectCollectionStorageNode
::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


// Constructors and Destructors --------------------------------------------------------------------

vtkMRMLLinearObjectCollectionStorageNode
::vtkMRMLLinearObjectCollectionStorageNode()
{
}


vtkMRMLLinearObjectCollectionStorageNode
::~vtkMRMLLinearObjectCollectionStorageNode()
{
}

// Storage node specific methods ----------------------------------------------------------------------------
bool vtkMRMLLinearObjectCollectionStorageNode
::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA( "vtkMRMLLinearObjectCollectionNode" );
}


void vtkMRMLLinearObjectCollectionStorageNode
::InitializeSupportedWriteFileTypes()
{
  this->SupportedWriteFileTypes->InsertNextValue("Linear Object Collection (.xml)");
}


const char* vtkMRMLLinearObjectCollectionStorageNode
::GetDefaultWriteFileExtension()
{
  return "xml";
}



// Read and Write methods ----------------------------------------------------------------------------
int vtkMRMLLinearObjectCollectionStorageNode
::ReadDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLLinearObjectCollectionNode* collectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( refNode );

  std::string fullName = this->GetFullNameFromFileName(); 
  if ( fullName == std::string( "" ) ) 
  {
    vtkErrorMacro("vtkMRMLLinearObjectCollectionNode: File name not specified");
    return 0;
  }

  // Clear the current buffer prior to importing
  collectionNode->Clear();

  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fullName.c_str() );
  parser->Parse();

  collectionNode->FromXMLElement( parser->GetRootElement() );

  // The buffer name should already be specified
  // The scene should already be populated with the desired transforms

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLLinearObjectCollectionStorageNode
::WriteDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLLinearObjectCollectionNode* collectionNode = vtkMRMLLinearObjectCollectionNode::SafeDownCast( refNode );

  std::string fullName =  this->GetFullNameFromFileName();
  if ( fullName == std::string( "" ) ) 
  {
    vtkErrorMacro("vtkMRMLLinearObjectCollectionNode: File name not specified");
    return 0;
  }

  std::ofstream output( fullName.c_str() );
  
  if ( ! output.is_open() )
  {
    vtkErrorMacro( "Collection file could not be opened!" );
    return 0;
  }

  output << collectionNode->ToXMLString();

  output.close();

  return 1;
}
