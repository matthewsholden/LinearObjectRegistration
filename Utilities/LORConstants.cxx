
#include "LORConstants.h"

const int LORConstants::REFERENCE_DOF = 4; // This obviously isn't true, but we need to distinguish from points
const int LORConstants::POINT_DOF = 0;
const int LORConstants::LINE_DOF = 1;
const int LORConstants::PLANE_DOF = 2;
const int LORConstants::UNKNOWN_DOF = -1;

extern const int LORConstants::REFERENCE_INDEX = 0;
extern const int LORConstants::POINT_INDEX = 1;
extern const int LORConstants::LINE_INDEX = 2;
extern const int LORConstants::PLANE_INDEX = 3;

const std::string LORConstants::REFERENCE_STRING = "Reference";
const std::string LORConstants::POINT_STRING = "Point";
const std::string LORConstants::LINE_STRING = "Line";
const std::string LORConstants::PLANE_STRING = "Plane";

const int LORConstants::MINIMUM_COLLECTION_POSITIONS = 100;
const int LORConstants::TRIM_POSITIONS = 10;

const double LORConstants::NOISE_THRESHOLD = 0.5;
const double LORConstants::MATCHING_THRESHOLD = 10.0;
const double LORConstants::DIRECTION_SCALE = 100.0;



int LORConstants
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


int LORConstants
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


std::string LORConstants
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


std::string LORConstants
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