
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
#include "vtkXMLDataElement.h"

#include "vtkMRMLNode.h"

// VNL includes
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"

// LinearObjectRegistration includes
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkMRMLLORReferenceNode.h"
#include "vtkMRMLLORPointNode.h"
#include "vtkMRMLLORLineNode.h"
#include "vtkMRMLLORPlaneNode.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORLinearObjectCollectionNode : public vtkMRMLNode
{
public:
  vtkTypeMacro( vtkMRMLLORLinearObjectCollectionNode, vtkMRMLNode );
  
  // Standard MRML node methods  
  static vtkMRMLLORLinearObjectCollectionNode *New();  

  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "LinearObjectCollection"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );
  
protected:

  // Constructor/desctructor methods
  vtkMRMLLORLinearObjectCollectionNode();
  virtual ~vtkMRMLLORLinearObjectCollectionNode();
  vtkMRMLLORLinearObjectCollectionNode ( const vtkMRMLLORLinearObjectCollectionNode& );
  void operator=( const vtkMRMLLORLinearObjectCollectionNode& );

private:
	std::vector< vtkSmartPointer< vtkMRMLLORLinearObjectNode > > objects;

public:

  // Basic collection functionality
  int Size();
  vtkSmartPointer< vtkMRMLLORLinearObjectNode > GetLinearObject( int index );
  vtkSmartPointer< vtkMRMLLORLinearObjectNode > GetLinearObject( std::string name );
  void AddLinearObject( vtkSmartPointer< vtkMRMLLORLinearObjectNode > newObject );
  void Concatenate( vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > catBuffer );
  void SetLinearObject( int index, vtkSmartPointer< vtkMRMLLORLinearObjectNode > newObject );
  void RemoveLinearObject( int index );
  void ShuffleOutNull();
  void Swap( int index0, int index1 );
  void Clear();

  void Translate( std::vector<double> vector );

  void CalculateSignature( vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > refBuffer );

  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > GetMatches( vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > candidates, double matchingThreshold );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );

};

#endif