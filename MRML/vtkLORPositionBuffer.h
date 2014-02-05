
#ifndef __vtkLORPositionBuffer_h
#define __vtkLORPositionBuffer_h

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
#include "vtkLORPosition.h"
#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"


class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT
vtkLORPositionBuffer : public vtkObject
{
public:
  vtkTypeMacro( vtkLORPositionBuffer, vtkObject );

  static vtkLORPositionBuffer* New();

  vtkSmartPointer< vtkLORPositionBuffer > DeepCopy();

protected:

  // Constructor/destructor
  vtkLORPositionBuffer();
  virtual ~vtkLORPositionBuffer();

public:

  // Standard collection functionality
  int Size();
  vtkLORPosition* GetPosition( int index );
  virtual void AddPosition( vtkLORPosition* newPosition );
  virtual void Clear();
  virtual void Trim( int trimSize );

  virtual void Translate( std::vector<double> translation );

  virtual std::vector<double> CalculateCentroid();
  virtual vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );
  int GetDOF( double noiseThreshold );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

protected:
  std::vector< vtkSmartPointer< vtkLORPosition > > Positions;

};

#endif