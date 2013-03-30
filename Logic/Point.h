//BTX

#ifndef POINT_H
#define POINT_H

#include "LinearObject.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// This class stores a vector of values and a string label
class Point : public LinearObject
{
public:
  Point();
  Point( std::vector<double> newBasePoint );
  ~Point();

  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif

//ETX