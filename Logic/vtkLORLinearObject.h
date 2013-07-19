
#ifndef __vtkLORLinearObject_h
#define __vtkLORLinearObject_h

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

// LinearObjectRegistration includes
#include "vtkSlicerLinearObjectRegistrationModuleLogicExport.h"

// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkLORLinearObject : public vtkObject
{
public:
  vtkTypeMacro( vtkLORLinearObject, vtkObject );

  // No "New" function - this class will be abstract

protected:

  // Constructor/Destructor
  vtkLORLinearObject();
  virtual ~vtkLORLinearObject();

public:

  std::string Name;
  std::string Type;
  std::vector<double> Signature;
  std::vector<double> BasePoint;

  static const int DIMENSION = 3;

public:


  double DistanceToVector( std::vector<double> vector );

  // Abstract functions that must be implemented in subclasses
  virtual std::vector<double> ProjectVector( std::vector<double> vector ) = 0;
  virtual void Translate( std::vector<double> vector ) = 0;

  virtual std::string ToXMLString() = 0;
  virtual void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element ) = 0;

};


double Distance( std::vector<double> v1, std::vector<double> v2 );
double Norm( std::vector<double> vector );
double Dot( std::vector<double> v1, std::vector<double> v2 );
std::vector<double> Cross( std::vector<double> v1, std::vector<double> v2 );

std::vector<double> Add( std::vector<double> v1, std::vector<double> v2 );
std::vector<double> Subtract( std::vector<double> v1, std::vector<double> v2 );
std::vector<double> Multiply( double c, std::vector<double> vector ); 

std::string VectorToString( std::vector<double> vector );
std::vector<double> StringToVector( std::string s, int size );


#endif