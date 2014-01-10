project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${Slicer_Base_INCLUDE_DIRS}
  )

# --------------------------------------------------------------------------
# Sources

set(${KIT}_SRCS
  vtkMRMLLinearObjectRegistrationNode.cxx
  vtkMRMLLinearObjectRegistrationNode.h
  vtkMRMLLORVectorMath.cxx
  vtkMRMLLORVectorMath.h
  vtkMRMLLORReferenceNode.cxx
  vtkMRMLLORReferenceNode.h
  vtkMRMLLORPositionBufferNode.cxx
  vtkMRMLLORPositionBufferNode.h
  vtkMRMLLORPositionNode.cxx
  vtkMRMLLORPositionNode.h
  vtkMRMLLORPointNode.cxx
  vtkMRMLLORPointNode.h
  vtkMRMLLORPlaneNode.cxx
  vtkMRMLLORPlaneNode.h
  vtkMRMLLORLinearObjectCollectionNode.cxx
  vtkMRMLLORLinearObjectCollectionNode.h
  vtkMRMLLORLinearObjectCollectionStorageNode.cxx
  vtkMRMLLORLinearObjectCollectionStorageNode.h
  vtkMRMLLORLinearObjectNode.cxx
  vtkMRMLLORLinearObjectNode.h
  vtkMRMLLORLineNode.cxx
  vtkMRMLLORLineNode.h
  )

SET (${KIT}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

set(${KIT}_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
  ${MRML_LIBRARIES}
  SlicerBaseLogic
  )

SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )