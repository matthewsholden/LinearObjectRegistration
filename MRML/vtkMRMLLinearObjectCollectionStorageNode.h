/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLLinearObjectCollectionStorageNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef __vtkMRMLLinearObjectCollectionStorageNode_h
#define __vtkMRMLLinearObjectCollectionStorageNode_h

// Standard includes
#include <ctime>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

//VTK includes
#include "vtkMRMLStorageNode.h"
#include "vtkStringArray.h"

// TransformRecorder includes
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


/// Storage nodes has methods to read/write transform bufferss to/from disk.
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLinearObjectCollectionStorageNode : public vtkMRMLStorageNode
{
public:
  vtkTypeMacro( vtkMRMLLinearObjectCollectionStorageNode, vtkMRMLStorageNode );

  // Standard MRML node methods  
  static vtkMRMLLinearObjectCollectionStorageNode* New();
  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName()  { return "LORLinearObjectCollectionStorage"; };
  void PrintSelf(ostream& os, vtkIndent indent);
  // No need for special read/write/copy

  // Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes();
  // Return a default file extension for writing
  virtual const char* GetDefaultWriteFileExtension();

  /// Support only linear object registration nodes
  virtual bool CanReadInReferenceNode(vtkMRMLNode* refNode);

protected:
  // Constructor/deconstructor
  vtkMRMLLinearObjectCollectionStorageNode();
  ~vtkMRMLLinearObjectCollectionStorageNode();
  vtkMRMLLinearObjectCollectionStorageNode(const vtkMRMLLinearObjectCollectionStorageNode&);
  void operator=(const vtkMRMLLinearObjectCollectionStorageNode&);


  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode *refNode);

  /// Write data from a referenced node
  virtual int WriteDataInternal(vtkMRMLNode *refNode);

};

#endif