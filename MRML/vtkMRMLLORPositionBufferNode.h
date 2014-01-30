
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

  vtkSmartPointer< vtkMRMLLORPositionBufferNode > DeepCopy();

protected:

  // Constructor/destructor
  vtkMRMLLORPositionBufferNode();
  virtual ~vtkMRMLLORPositionBufferNode();

public:

  // Standard collection functionality
  int Size();
  vtkMRMLLORPositionNode* GetPosition( int index );
  virtual void AddPosition( vtkMRMLLORPositionNode* newPosition );
  void Clear();
  void Trim( int trimSize );

  void Translate( std::vector<double> translation );

  virtual std::vector<double> CalculateCentroid();
  virtual vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );
  int GetDOF( double noiseThreshold );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

protected:
  std::vector< vtkSmartPointer< vtkMRMLLORPositionNode > > Positions;

};

#endif