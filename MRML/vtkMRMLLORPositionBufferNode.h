
#ifndef __vtkMRMLLORPositionBufferNode_h
#define __vtkMRMLLORPositionBufferNode_h

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
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/algo/vnl_svd.h"

// LinearObjectRegistration includes
#include "vtkMRMLLORPositionNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORPositionBufferNode : public vtkObject
{
public:
  vtkTypeMacro( vtkMRMLLORPositionBufferNode, vtkObject );

  static vtkMRMLLORPositionBufferNode* New();

  vtkMRMLLORPositionBufferNode* DeepCopy();

protected:

  // Constructor/destructor
  vtkMRMLLORPositionBufferNode();
  virtual ~vtkMRMLLORPositionBufferNode();

public:

  // Standard collection functionality
  int Size();
  vtkSmartPointer< vtkMRMLLORPositionNode > GetPosition( int index );
  void AddPosition( vtkSmartPointer< vtkMRMLLORPositionNode > newPosition );
  void Clear();

  void Translate( std::vector<double> translation );

  std::vector<double> CalculateCentroid();
  vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );

  std::string ToXMLString();
  void FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element );

protected:
  std::vector< vtkSmartPointer< vtkMRMLLORPositionNode > > Positions;

};

#endif