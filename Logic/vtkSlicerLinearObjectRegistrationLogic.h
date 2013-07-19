
// .NAME vtkSlicerLinearObjectRegistrationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerLinearObjectRegistrationLogic_h
#define __vtkSlicerLinearObjectRegistrationLogic_h


// Standard includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>

// VTK includes
#include "vtkObject.h"
#include "vtkObjectBase.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkSmartPointer.h"
#include "vtkXMLDataParser.h"
#include "vtkNew.h"
#include "vtkIntArray.h"
#include "vtkMatrix4x4.h"

// VNL includes
#include "vnl/vnl_matrix.h"

// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"

// LinearObjectRegistration includes
#include "vtkLORLinearObject.h"
#include "vtkLORLinearObjectBuffer.h"
#include "vtkLORPoint.h"
#include "vtkLORLine.h"
#include "vtkLORPlane.h"
#include "vtkLORReference.h"
#include "vtkLORPointObservation.h"
#include "vtkLORPointObservationBuffer.h"
#include "vtkSlicerLinearObjectRegistrationModuleLogicExport.h"




/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_LINEAROBJECTREGISTRATION_MODULE_LOGIC_EXPORT
vtkSlicerLinearObjectRegistrationLogic : public vtkSlicerModuleLogic
{

// This is general logic ----------------------------------------------------------------

public:

  static vtkSlicerLinearObjectRegistrationLogic *New();
  vtkTypeMacro(vtkSlicerLinearObjectRegistrationLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void OnMRMLSceneEndClose();
 
  
protected:
  
  vtkSlicerLinearObjectRegistrationLogic();
  virtual ~vtkSlicerLinearObjectRegistrationLogic();
  
  virtual void SetMRMLSceneInternal( vtkMRMLScene* newScene );
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded( vtkMRMLNode* node );
  virtual void OnMRMLSceneNodeRemoved( vtkMRMLNode* node );
  

private:

  vtkSlicerLinearObjectRegistrationLogic(const vtkSlicerLinearObjectRegistrationLogic&); // Not implemented
  void operator=(const vtkSlicerLinearObjectRegistrationLogic&);               // Not implemented


// Specific LinearObjectRegistration logic -----------------------------------------------

public:
  void ImportGeometry( std::string fileName );
  void ImportRecord( std::string fileName, int filterWidth, int collectionFrames, double extractionThreshold );
  void Register( double matchingThreshold );
  void SetRegistrationTransformNode( vtkSmartPointer< vtkMRMLLinearTransformNode > newRegistrationTransformNode );

  vnl_matrix<double>* LinearObjectICP( vnl_matrix<double>* initialRotation );

  double GetError();
  std::string GetStatus();


private:

  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryPointBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryLineBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryPlaneBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > GeometryReferenceBuffer;

  void ResetGeometry();

  vtkSmartPointer< vtkLORLinearObjectBuffer > RecordPointBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > RecordLineBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > RecordPlaneBuffer;
  vtkSmartPointer< vtkLORLinearObjectBuffer > RecordReferenceBuffer;

  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > ReferencePoints;
  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > LinearObjectPoints;
  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > PointPoints;
  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > LinePoints;
  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > PlanePoints;

  void ResetRecord();

  vtkSmartPointer< vtkMRMLLinearTransformNode > RegistrationTransformNode;

  double ErrorRMS;
  std::string Status;

};

#endif
