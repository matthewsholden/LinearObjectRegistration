
#ifndef vtkLORLinearObjectBuffer_h
#define vtkLORLinearObjectBuffer_h

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
#include "vnl/algo/vnl_matrix_inverse.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkLORReference.h"
#include "vtkLORPoint.h"
#include "vtkLORLine.h"
#include "vtkLORPlane.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkLORLinearObjectBuffer : public vtkObject
{
public:
  vtkTypeMacro( vtkLORLinearObjectBuffer, vtkObject );

  static vtkLORLinearObjectBuffer* New();

protected:

  // Constructor/destructor
  vtkLORLinearObjectBuffer();
  virtual ~vtkLORLinearObjectBuffer();

private:
	std::vector<vtkLORLinearObject*> objects;

public:

  // Basic collection functionality
  int Size();
  vtkLORLinearObject* GetLinearObject( int index );
  vtkLORLinearObject* GetLinearObject( std::string name );
  void AddLinearObject( vtkLORLinearObject* newObject );
  void Concatenate( vtkLORLinearObjectBuffer* catBuffer );

  void Translate( std::vector<double> vector );

  void CalculateSignature( vtkLORLinearObjectBuffer* refBuffer );

  vtkLORLinearObjectBuffer* GetMatches( vtkLORLinearObjectBuffer* candidates, double matchingThreshold );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif