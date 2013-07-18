
#ifndef __vtkLORPointObservation_h
#define __vtkLORPointObservation_h

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

// VNL includes
#include "vnl/vnl_matrix.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkSlicerLinearObjectRegistrationModuleLogicExport.h"


// This class stores a vector of values only - we do not care about time
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkLORPointObservation : public vtkObject
{
public:
  vtkTypeMacro( vtkLORPointObservation, vtkObject );

  static vtkLORPointObservation* New();
  static vtkLORPointObservation* New( std::vector<double> newObservation );

protected:

  // Constructor/destructor
  vtkLORPointObservation();
  virtual ~vtkLORPointObservation();

public:
  std::vector<double> Observation;
  static const int SIZE = 3;
  static const int MATRIX_ELEMENTS = 16;

public:

  void Translate ( std::vector<double> translation );
  void Rotate( vnl_matrix<double>* rotation );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );
  bool FromXMLElement( vtkXMLDataElement* currElement, vtkXMLDataElement* prevElement ); // Exclude if threshold not exceeded

};

#endif