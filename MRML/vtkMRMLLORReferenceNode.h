
#ifndef __vtkMRMLLORReferenceNode_h
#define __vtkMRMLLORReferenceNode_h

// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// VTK includes
#include "vtkObject.h"
#include "vtkObjectBase.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkPointSource.h"

// LinearObjectRegistration includes
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"



// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORReferenceNode : public vtkMRMLLORLinearObjectNode
{
public:
  vtkTypeMacro( vtkMRMLLORReferenceNode, vtkObject );

  static vtkMRMLLORReferenceNode* New();
  static vtkMRMLLORReferenceNode* New( std::vector<double> newBasePoint );

  vtkSmartPointer< vtkMRMLLORLinearObjectNode > DeepCopy();

protected:

  // Constructor/destructor
  vtkMRMLLORReferenceNode();
  virtual ~vtkMRMLLORReferenceNode();

public:

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  vtkPolyData* CreateModelPolyData();

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif