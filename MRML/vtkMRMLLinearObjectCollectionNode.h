
#ifndef vtkMRMLLinearObjectCollectionNode_h
#define vtkMRMLLinearObjectCollectionNode_h

// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// VTK includes
#include "vtkSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataParser.h"

#include "vtkMRMLNode.h"
#include "vtkMRMLStorableNode.h"

// VNL includes
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"

// LinearObjectRegistration includes
#include "vtkMRMLLinearObjectCollectionStorageNode.h"
#include "vtkLORLinearObject.h"
#include "vtkLORReference.h"
#include "vtkLORPoint.h"
#include "vtkLORLine.h"
#include "vtkLORPlane.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLinearObjectCollectionNode : public vtkMRMLStorableNode
{
public:
  vtkTypeMacro( vtkMRMLLinearObjectCollectionNode, vtkMRMLStorableNode );
  
  // Standard MRML node methods  
  static vtkMRMLLinearObjectCollectionNode *New();  

  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "LinearObjectCollection"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );
  
  // To use the storage node
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode() { return vtkMRMLLinearObjectCollectionStorageNode::New(); };
  bool GetModifiedSinceRead() { return ( this->GetMTime() > this->GetStoredTime() ); };
  virtual void UpdateScene( vtkMRMLScene *scene ) { Superclass::UpdateScene(scene); };
  
protected:

  // Constructor/desctructor methods
  vtkMRMLLinearObjectCollectionNode();
  virtual ~vtkMRMLLinearObjectCollectionNode();
  vtkMRMLLinearObjectCollectionNode ( const vtkMRMLLinearObjectCollectionNode& );
  void operator=( const vtkMRMLLinearObjectCollectionNode& );

public:

  // Basic collection functionality
  int Size();
  vtkLORLinearObject* GetLinearObject( int index );
  vtkLORLinearObject* GetLinearObject( std::string name );
  int GetLinearObjectPosition( vtkLORLinearObject* node );

  void AddLinearObject( vtkLORLinearObject* newObject );
  void Concatenate( vtkMRMLLinearObjectCollectionNode* catBuffer );
  void SetLinearObject( int index, vtkLORLinearObject* newObject );
  void RemoveLinearObject( int index );
  void ShuffleOutNull();
  void Swap( int index0, int index1 );
  void Clear();

  void Translate( std::vector<double> vector );

  void CalculateSignature( vtkMRMLLinearObjectCollectionNode* refBuffer );

  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > GetMatches( vtkMRMLLinearObjectCollectionNode* candidates, double matchingThreshold );

  bool AllHavePositionBuffers();

  std::vector<double> CalculateCentroid();

  std::string GetModelHierarchyNodeID();
  void SetModelHierarchyNodeID( std::string newModelHierarchyNodeID );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );


private:
	std::vector< vtkSmartPointer< vtkLORLinearObject > > LinearObjects;

    std::string ModelHierarchyNodeID;

};

#endif