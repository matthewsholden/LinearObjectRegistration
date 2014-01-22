
#ifndef __vtkMRMLLORPositionNode_h
#define __vtkMRMLLORPositionNode_h

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
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"

// LinearObjectRegistration includes
#include "vtkMRMLLORVectorMath.h"

// VNL includes
#include "vnl/vnl_matrix.h"

#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


// This class stores a vector of values only - we do not care about time
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORPositionNode : public vtkObject
{
public:
  vtkTypeMacro( vtkMRMLLORPositionNode, vtkObject );

  static vtkMRMLLORPositionNode* New();
  static vtkMRMLLORPositionNode* New( std::vector<double> newPosition );
  static vtkMRMLLORPositionNode* New( vtkMatrix4x4* newMatrix );

  vtkSmartPointer< vtkMRMLLORPositionNode > DeepCopy();

protected:

  // Constructor/destructor
  vtkMRMLLORPositionNode();
  virtual ~vtkMRMLLORPositionNode();

public:

  static const int SIZE = 3;
  static const int MATRIX_ELEMENTS = 16;

  void Translate ( std::vector<double> translation );
  void Rotate( vnl_matrix<double>* rotation );

  void Transform( vtkMatrix4x4* matrix );

  std::vector<double> GetPositionVector();
  void SetPositionVector( std::vector<double> newPositionVector );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );
  // bool FromXMLElement( vtkXMLDataElement* currElement, vtkXMLDataElement* prevElement ); // Exclude if threshold not exceeded

protected:

  std::vector<double> PositionVector;

};

#endif