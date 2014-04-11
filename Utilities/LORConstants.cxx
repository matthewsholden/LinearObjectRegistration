
#include "LORConstants.h"

const int LORConstants::REFERENCE_DOF = 4; // This obviously isn't true, but we need to distinguish from points
const int LORConstants::POINT_DOF = 0;
const int LORConstants::LINE_DOF = 1;
const int LORConstants::PLANE_DOF = 2;
const int LORConstants::UNKNOWN_DOF = -1;

// These should be human readable
const std::string LORConstants::REFERENCE_STRING = "Reference";
const std::string LORConstants::POINT_STRING = "Point";
const std::string LORConstants::LINE_STRING = "Line";
const std::string LORConstants::PLANE_STRING = "Plane";

extern const int LORConstants::REFERENCE_SPINNER_INDEX = 0;
extern const int LORConstants::POINT_SPINNER_INDEX = 1;
extern const int LORConstants::LINE_SPINNER_INDEX = 2;
extern const int LORConstants::PLANE_SPINNER_INDEX = 3;

const std::string LORConstants::REFERENCE_COLLECT_STATE = "ReferenceCollectState";
const std::string LORConstants::POINT_COLLECT_STATE = "PointCollectState";
const std::string LORConstants::LINE_COLLECT_STATE = "LineCollectState";
const std::string LORConstants::PLANE_COLLECT_STATE = "PlaneCollectState";
const std::string LORConstants::UNKNOWNDOF_COLLECT_STATE = "UnknownDOFCollectState";

// These should be human readable
const std::string LORConstants::MANUAL_DOF_COLLECT_MODE = "Manual DOF";
const std::string LORConstants::MANUAL_SEGMENTATION_COLLECT_MODE = "Manual Segmentation";
const std::string LORConstants::AUTOMATIC_COLLECT_MODE = "Automatic";
const std::string LORConstants::MODEL_COLLECT_MODE = "Model";

const double LORConstants::DIRECTION_SCALE = 100.0;
const double LORConstants::ROTATION_THRESHOLD = 0.005;
const double LORConstants::TRANSLATION_THRESHOLD = 0.5;



int LORConstants
::COLLECT_STATE_TO_DOF( std::string s )
{
  if ( s.compare( REFERENCE_COLLECT_STATE ) == 0 )
  {
    return REFERENCE_DOF;
  }
  if ( s.compare( POINT_COLLECT_STATE ) == 0 )
  {
    return POINT_DOF;
  }
  if ( s.compare( LINE_COLLECT_STATE ) == 0 )
  {
    return LINE_DOF;
  }
  if ( s.compare( PLANE_COLLECT_STATE ) == 0 )
  {
    return PLANE_DOF;
  }

  return UNKNOWN_DOF;
}


std::string LORConstants
::DOF_TO_COLLECT_STATE( int d )
{
  if ( d == REFERENCE_DOF )
  {
    return REFERENCE_COLLECT_STATE;
  }
  if ( d == POINT_DOF )
  {
    return POINT_COLLECT_STATE;
  }
  if ( d == LINE_DOF )
  {
    return LINE_COLLECT_STATE;
  }
  if ( d == PLANE_DOF )
  {
    return PLANE_COLLECT_STATE;
  }

  return UNKNOWNDOF_COLLECT_STATE;
}


int LORConstants
::DOF_TO_SPINNER_INDEX( int d )
{
  if ( REFERENCE_DOF == d )
  {
    return REFERENCE_SPINNER_INDEX;
  }
  if ( POINT_DOF == d )
  {
    return POINT_SPINNER_INDEX;
  }
  if ( LINE_DOF == d )
  {
    return LINE_SPINNER_INDEX;
  }
  if ( PLANE_DOF == d )
  {
    return PLANE_SPINNER_INDEX;
  }

  return -1;
}


int LORConstants
::SPINNER_INDEX_TO_DOF( int i )
{
  if ( i == REFERENCE_SPINNER_INDEX )
  {
    return REFERENCE_DOF;
  }
  if ( i == POINT_SPINNER_INDEX )
  {
    return POINT_DOF;
  }
  if ( i == LINE_SPINNER_INDEX )
  {
    return LINE_DOF;
  }
  if ( i == PLANE_SPINNER_INDEX )
  {
    return PLANE_DOF;
  }

  return UNKNOWN_DOF;
}


int LORConstants
::STRING_TO_SPINNER_INDEX( std::string s )
{
  if ( s.compare( REFERENCE_STRING ) == 0 )
  {
    return REFERENCE_SPINNER_INDEX;
  }
  if ( s.compare( POINT_STRING ) == 0 )
  {
    return POINT_SPINNER_INDEX;
  }
  if ( s.compare( LINE_STRING ) == 0 )
  {
    return LINE_SPINNER_INDEX;
  }
  if ( s.compare( PLANE_STRING ) == 0 )
  {
    return PLANE_SPINNER_INDEX;
  }

  return -1;
}


std::string LORConstants
::SPINNER_INDEX_TO_STRING( int i )
{
  if ( i == REFERENCE_SPINNER_INDEX )
  {
    return REFERENCE_STRING;
  }
  if ( i == POINT_SPINNER_INDEX )
  {
    return POINT_STRING;
  }
  if ( i == LINE_SPINNER_INDEX )
  {
    return LINE_STRING;
  }
  if ( i == PLANE_SPINNER_INDEX )
  {
    return PLANE_STRING;
  }

  return "";
}