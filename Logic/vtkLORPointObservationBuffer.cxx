
#include "vtkLORPointObservationBuffer.h"

vtkStandardNewMacro( vtkLORPointObservationBuffer );

vtkLORPointObservationBuffer
::vtkLORPointObservationBuffer()
{
}


vtkLORPointObservationBuffer
::~vtkLORPointObservationBuffer()
{
  this->Clear();
}


int vtkLORPointObservationBuffer
::Size()
{
  return this->observations.size();
}


vtkSmartPointer< vtkLORPointObservation > vtkLORPointObservationBuffer
::GetObservation( int index )
{
  return this->observations.at(index);
}


void vtkLORPointObservationBuffer
::AddObservation( vtkSmartPointer< vtkLORPointObservation > newObservation )
{
  this->observations.push_back( newObservation );
}


void vtkLORPointObservationBuffer
::Clear()
{
  this->observations.clear();
}


void vtkLORPointObservationBuffer
::Translate( std::vector<double> translation )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetObservation(i)->Translate( translation );
  }
}


vnl_matrix<double>* vtkLORPointObservationBuffer
::SphericalRegistration( vtkSmartPointer< vtkLORPointObservationBuffer > fromPoints )
{
  // Assume that it is already mean zero
  const double CONDITION_THRESHOLD = 1e-3;

  // Let us construct the data matrix
  vnl_matrix<double>* DataMatrix = new vnl_matrix<double>( vtkLORPointObservation::SIZE, vtkLORPointObservation::SIZE, 0.0 );

  // Pick two dimensions, and find their data matrix entry
  for ( int d1 = 0; d1 < vtkLORPointObservation::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkLORPointObservation::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < this->Size(); i++ )
	  {
	    DataMatrix->put( d1, d2, DataMatrix->get( d1, d2 ) + fromPoints->GetObservation(i)->Observation.at(d1) * this->GetObservation(i)->Observation.at(d2) );
	  }
	}
  }

  // Now we can calculate its svd
  vnl_svd<double>* SVDMatrix = new vnl_svd<double>( *DataMatrix, 0.0 );
  if ( SVDMatrix->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error("Failed - spherical registration is ill-conditioned!");
  }

  return new vnl_matrix<double>( SVDMatrix->V() * SVDMatrix->U().transpose() );
}


vnl_matrix<double>* vtkLORPointObservationBuffer
::TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vnl_matrix<double>* rotation )
{
  // Make matrices out of the centroids
  vnl_matrix<double>* toMatrix = new vnl_matrix<double>( vtkLORPointObservation::SIZE, 1, 0.0 );
  vnl_matrix<double>* fromMatrix = new vnl_matrix<double>( vtkLORPointObservation::SIZE, 1, 0.0 );

  for ( int i = 0; i < vtkLORPointObservation::SIZE; i++ )
  {
    toMatrix->put( i, 0, toCentroid.at(i) );
	fromMatrix->put( i, 0, fromCentroid.at(i) );
  }

  return new vnl_matrix<double>( (*toMatrix) - (*rotation) * (*fromMatrix) );
}


