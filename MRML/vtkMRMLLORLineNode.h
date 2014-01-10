
#ifndef __vtkMRMLLORLineNode_h
#define __vtkMRMLLORLineNode_h


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
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"

// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORLineNode : public vtkMRMLLORLinearObjectNode
{
public:
  vtkTypeMacro( vtkMRMLLORLineNode, vtkObject );

  static vtkMRMLLORLineNode* New();
  static vtkMRMLLORLineNode* New( std::vector<double> newBasePoint, std::vector<double> newEndPoint );

protected:

  // Constructor/destructor
  vtkMRMLLORLineNode();
  virtual ~vtkMRMLLORLineNode();


public:

  std::vector<double> GetDirection();

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  std::vector<double> GetOrthogonalNormal1();
  std::vector<double> GetOrthogonalNormal2();

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );


protected:

  std::vector<double> EndPoint;

};

#endif