project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkSlicer${MODULE_NAME}ModuleUtilities_SOURCE_DIR}
  ${vtkSlicer${MODULE_NAME}ModuleUtilities_BINARY_DIR}
  )

# --------------------------------------------------------------------------
# Sources

set(${KIT}_SRCS
  vtkMRMLLinearObjectRegistrationNode.cxx
  vtkMRMLLinearObjectRegistrationNode.h
  vtkMRMLLinearObjectCollectionNode.cxx
  vtkMRMLLinearObjectCollectionNode.h
  vtkMRMLLinearObjectCollectionStorageNode.cxx
  vtkMRMLLinearObjectCollectionStorageNode.h
  vtkLORReference.cxx
  vtkLORReference.h
  vtkLORPositionBuffer.cxx
  vtkLORPositionBuffer.h
  vtkLORRealTimePositionBuffer.cxx
  vtkLORRealTimePositionBuffer.h
  vtkLORPosition.cxx
  vtkLORPosition.h
  vtkLORPoint.cxx
  vtkLORPoint.h
  vtkLORPlane.cxx
  vtkLORPlane.h
  vtkLORLinearObject.cxx
  vtkLORLinearObject.h
  vtkLORLine.cxx
  vtkLORLine.h
  )

SET (${KIT}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleUtilities
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