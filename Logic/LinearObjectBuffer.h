//BTX

#ifndef LINEAROBJECTBUFFER_H
#define LINEAROBJECTBUFFER_H

#include "LinearObject.h"
#include "Reference.h"
#include "Point.h"
#include "Line.h"
#include "Plane.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"

// This class stores a vector of values and a string label
class LinearObjectBuffer
{
private:
	std::vector<LinearObject*> objects;

public:
  LinearObjectBuffer();
  ~LinearObjectBuffer();

  int Size();
  LinearObject* GetLinearObject( int index );
  LinearObject* GetLinearObject( std::string name );
  void AddLinearObject( LinearObject* newObject );
  void Concatenate( LinearObjectBuffer* catBuffer );

  void Translate( std::vector<double> vector );

  void CalculateSignature( LinearObjectBuffer* refBuffer );

  LinearObjectBuffer* GetMatches( LinearObjectBuffer* candidates );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif

//ETX