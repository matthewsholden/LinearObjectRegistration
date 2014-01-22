
#include "vtkMRMLLORConstants.h"

const int vtkMRMLLORConstants::REFERENCE_DOF = 4; // This obviously isn't true, but we need to distinguish from points
const int vtkMRMLLORConstants::POINT_DOF = 0;
const int vtkMRMLLORConstants::LINE_DOF = 1;
const int vtkMRMLLORConstants::PLANE_DOF = 2;
const int vtkMRMLLORConstants::UNKNOWN_DOF = -1;

const int vtkMRMLLORConstants::MINIMUM_COLLECTION_POSITIONS = 100;
const int vtkMRMLLORConstants::TRIM_POSITIONS = 10;

const double vtkMRMLLORConstants::NOISE_THRESHOLD = 0.5;
const double vtkMRMLLORConstants::MATCHING_THRESHOLD = 10.0;
const double vtkMRMLLORConstants::DIRECTION_SCALE = 100.0;