#ifndef __vtkMRMLLORPointNode_h
#define __vtkMRMLLORPointNode_h

#include "vtkMRMLLORLinearObjectNode.h"

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

// LinearObjectRegistration includes
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORPointNode : public vtkMRMLLORLinearObjectNode
{
public:
  vtkTypeMacro( vtkMRMLLORPointNode, vtkObject );

  static vtkMRMLLORPointNode* New();
  static vtkMRMLLORPointNode* New( std::vector<double> newBasePoint );

protected:

  // Constructor/desstructor
  vtkMRMLLORPointNode();
  virtual ~vtkMRMLLORPointNode();


public:

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif