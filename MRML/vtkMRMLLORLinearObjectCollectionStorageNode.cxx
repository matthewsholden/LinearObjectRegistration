/*=Auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTransformStorageNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkMRMLLORLinearObjectCollectionStorageNode.h"
#include "vtkMRMLLORLinearObjectCollectionNode.h"


// Standard MRML Node Methods ------------------------------------------------------------

vtkMRMLLORLinearObjectCollectionStorageNode* vtkMRMLLORLinearObjectCollectionStorageNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLORLinearObjectCollectionStorageNode" );
  if( ret )
    {
      return ( vtkMRMLLORLinearObjectCollectionStorageNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLORLinearObjectCollectionStorageNode();
}


vtkMRMLNode* vtkMRMLLORLinearObjectCollectionStorageNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLORLinearObjectCollectionStorageNode" );
  if( ret )
    {
      return ( vtkMRMLLORLinearObjectCollectionStorageNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLLORLinearObjectCollectionStorageNode();
}



void vtkMRMLLORLinearObjectCollectionStorageNode
::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


// Constructors and Destructors --------------------------------------------------------------------

vtkMRMLLORLinearObjectCollectionStorageNode
::vtkMRMLLORLinearObjectCollectionStorageNode()
{
}


vtkMRMLLORLinearObjectCollectionStorageNode
::~vtkMRMLLORLinearObjectCollectionStorageNode()
{
}

// Storage node specific methods ----------------------------------------------------------------------------
bool vtkMRMLLORLinearObjectCollectionStorageNode
::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA( "vtkMRMLLORLinearObjectCollectionNode" );
}


void vtkMRMLLORLinearObjectCollectionStorageNode
::InitializeSupportedWriteFileTypes()
{
  this->SupportedWriteFileTypes->InsertNextValue("Linear Object Collection (.xml)");
}


const char* vtkMRMLLORLinearObjectCollectionStorageNode
::GetDefaultWriteFileExtension()
{
  return "xml";
}



// Read and Write methods ----------------------------------------------------------------------------
int vtkMRMLLORLinearObjectCollectionStorageNode
::ReadDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLLORLinearObjectCollectionNode* collectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( refNode );

  std::string fullName = this->GetFullNameFromFileName(); 
  if ( fullName == std::string( "" ) ) 
  {
    vtkErrorMacro("vtkMRMLLORLinearObjectCollectionNode: File name not specified");
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
int vtkMRMLLORLinearObjectCollectionStorageNode
::WriteDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLLORLinearObjectCollectionNode* collectionNode = vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( refNode );

  std::string fullName =  this->GetFullNameFromFileName();
  if ( fullName == std::string( "" ) ) 
  {
    vtkErrorMacro("vtkMRMLLORLinearObjectCollectionNode: File name not specified");
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
