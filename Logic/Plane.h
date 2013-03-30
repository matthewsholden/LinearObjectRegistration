//BTX

#ifndef PLANE_H
#define PLANE_H

#include "LinearObject.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// This class stores a vector of values and a string label
class Plane : public LinearObject
{
public:
  std::vector<double> EndPoint1;
  std::vector<double> EndPoint2;

  Plane();
  Plane( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 );
  ~Plane();

  std::vector<double> GetNormal();
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif

//ETX