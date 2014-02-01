
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
#include <limits>

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
#include "vtkCommand.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLModelDisplayNode.h"

// LinearObjectRegistration includes
#include "vtkMRMLLinearObjectRegistrationNode.h"
#include "vtkMRMLLORLinearObjectCollectionNode.h"
#include "vtkMRMLLORPointNode.h"
#include "vtkMRMLLORLineNode.h"
#include "vtkMRMLLORPlaneNode.h"
#include "vtkMRMLLORReferenceNode.h"
#include "vtkMRMLLORPositionBufferNode.h"
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
 
  void ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData );
  void ProcessMRMLSceneEvents( vtkObject* caller, unsigned long event, void* callData );
  
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

  void CalculateTransform( vtkMRMLNode* node );
  void UpdateOutputTransform( vtkMRMLLinearTransformNode* outputTransform, vtkMatrix4x4* newTransformMatrix );

  vtkSmartPointer< vtkMatrix4x4 > SphericalRegistration( vtkMRMLLORPositionBufferNode* fromPoints, vtkMRMLLORPositionBufferNode* toPoints, vtkMatrix4x4* currentFromToToTransform );
  std::vector<double> TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vtkMatrix4x4* rotation );
  std::vector<double> TranslationalAdjustment( vtkMRMLLORPositionBufferNode* fromPositions, vtkMRMLLORPositionBufferNode* toPositions, vtkMatrix4x4* currentFromToToTransform );

  void VNLMatrixToVTKMatrix( vnl_matrix<double>* vnlMatrix, vtkMatrix4x4* vtkMatrix );
  void MatrixRotationPart( vtkMatrix4x4* matrix, vtkMatrix4x4* rotation );
  std::vector<double> MatrixTranslationPart( vtkMatrix4x4* matrix );
  void RotationTranslationToMatrix( vtkMatrix4x4* rotation, std::vector<double> translation, vtkMatrix4x4* matrix );

  double LinearObjectICPTA( vtkMRMLLORLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLORLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* initialRotation, vtkMatrix4x4* calculatedMatrix );

  vtkSmartPointer< vtkMatrix4x4 > CombineRotationAndTranslation( vtkMatrix4x4* rotation, std::vector<double> translation );

  void FindClosestPositions( vtkMRMLLORLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLORLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* currentFromToToTransform,
                    vtkMRMLLORPositionBufferNode* fromPositions, vtkMRMLLORPositionBufferNode* toPositions );

  std::string GetOutputMessage( std::string nodeID );
  void SetOutputMessage( std::string nodeID, std::string newOutputMessage );

  vtkSmartPointer< vtkMRMLLORLinearObjectNode > PositionBufferToLinearObject( vtkMRMLLORPositionBufferNode* positionBuffer, double noiseThreshold, int dof = -1 );

  vtkMRMLLORLinearObjectCollectionNode* GetActiveCollectionNode();
  void SetActiveCollectionNode( vtkMRMLLORLinearObjectCollectionNode* newActiveCollectionNode );

  void MatchCollections( vtkMRMLLORLinearObjectCollectionNode* collection0, vtkMRMLLORLinearObjectCollectionNode* collection1, double matchingThreshold, bool removeUnmatched = false );
  void PairCollections( vtkMRMLLORLinearObjectCollectionNode* collection0, vtkMRMLLORLinearObjectCollectionNode* collection1 );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > GetReferences( vtkMRMLLORLinearObjectCollectionNode* collection );
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > GetNonReferences( vtkMRMLLORLinearObjectCollectionNode* collection );

  void CreateLinearObjectModelHierarchyNode( vtkMRMLLORLinearObjectNode* linearObject, vtkMRMLLORLinearObjectCollectionNode* collection );
  void CreateLinearObjectCollectionModelHierarchyNode( vtkMRMLLORLinearObjectCollectionNode* collection );
  void CreateLinearObjectModelHierarchyNodes( vtkMRMLLORLinearObjectCollectionNode* collection );

  void ShowLinearObjectModel( vtkMRMLLORLinearObjectNode* linearObject );
  void HideLinearObjectModel( vtkMRMLLORLinearObjectNode* linearObject );
  bool GetLinearObjectModelVisibility( vtkMRMLLORLinearObjectNode* linearObject );

  void ShowLinearObjectCollectionModel( vtkMRMLLORLinearObjectCollectionNode* collection );
  void HideLinearObjectCollectionModel( vtkMRMLLORLinearObjectCollectionNode* collection );
  bool GetLinearObjectCollectionModelVisibility( vtkMRMLLORLinearObjectCollectionNode* collection );

  // Only to be called from the register method
  void GetFromAndToCollections( vtkMRMLLORLinearObjectCollectionNode* fromCollection, vtkMRMLLORLinearObjectCollectionNode* fromReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* fromPointCollection, vtkMRMLLORLinearObjectCollectionNode* fromLineCollection, vtkMRMLLORLinearObjectCollectionNode* fromPlaneCollection,
                          vtkMRMLLORLinearObjectCollectionNode* toCollection, vtkMRMLLORLinearObjectCollectionNode* toReferenceCollection, vtkMRMLLORLinearObjectCollectionNode* toPointCollection, vtkMRMLLORLinearObjectCollectionNode* toLineCollection, vtkMRMLLORLinearObjectCollectionNode* toPlaneCollection );

private:

  std::map< std::string, std::string > OutputMessages;
};

#endif
