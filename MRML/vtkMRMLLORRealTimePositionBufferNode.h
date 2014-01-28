
#ifndef __vtkMRMLLORRealTimePositionBufferNode_h
#define __vtkMRMLLORRealTimePositionBufferNode_h

// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>


// LinearObjectRegistration includes
#include "vtkMRMLLORPositionBufferNode.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkMRMLLORRealTimePositionBufferNode : public vtkMRMLLORPositionBufferNode
{
public:
  vtkTypeMacro( vtkMRMLLORRealTimePositionBufferNode, vtkObject );

  static vtkMRMLLORRealTimePositionBufferNode* New();

protected:

  // Constructor/destructor
  vtkMRMLLORRealTimePositionBufferNode();
  virtual ~vtkMRMLLORRealTimePositionBufferNode();

public:


  // Standard collection functionality
  void AddPosition( vtkMRMLLORPositionNode* newPosition );

  std::vector<double> CalculateCentroid();
  vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );
  // Get DOF will be the same as before, but just use the more efficient centroid/covariance matrix calculation
  // Note that the eigenvalue calculation is always for a 3x3 matrix - so it does not change as number of recorded positions increases

protected:

  vnl_matrix<double>* CurrentOuterProductSum;
  std::vector<double> CurrentVectorSum;

};

#endif