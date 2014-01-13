
#ifndef vtkMRMLLORLinearObjectCollectionNode_h
#define vtkMRMLLORLinearObjectCollectionNode_h

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
#include "vtkMRMLLORLinearObjectCollectionStorageNode.h"
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkMRMLLORReferenceNode.h"
#include "vtkMRMLLORPointNode.h"
#include "vtkMRMLLORLineNode.h"
#include "vtkMRMLLORPlaneNode.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORLinearObjectCollectionNode : public vtkMRMLStorableNode
{
public:
  vtkTypeMacro( vtkMRMLLORLinearObjectCollectionNode, vtkMRMLStorableNode );
  
  // Standard MRML node methods  
  static vtkMRMLLORLinearObjectCollectionNode *New();  

  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "LinearObjectCollection"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );
  
  // To use the storage node
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode() { return vtkMRMLLORLinearObjectCollectionStorageNode::New(); };
  bool GetModifiedSinceRead() { return ( this->GetMTime() > this->GetStoredTime() ); };
  virtual void UpdateScene( vtkMRMLScene *scene ) { Superclass::UpdateScene(scene); };
  
protected:

  // Constructor/desctructor methods
  vtkMRMLLORLinearObjectCollectionNode();
  virtual ~vtkMRMLLORLinearObjectCollectionNode();
  vtkMRMLLORLinearObjectCollectionNode ( const vtkMRMLLORLinearObjectCollectionNode& );
  void operator=( const vtkMRMLLORLinearObjectCollectionNode& );

public:

  // Basic collection functionality
  int Size();
  vtkMRMLLORLinearObjectNode* GetLinearObject( int index );
  vtkMRMLLORLinearObjectNode* GetLinearObject( std::string name );
  void AddLinearObject( vtkMRMLLORLinearObjectNode* newObject );
  void Concatenate( vtkMRMLLORLinearObjectCollectionNode* catBuffer );
  void SetLinearObject( int index, vtkMRMLLORLinearObjectNode* newObject );
  void RemoveLinearObject( int index );
  void ShuffleOutNull();
  void Swap( int index0, int index1 );
  void Clear();

  void Translate( std::vector<double> vector );

  void CalculateSignature( vtkMRMLLORLinearObjectCollectionNode* refBuffer );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > GetMatches( vtkMRMLLORLinearObjectCollectionNode* candidates, double matchingThreshold );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );


private:
	std::vector< vtkSmartPointer< vtkMRMLLORLinearObjectNode > > LinearObjects;

};

#endif