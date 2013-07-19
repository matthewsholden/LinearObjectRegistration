
#ifndef __vtkLORReference_h
#define __vtkLORReference_h

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
#include "vtkLORLinearObject.h"
#include "vtkSlicerLinearObjectRegistrationModuleLogicExport.h"



// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkLORReference : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORReference, vtkObject );

  static vtkLORReference* New();
  static vtkLORReference* New( std::vector<double> newBasePoint );

protected:

  // Constructor/destructor
  vtkLORReference();
  virtual ~vtkLORReference();

public:

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );

};

#endif