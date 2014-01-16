
#ifndef __vtkMRMLLORPlaneNode_h
#define __vtkMRMLLORPlaneNode_h

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
#include "vtkPlaneSource.h"

// LinearObjectRegistration includes
#include "vtkMRMLLORLinearObjectNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


// This class stores a vector of values and a string label
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORPlaneNode : public vtkMRMLLORLinearObjectNode
{
public:
  vtkTypeMacro( vtkMRMLLORPlaneNode, vtkObject );

  static vtkMRMLLORPlaneNode* New();
  static vtkMRMLLORPlaneNode* New( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 );

  vtkSmartPointer< vtkMRMLLORLinearObjectNode > DeepCopy();

protected:

  // Constructor/destructor
  vtkMRMLLORPlaneNode();
  virtual ~vtkMRMLLORPlaneNode();

public:

  std::vector<double> GetNormal();

  // Implement inherited abstract methods
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  vtkPolyData* CreateModelPolyData();

  // Implement inherited abstract methods
  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

protected:

  std::vector<double> EndPoint1;
  std::vector<double> EndPoint2;

};

#endif