vtkSmartPointer< vtkLORLinearObject > vtkLORPointObservationBuffer
::LeastSquaresLinearObject( int dof )
{
  std::vector<double> centroid = this->CalculateCentroid();
  vnl_matrix<double>* cov = this->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( vtkLORPointObservation::SIZE, vtkLORPointObservation::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( vtkLORPointObservation::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Grab only the most important eigenvectors
  std::vector<double> Eigenvector1( vtkLORPointObservation::SIZE, 0.0 ); // Smallest
  std::vector<double> Eigenvector2( vtkLORPointObservation::SIZE, 0.0 ); // Medium
  std::vector<double> Eigenvector3( vtkLORPointObservation::SIZE, 0.0 ); // Largest

  Eigenvector1.at(0) = eigenvectors.get( 0, 0 );
  Eigenvector1.at(1) = eigenvectors.get( 1, 0 );
  Eigenvector1.at(2) = eigenvectors.get( 2, 0 );

  Eigenvector2.at(0) = eigenvectors.get( 0, 1 );
  Eigenvector2.at(1) = eigenvectors.get( 1, 1 );
  Eigenvector2.at(2) = eigenvectors.get( 2, 1 );

  Eigenvector3.at(0) = eigenvectors.get( 0, 2 );
  Eigenvector3.at(1) = eigenvectors.get( 1, 2 );
  Eigenvector3.at(2) = eigenvectors.get( 2, 2 );

  // The threshold noise is twice the extraction threshold
  if ( dof == 0 )
  {
    return vtkSmartPointer< vtkLORPoint >::Take( vtkLORPoint::New( centroid ) );
  }
  if ( dof == 1 )
  {
    return vtkSmartPointer< vtkLORLine >::Take( vtkLORLine::New( centroid, Add( centroid, Eigenvector3 ) ) ); 
  }
  if ( dof == 2 )
  {
    return vtkSmartPointer< vtkLORPlane >::Take( vtkLORPlane::New( centroid, Add( centroid, Eigenvector2 ), Add( centroid, Eigenvector3 ) ) );
  }

  return NULL;
}


void vtkLORPointObservationBuffer
::Filter( vtkSmartPointer< vtkLORLinearObject > object, int filterWidth )
{
  const int THRESHOLD = 1e-3; // Deal with the case of very little noise
  bool changed = true;

  while ( changed )
  {
    std::vector<double> distances( this->Size(), 0 );
	double meanDistance = 0;
	double stdev = 0;

    // Calculate the distance of each point to the linear object
    for ( int i = 0; i < this->Size(); i++ )
	{
      distances.at(i) = object->DistanceToVector( this->GetObservation(i)->Observation );
	  meanDistance = meanDistance + distances.at(i);
	  stdev = stdev + distances.at(i) * distances.at(i);
	}
	meanDistance = meanDistance / this->Size();
	stdev = stdev / this->Size();
	stdev = sqrt( stdev - meanDistance * meanDistance );
    
	// Keep only the points that are within certain number of standard deviations
	std::vector< vtkSmartPointer< vtkLORPointObservation > > newObservations;
	for ( int i = 0; i < this->Size(); i++ )
	{
      if ( distances.at(i) < filterWidth * stdev || distances.at(i) < THRESHOLD )
	  {
        newObservations.push_back( this->GetObservation(i) );
	  }
	}

	if ( newObservations.size() < this->Size() )
	{
      changed = true;
	}
	else
	{
      changed = false;
	}

    this->observations.clear();
	this->observations = newObservations;

  }
  
}


std::string vtkLORPointObservationBuffer
::ToXMLString()
{
  std::stringstream xmlstring;

  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetObservation(i)->ToXMLString();
  }

  return xmlstring.str();
}


void vtkLORPointObservationBuffer
::FromXMLElement( vtkSmartPointer< vtkXMLDataElement > element )
{
  vtkSmartPointer< vtkLORPointObservation > blankObservation;
  this->observations = std::vector< vtkSmartPointer< vtkLORPointObservation > >( 0, blankObservation );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {
    vtkSmartPointer< vtkXMLDataElement > noteElement = element->GetNestedElement( i );

    vtkSmartPointer< vtkLORPointObservation > newObservation = vtkSmartPointer< vtkLORPointObservation >::New();
	newObservation->FromXMLElement( noteElement );
    this->AddObservation( newObservation );
  }

}


vnl_matrix<double>* vtkLORPointObservationBuffer
::CovarianceMatrix( std::vector<double> centroid )
{
  // Construct a buffer for the zero mean data; initialize covariance matrix
  vtkSmartPointer< vtkLORPointObservationBuffer > zeroMeanBuffer = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
  vnl_matrix<double> *cov = new vnl_matrix<double>( vtkLORPointObservation::SIZE, vtkLORPointObservation::SIZE );
  cov->fill( 0.0 );

  // Subtract the mean from each observation
  for ( int i = 0; i < this->Size(); i++ )
  {
    vtkSmartPointer< vtkLORPointObservation > newObservation = vtkSmartPointer< vtkLORPointObservation >::New();
    for( int d = 0; d < vtkLORPointObservation::SIZE; d++ )
	{
	  newObservation->Observation.push_back( this->GetObservation(i)->Observation.at(d) - centroid.at(d) );
	}
	zeroMeanBuffer->AddObservation( newObservation );
  }

  // Pick two dimensions, and find their covariance
  for ( int d1 = 0; d1 < vtkLORPointObservation::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < vtkLORPointObservation::SIZE; d2++ )
	{
	  // Iterate over all times
	  for ( int i = 0; i < this->Size(); i++ )
	  {
	    cov->put( d1, d2, cov->get( d1, d2 ) + zeroMeanBuffer->GetObservation(i)->Observation.at(d1) * zeroMeanBuffer->GetObservation(i)->Observation.at(d2) );
	  }
	  // Divide by the number of records
	  cov->put( d1, d2, cov->get( d1, d2 ) / this->Size() );
	}
  }

  return cov;

}


