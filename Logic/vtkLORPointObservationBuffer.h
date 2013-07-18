
#ifndef __vtkLORPointObservationBuffer_h
#define __vtkLORPointObservationBuffer_h

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
#include "vtkLORPointObservation.h"
#include "vtkLORLinearObject.h"
#include "vtkLORPoint.h"
#include "vtkLORLine.h"
#include "vtkLORPlane.h"
#include "vtkLORReference.h"
#include "vtkSlicerLinearObjectRegistrationModuleLogicExport.h"


class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkLORPointObservationBuffer : public vtkObject
{
public:
  vtkTypeMacro( vtkLORPointObservationBuffer, vtkObject );

  static vtkLORPointObservationBuffer* New();

protected:

  // Constructor/destructor
  vtkLORPointObservationBuffer();
  virtual ~vtkLORPointObservationBuffer();

private:
  std::vector<vtkLORPointObservation*> observations;

public:

  // Standard collection functionality
  int Size();
  vtkLORPointObservation* GetObservation( int index );
  void AddObservation( vtkLORPointObservation* newObservation );
  void Clear();

  void Translate( std::vector<double> translation );

  vtkLORLinearObject* LeastSquaresLinearObject( int dof );
  void Filter( vtkLORLinearObject* object, int filterWidth );

  vnl_matrix<double>* SphericalRegistration( vtkLORPointObservationBuffer* fromPoints );
  vnl_matrix<double>* TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vnl_matrix<double>* rotation );

  std::vector<vtkLORPointObservationBuffer*> ExtractLinearObjects( int collectionFrames, double extractionThreshold, std::vector<int>* dof );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

private:
  std::vector<double> CalculateCentroid();
  vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );

};

#endif