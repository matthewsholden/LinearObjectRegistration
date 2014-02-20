
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
#include "vtkCellLocator.h"
#include "vtkPointLocator.h"
#include "vtkPolygon.h"
#include "vtkGenericCell.h"
#include "vtkFeatureEdges.h"

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
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"

// LinearObjectRegistration includes
#include "vtkMRMLLinearObjectRegistrationNode.h"
#include "vtkMRMLLinearObjectCollectionNode.h"
#include "vtkLORPoint.h"
#include "vtkLORLine.h"
#include "vtkLORPlane.h"
#include "vtkLORReference.h"
#include "vtkLORPositionBuffer.h"
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

  vtkSmartPointer< vtkMatrix4x4 > SphericalRegistration( vtkLORPositionBuffer* fromPoints, vtkLORPositionBuffer* toPoints, vtkMatrix4x4* currentFromToToTransform );
  std::vector<double> TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vtkMatrix4x4* rotation );
  std::vector<double> TranslationalAdjustment( vtkLORPositionBuffer* fromPositions, vtkLORPositionBuffer* toPositions, vtkMatrix4x4* currentFromToToTransform );

  void VNLMatrixToVTKMatrix( vnl_matrix<double>* vnlMatrix, vtkMatrix4x4* vtkMatrix );
  void MatrixRotationPart( vtkMatrix4x4* matrix, vtkMatrix4x4* rotation );
  std::vector<double> MatrixTranslationPart( vtkMatrix4x4* matrix );
  void RotationTranslationToMatrix( vtkMatrix4x4* rotation, std::vector<double> translation, vtkMatrix4x4* matrix );

  double LinearObjectICPTA( vtkMRMLLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* initialRotation, vtkMatrix4x4* calculatedMatrix );

  vtkSmartPointer< vtkMatrix4x4 > CombineRotationAndTranslation( vtkMatrix4x4* rotation, std::vector<double> translation );

  void FindClosestPositions( vtkMRMLLinearObjectCollectionNode* fromLinearObjects, vtkMRMLLinearObjectCollectionNode* toLinearObjects, vtkMatrix4x4* currentFromToToTransform,
                    vtkLORPositionBuffer* fromPositions, vtkLORPositionBuffer* toPositions );

  std::string GetOutputMessage( std::string nodeID );
  void SetOutputMessage( std::string nodeID, std::string newOutputMessage );

  vtkSmartPointer< vtkLORLinearObject > PositionBufferToLinearObject( vtkLORPositionBuffer* positionBuffer, double noiseThreshold, int dof = -1 );

  vtkSmartPointer< vtkLORLinearObject > CorrespondPointToReference( vtkLORLinearObject* linearObject, vtkMRMLLinearObjectRegistrationNode* lorNode );

  void CreateModelPlane( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer );
  void CreateModelLine( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer );
  void CreateModelPoint( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer );
  void CreateModelReference( vtkMRMLNode* node, vtkLORPositionBuffer* positionBuffer );

  vtkMRMLLinearObjectCollectionNode* GetActiveCollectionNode();
  void SetActiveCollectionNode( vtkMRMLLinearObjectCollectionNode* newActiveCollectionNode );
  vtkMRMLMarkupsNode* GetActiveMarkupsNode();
  void SetActiveMarkupsNode( vtkMRMLMarkupsNode* newActiveMarkupsNode );

  void MatchCollections( vtkMRMLLinearObjectCollectionNode* collection0, vtkMRMLLinearObjectCollectionNode* collection1, double matchingThreshold, bool removeUnmatched = false );
  void PairCollections( vtkMRMLLinearObjectCollectionNode* collection0, vtkMRMLLinearObjectCollectionNode* collection1 );
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > GetReferences( vtkMRMLLinearObjectCollectionNode* collection );
  vtkSmartPointer< vtkMRMLLinearObjectCollectionNode > GetNonReferences( vtkMRMLLinearObjectCollectionNode* collection );

  void CreateLinearObjectModelHierarchyNode( vtkLORLinearObject* linearObject, vtkMRMLLinearObjectCollectionNode* collection );
  void CreateLinearObjectCollectionModelHierarchyNode( vtkMRMLLinearObjectCollectionNode* collection );
  void CreateLinearObjectModelHierarchyNodes( vtkMRMLLinearObjectCollectionNode* collection );

  void ShowLinearObjectModel( vtkLORLinearObject* linearObject );
  void HideLinearObjectModel( vtkLORLinearObject* linearObject );
  bool GetLinearObjectModelVisibility( vtkLORLinearObject* linearObject );

  void ShowLinearObjectCollectionModel( vtkMRMLLinearObjectCollectionNode* collection );
  void HideLinearObjectCollectionModel( vtkMRMLLinearObjectCollectionNode* collection );
  bool GetLinearObjectCollectionModelVisibility( vtkMRMLLinearObjectCollectionNode* collection );

  // Only to be called from the register method
  void GetFromAndToCollections( vtkMRMLLinearObjectCollectionNode* fromCollection, vtkMRMLLinearObjectCollectionNode* fromReferenceCollection, vtkMRMLLinearObjectCollectionNode* fromPointCollection, vtkMRMLLinearObjectCollectionNode* fromLineCollection, vtkMRMLLinearObjectCollectionNode* fromPlaneCollection,
                          vtkMRMLLinearObjectCollectionNode* toCollection, vtkMRMLLinearObjectCollectionNode* toReferenceCollection, vtkMRMLLinearObjectCollectionNode* toPointCollection, vtkMRMLLinearObjectCollectionNode* toLineCollection, vtkMRMLLinearObjectCollectionNode* toPlaneCollection );

private:

  std::map< std::string, std::string > OutputMessages;
};

#endif
