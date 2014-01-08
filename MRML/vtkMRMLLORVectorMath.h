
#ifndef __vtkMRMLLORVectorMath_h
#define __vtkMRMLLORVectorMath_h

// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vtkSlicerLinearObjectRegistrationModuleMRMLExport.h"

namespace vtkMRMLLORVectorMath
{

  // Note: Do not remove any VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT - it exports these functions so they are usable by the logic
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT double Distance( std::vector<double> v1, std::vector<double> v2 );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT double Norm( std::vector<double> vector );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT double Dot( std::vector<double> v1, std::vector<double> v2 );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::vector<double> Cross( std::vector<double> v1, std::vector<double> v2 );

  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::vector<double> Add( std::vector<double> v1, std::vector<double> v2 );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::vector<double> Subtract( std::vector<double> v1, std::vector<double> v2 );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::vector<double> Multiply( double c, std::vector<double> vector ); 

  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::string VectorToString( std::vector<double> vector );
  VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_MRML_EXPORT std::vector<double> StringToVector( std::string s, int size );

}

#endif