
#ifndef __vtkLORRealTimePositionBuffer_h
#define __vtkLORRealTimePositionBuffer_h

// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>


// LinearObjectRegistration includes
#include "vtkLORPositionBuffer.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkLORRealTimePositionBuffer : public vtkLORPositionBuffer
{
public:
  vtkTypeMacro( vtkLORRealTimePositionBuffer, vtkObject );

  static vtkLORRealTimePositionBuffer* New();

protected:

  // Constructor/destructor
  vtkLORRealTimePositionBuffer();
  virtual ~vtkLORRealTimePositionBuffer();

public:


  // Standard collection functionality
  void AddPosition( vtkLORPosition* newPosition );
  void Clear();
  void Trim( int trimSize ); // Very slow for real-time position buffers (not recommended)

  void Translate( std::vector<double> translation ); // Very slow for real-time position buffers (not recommended)

  std::vector<double> CalculateCentroid();
  vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );
  // Get DOF will be the same as before, but just use the more efficient centroid/covariance matrix calculation
  // Note that the eigenvalue calculation is always for a 3x3 matrix - so it does not change as number of recorded positions increases

protected:

  vnl_matrix<double>* CurrentOuterProductSum;
  std::vector<double> CurrentVectorSum;

};

#endif