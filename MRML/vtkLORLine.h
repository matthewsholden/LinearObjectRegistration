
#ifndef __vtkLORLine_h
#define __vtkLORLine_h


// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// VTK includes
#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkObjectBase.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkLineSource.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"

// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkLORLine : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORLine, vtkObject );

  static vtkLORLine* New();
  static vtkLORLine* New( std::vector<double> newBasePoint, std::vector<double> newEndPoint );

  vtkSmartPointer< vtkLORLinearObject > DeepCopy();

protected:

  // Constructor/destructor
  vtkLORLine();
  virtual ~vtkLORLine();


public:

  std::vector<double> GetDirection();

  // Implement inherited abstract methods
  bool IsCoincident( vtkLORLinearObject *testLinearObject, double threshold );

  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  vtkPolyData* CreateModelPolyData();

  std::vector<double> GetOrthogonalNormal1();
  std::vector<double> GetOrthogonalNormal2();

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );


protected:

  std::vector<double> EndPoint;

};

#endif