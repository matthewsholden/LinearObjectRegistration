//BTX

#ifndef REFERENCE_H
#define REFERENCE_H

#include "LinearObject.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// This class stores a vector of values and a string label
class Reference : public LinearObject
{
public:
  Reference();
  Reference( std::vector<double> newBasePoint );
  ~Reference();

  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif

//ETX