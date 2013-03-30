//BTX

#ifndef POINTOBSERVATION_H
#define POINTOBSERVATION_H

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vtkXMLDataElement.h"

// This class stores a vector of values only - we do not care about time
class PointObservation
{
public:
  std::vector<double> Observation;
  static const int SIZE = 3;

public:
  PointObservation();
  PointObservation( std::vector<double> newObservation );
  ~PointObservation();

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif

//ETX