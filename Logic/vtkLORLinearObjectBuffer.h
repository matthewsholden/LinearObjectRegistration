
#ifndef vtkLORLinearObjectBuffer_h
#define vtkLORLinearObjectBuffer_h

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
	std::vector< vtkSmartPointer< vtkLORLinearObject > > objects;

public:

  // Basic collection functionality
  int Size();
  vtkSmartPointer< vtkLORLinearObject > GetLinearObject( int index );
  vtkSmartPointer< vtkLORLinearObject > GetLinearObject( std::string name );
  void AddLinearObject( vtkSmartPointer< vtkLORLinearObject > newObject );
  void Concatenate( vtkSmartPointer< vtkLORLinearObjectBuffer > catBuffer );

  void Translate( std::vector<double> vector );

  void CalculateSignature( vtkSmartPointer< vtkLORLinearObjectBuffer > refBuffer );

  vtkSmartPointer< vtkLORLinearObjectBuffer > GetMatches( vtkSmartPointer< vtkLORLinearObjectBuffer > candidates, double matchingThreshold );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );

};

#endif