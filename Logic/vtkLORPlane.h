
#ifndef __vtkLORPlane_h
#define __vtkLORPlane_h

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
vtkLORPlane : public vtkLORLinearObject
{
public:
  vtkTypeMacro( vtkLORPlane, vtkObject );

  static vtkLORPlane* New();
  static vtkLORPlane* New( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 );

protected:

  // Constructor/destructor
  vtkLORPlane();
  virtual ~vtkLORPlane();

public:

  std::vector<double> EndPoint1;
  std::vector<double> EndPoint2;

public:

  std::vector<double> GetNormal();

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif