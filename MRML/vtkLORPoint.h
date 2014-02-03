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
#include "vtkPointSource.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkLORPoint : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORPoint, vtkObject );

  static vtkLORPoint* New();
  static vtkLORPoint* New( std::vector<double> newBasePoint );

  vtkSmartPointer< vtkLORLinearObject > DeepCopy();

protected:

  // Constructor/desstructor
  vtkLORPoint();
  virtual ~vtkLORPoint();


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