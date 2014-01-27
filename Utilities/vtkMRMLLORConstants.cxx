
#include "vtkMRMLLORConstants.h"

const int vtkMRMLLORConstants::REFERENCE_DOF = 4; // This obviously isn't true, but we need to distinguish from points
const int vtkMRMLLORConstants::POINT_DOF = 0;
const int vtkMRMLLORConstants::LINE_DOF = 1;
const int vtkMRMLLORConstants::PLANE_DOF = 2;
const int vtkMRMLLORConstants::UNKNOWN_DOF = -1;

extern const int vtkMRMLLORConstants::REFERENCE_INDEX = 0;
extern const int vtkMRMLLORConstants::POINT_INDEX = 1;
extern const int vtkMRMLLORConstants::LINE_INDEX = 2;
extern const int vtkMRMLLORConstants::PLANE_INDEX = 3;

const std::string vtkMRMLLORConstants::REFERENCE_STRING = "Reference";
const std::string vtkMRMLLORConstants::POINT_STRING = "Point";
const std::string vtkMRMLLORConstants::LINE_STRING = "Line";
const std::string vtkMRMLLORConstants::PLANE_STRING = "Plane";

const int vtkMRMLLORConstants::MINIMUM_COLLECTION_POSITIONS = 100;
const int vtkMRMLLORConstants::TRIM_POSITIONS = 10;

const double vtkMRMLLORConstants::NOISE_THRESHOLD = 0.5;
const double vtkMRMLLORConstants::MATCHING_THRESHOLD = 10.0;
const double vtkMRMLLORConstants::DIRECTION_SCALE = 100.0;



int vtkMRMLLORConstants
::STRING_TO_DOF( std::string s )
{
  if ( s.compare( REFERENCE_STRING ) == 0 )
  {
    return REFERENCE_DOF;
  }
  if ( s.compare( POINT_STRING ) == 0 )
  {
    return POINT_DOF;
  }
  if ( s.compare( LINE_STRING ) == 0 )
  {
    return LINE_DOF;
  }
  if ( s.compare( PLANE_STRING ) == 0 )
  {
    return PLANE_DOF;
  }

  return UNKNOWN_DOF;
}


int vtkMRMLLORConstants
::STRING_TO_INDEX( std::string s )
{
  if ( s.compare( REFERENCE_STRING ) == 0 )
  {
    return REFERENCE_INDEX;
  }
  if ( s.compare( POINT_STRING ) == 0 )
  {
    return POINT_INDEX;
  }
  if ( s.compare( LINE_STRING ) == 0 )
  {
    return LINE_INDEX;
  }
  if ( s.compare( PLANE_STRING ) == 0 )
  {
    return PLANE_INDEX;
  }

  return -1;
}


std::string vtkMRMLLORConstants
::DOF_TO_STRING( int d )
{
  if ( d == REFERENCE_DOF )
  {
    return REFERENCE_STRING;
  }
  if ( d == POINT_DOF )
  {
    return POINT_STRING;
  }
  if ( d == LINE_DOF )
  {
    return LINE_STRING;
  }
  if ( d == PLANE_DOF )
  {
    return PLANE_STRING;
  }

  return "";
}


std::string vtkMRMLLORConstants
::INDEX_TO_STRING( int i )
{
  if ( i == REFERENCE_INDEX )
  {
    return REFERENCE_STRING;
  }
  if ( i == POINT_INDEX )
  {
    return POINT_STRING;
  }
  if ( i == LINE_INDEX )
  {
    return LINE_STRING;
  }
  if ( i == PLANE_INDEX )
  {
    return PLANE_STRING;
  }

  return "";
}