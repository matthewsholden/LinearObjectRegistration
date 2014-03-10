
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
#include "vtkPointSource.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"



// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkLORReference : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORReference, vtkObject );

  static vtkLORReference* New();
  static vtkLORReference* New( std::vector<double> newBasePoint );

  vtkSmartPointer< vtkLORLinearObject > DeepCopy();

protected:

  // Constructor/destructor
  vtkLORReference();
  virtual ~vtkLORReference();

public:

  // Implement inherited abstract methods
  bool IsCoincident( vtkLORLinearObject *testLinearObject, double threshold );

  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  vtkPolyData* CreateModelPolyData();

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif