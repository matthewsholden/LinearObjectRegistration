#ifndef __vtkLORPoint_h
#define __vtkLORPoint_h

#include "vtkLORLinearObject.h"

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
vtkLORPoint : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORPoint, vtkObject );

  static vtkLORPoint* New();
  static vtkLORPoint* New( std::vector<double> newBasePoint );

protected:

  // Constructor/desstructor
  vtkLORPoint();
  virtual ~vtkLORPoint();


public:

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );

};

#endif