std::vector<double> vtkLORPointObservationBuffer
::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( vtkLORPointObservation::SIZE, 0.0 );
  for ( int i = 0; i < this->Size(); i++ )
  {
	  for ( int d = 0; d < vtkLORPointObservation::SIZE; d++ )
	{
      centroid.at(d) = centroid.at(d) + this->GetObservation(i)->Observation.at(d);
	}
  }
  for ( int d = 0; d < vtkLORPointObservation::SIZE; d++ )
  {
    centroid.at(d) = centroid.at(d) / this->Size();
  }

  return centroid;
}


std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > vtkLORPointObservationBuffer
::ExtractLinearObjects( int collectionFrames, double extractionThreshold, std::vector<int>* dof )
{

  // First, let us identify the segmentation points and the associated DOFs, then we can divide up the points
  int TEST_INTERVAL = 21;

  vtkSmartPointer< vtkLORPointObservationBuffer > eigenBuffer = vtkSmartPointer< vtkLORPointObservationBuffer >::New(); // Note: 1 < 2 < 3
  int currStartIndex, currEndIndex;
  bool collecting = false;

  std::vector< vtkSmartPointer< vtkLORPointObservationBuffer > > linearObjects;

  // Note: i is the start of the interval over which we will exam for linearity
  for ( int i = 0; i < this->Size() - TEST_INTERVAL; i++ )
  {
    // Create a smaller point observation buffer to work with at each iteration with the points of interest
    vtkSmartPointer< vtkLORPointObservationBuffer > tempBuffer = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
    for ( int j = i; j < i + TEST_INTERVAL; j++ )
	{
      tempBuffer->AddObservation( this->GetObservation(j) );
	}

	// Find the eigenvalues of covariance matrix
    std::vector<double> centroid = tempBuffer->CalculateCentroid();
    vnl_matrix<double>* cov = tempBuffer->CovarianceMatrix( centroid );

    //Calculate the eigenvectors of the covariance matrix
    vnl_matrix<double> eigenvectors( vtkLORPointObservation::SIZE, vtkLORPointObservation::SIZE, 0.0 );
    vnl_vector<double> eigenvalues( vtkLORPointObservation::SIZE, 0.0 );
    vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
    // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

	std::vector<double> eigen( 3, 0.0 );
	eigen.at(0) = eigenvalues.get( 0 );
	eigen.at(1) = eigenvalues.get( 1 );
	eigen.at(2) = eigenvalues.get( 2 );
    eigenBuffer->AddObservation( vtkSmartPointer< vtkLORPointObservation >::Take( vtkLORPointObservation::New( eigen ) ) );


	if ( ! collecting )
	{
      currStartIndex = i;
	}

	if ( eigenvalues.get( 0 ) < extractionThreshold )
	{
      collecting = true;
	  continue;
	}
	collecting = false;
	currEndIndex = i;

	// Suppose that we have reached the end of a collecting section
	// If its too short then skip
	if ( currEndIndex - currStartIndex < collectionFrames )
	{
      continue;
	}

	
	// Now search for the largest interval of points which has the fewest DOF and satisfies the minimum interval
	for ( int e = vtkLORPointObservation::SIZE - 1; e >= 0; e-- )
	{
	  // Find the intervals where the eigenvalue is less than the threshold
	  std::vector<int> dofInterval;
	  dofInterval.push_back( currStartIndex );
	  for ( int j = currStartIndex; j < currEndIndex; j++ )
	  {
        if ( eigenBuffer->GetObservation(j)->Observation.at(e) > extractionThreshold )
	    {
          dofInterval.push_back(j);
	    }
	  }
      dofInterval.push_back( currEndIndex );

	  // Find the longest such interval
	  int maxIntervalLength = 0;
	  int maxIntervalIndex = 0;
	  for ( int j = 0; j < dofInterval.size() - 1; j++ )
	  {
        if ( dofInterval.at(j+1) - dofInterval.at(j) > maxIntervalLength )
	    {
          maxIntervalLength = dofInterval.at(j+1) - dofInterval.at(j);
		  maxIntervalIndex = j;
	    }
	  }

	  // If the longest interval is too short, then ignore
      if ( maxIntervalLength < collectionFrames )
	  {
        continue; 
	  }

	  // Otherwise, this is a collected linear object
      vtkSmartPointer< vtkLORPointObservationBuffer > foundBuffer = vtkSmartPointer< vtkLORPointObservationBuffer >::New();
	  for ( int j = dofInterval.at(maxIntervalIndex); j < dofInterval.at( maxIntervalIndex + 1 ); j++ )
	  {
        foundBuffer->AddObservation( this->GetObservation( j + TEST_INTERVAL ) );
	  }

      linearObjects.push_back( foundBuffer );
	  dof->push_back( vtkLORPointObservation::SIZE - 1 - e );
	  break;

    }

  }

  return linearObjects;

}