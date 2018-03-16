/*
 *  The Metric class implementation.
 *
 */

#define PI 3.14159

#include "Metric.h"
#include <math.h> 
#include "../../Logger.h"

using namespace std;


/***  Utils  ***/
float arcLength( const float *pStreamline, unsigned int N, unsigned int D )
{
    float arcLength = 0.0;
    float dx, dy, dz;

    for( unsigned int i=D; i < N*D; i+=D )
    {
        dx = pStreamline[i]   - pStreamline[(i-D)];
        dy = pStreamline[i+1] - pStreamline[(i-D)+1];
        dz = pStreamline[i+2] - pStreamline[(i-D)+2];
        arcLength += sqrt(dx * dx + dy * dy + dz * dz);
    }
	
    return arcLength;
}

float cosineDistance( const float *v1, const float *v2, unsigned int D, bool alwaysPositve=true )
{
    float n_v1 = 0.0;
    float n_v2 = 0.0;
    float d = 0.0;
    for( unsigned int i= 0; i < D; ++i )
    {
        d    += v1[i] * v2[i];
        n_v1 += v1[i] * v1[i];
        n_v2 += v2[i] * v2[i];
    }

    d /= sqrt(n_v1) * sqrt(n_v2);

    if( alwaysPositve )
    {
        return 2 * acos(fabs(d)) / PI;
    }

    return acos(d) / PI;
}

void curvature( const float *pStreamline, unsigned int N, unsigned int D, float *pCurvature, bool isFlip=false )
{
    // Compute a proxy for curvature at each point
    float v1[3], v2[3];
    int idx;

    for( unsigned int i= 0; i < (N-2)*D; i+=3 )
    {
        idx = (isFlip)? (N-3)*D -i : i;
        v1[0] = pStreamline[idx]   - pStreamline[(idx+D)];
        v1[1] = pStreamline[idx+1] - pStreamline[(idx+D)+1];
        v1[2] = pStreamline[idx+2] - pStreamline[(idx+D)+2];

        v2[0] = pStreamline[(idx+2*D)]   - pStreamline[(idx+D)];
        v2[1] = pStreamline[(idx+2*D)+1] - pStreamline[(idx+D)+1];
        v2[2] = pStreamline[(idx+2*D)+2] - pStreamline[(idx+D)+2];

        pCurvature[i/3] = 1 - cosineDistance( v1, v2, D, false );
    }
}

bool isFlip( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int D )
{
    // Check if pStreamline1 is flip compare to the pStreamline2.
    float dx, dy, dz;
    
    dx = pStreamline1[0] - pStreamline2[0];
    dy = pStreamline1[1] - pStreamline2[1];
    dz = pStreamline1[2] - pStreamline2[2];

    float endpointDist = dx*dx + dy*dy + dz*dz;

    dx = pStreamline1[(N1-1)*D]   - pStreamline2[0];
    dy = pStreamline1[(N1-1)*D+1] - pStreamline2[1];
    dz = pStreamline1[(N1-1)*D+2] - pStreamline2[2];

    float endpointDistFlip = dx*dx + dy*dy + dz*dz;

    return endpointDist > endpointDistFlip;
}

float AveragePointwiseDistance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D, bool isFlip=false )
{
    float d = 0.0;
    float dx, dy, dz;
    int idx;

    for( unsigned int i=0; i < N2*D; i+=D )
    {
        idx = (isFlip)? (N2-1)*D -i : i;
        dx = pStreamline1[i]   - pStreamline2[idx];
        dy = pStreamline1[i+1] - pStreamline2[idx+1];
        dz = pStreamline1[i+2] - pStreamline2[idx+2];
        d += sqrt(dx * dx + dy * dy + dz * dz);
    }

    return d / N2;
}


/***  Cluster  ***/
Cluster::Cluster(Metric *pMetric)
:m_pMetric(pMetric)
{}

float Cluster::distance( const float *pStreamline, unsigned int N, unsigned int D )
{
    return m_pMetric->distance( &m_centroid[0], pStreamline, N, D );
}

void Cluster::add( unsigned int idx, const float *pStreamline, unsigned int N, unsigned int D )
{
    if( m_indices.size() == 0 )
    {
        m_centroid = vector<float>( m_pMetric->newCentroid( N, D ) );
    }

    m_pMetric->add( &m_centroid[0], m_indices.size(), pStreamline, N, D );
    m_indices.push_back( idx );
}


/***  WeightedMetric  ***/
void WeightedMetric::addMetric( Metric *pMetric, float weight )
{
    if( weight > 0.0 )
    {
        m_metrics.push_back(pMetric);
        m_weights.push_back(weight);
    }
}

void WeightedMetric::removeMetric( unsigned int idxMetric )
{
    m_metrics.erase(m_metrics.begin() + idxMetric);
    m_weights.erase(m_weights.begin() + idxMetric);
}

void WeightedMetric::updateMetric( unsigned int idxMetric, float weight )
{
    m_weights[idxMetric] = weight;
}

vector<float> WeightedMetric::newCentroid( unsigned int N, unsigned int D )
{
    vector<float> newCentroid;
    m_centroidsStart.push_back( 0 );

    for(vector<Metric*>::const_iterator it = m_metrics.begin(); it != m_metrics.end(); ++it )
    {
        vector<float> newMetricCentroid = (*it)->newCentroid( N, D );
        newCentroid.insert( newCentroid.end(), newMetricCentroid.begin(), newMetricCentroid.end() );
        m_centroidsStart.push_back( m_centroidsStart.back() + newMetricCentroid.size() );
    }

    return newCentroid;
}

float WeightedMetric::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    float dist = 0.0;

    for( unsigned int i= 0; i < m_metrics.size(); ++i )
    {
        float metricDist = m_metrics[i]->distance( pStreamline1, N1, pStreamline2, N2, D );
        dist += m_weights[i] * metricDist;
    }
    
    return dist;
}

float WeightedMetric::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    float dist = 0.0;

    for( unsigned int i= 0; i < m_metrics.size(); ++i )
    {
        float metricDist = m_metrics[i]->distance( &pCentroid[m_centroidsStart[i]], pStreamline, N, D );
        dist += m_weights[i] * metricDist;
    }
    
    return dist;
}

void WeightedMetric::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    for( unsigned int i= 0; i < m_metrics.size(); ++i )
    {
        m_metrics[i]->add( &pCentroid[m_centroidsStart[i]], clusterSize, pStreamline, N, D );
    }
}


/***  MDF  ***/
vector<float> MDF::newCentroid( unsigned int N, unsigned int D )
{
    return vector<float>(N*D, 0.0);
}

float MDF::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    float dist      = AveragePointwiseDistance( pStreamline1, N1, pStreamline2, N2, D, false );
    float distFlip  = AveragePointwiseDistance( pStreamline1, N1, pStreamline2, N2, D, true );
    
    return min(dist, distFlip);
}

float MDF::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    float dist      = AveragePointwiseDistance( pCentroid, N, pStreamline, N, D, false );
    float distFlip  = AveragePointwiseDistance( pCentroid, N, pStreamline, N, D, true );
    
    return min(dist, distFlip);
}

void MDF::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    float dist      = AveragePointwiseDistance( pCentroid, N, pStreamline, N, D, false );
    float distFlip  = AveragePointwiseDistance( pCentroid, N, pStreamline, N, D, true );

    bool isFlip = dist > distFlip;
    unsigned int idx;

    for( unsigned int i=0; i < N*D; i+=D )
    {
        idx = (isFlip)? (N-1)*D -i : i;
        pCentroid[i]   = ((pCentroid[i]   * clusterSize) + pStreamline[idx])   / (clusterSize+1);
        pCentroid[i+1] = ((pCentroid[i+1] * clusterSize) + pStreamline[idx+1]) / (clusterSize+1);
        pCentroid[i+2] = ((pCentroid[i+2] * clusterSize) + pStreamline[idx+2]) / (clusterSize+1);
    }
}


/***  ShapeMetric  ***/
vector<float> ShapeMetric::newCentroid( unsigned int N, unsigned int D )
{
    return vector<float>(N-2, 0.0);
}

float ShapeMetric::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    float dk;
    float dist = 0.0, distFlip = 0.0;
    float *pCurvature1 = new float[N1-2];
    float *pCurvature1Flip = new float[N1-2];
    float *pCurvature2 = new float[N2-2];

    curvature( pStreamline1, N1, D, pCurvature1,     false);
    curvature( pStreamline1, N1, D, pCurvature1Flip, true);
    curvature( pStreamline2, N2, D, pCurvature2,     false);

    for( unsigned int i= 0; i < N1-2; ++i )
    {
        dk = pCurvature1[i] - pCurvature2[i];
        dist += log(1 + sqrt(fabs(dk)));

        dk = pCurvature1Flip[i] - pCurvature2[i];
        distFlip += log(1 + sqrt(fabs(dk)));
    }

    delete[] pCurvature1;
    delete[] pCurvature1Flip;
    delete[] pCurvature2;

    return min(dist, distFlip);
}

float ShapeMetric::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    float dk;
    float dist = 0.0, distFlip = 0.0;
    float *pCurvature = new float[N-2];
    float *pCurvatureFlip = new float[N-2];
    curvature( pStreamline, N, D, pCurvature,     false);
    curvature( pStreamline, N, D, pCurvatureFlip, true);

    for( unsigned int i= 0; i < N-2; ++i )
    {
        dk = pCurvature[i] - pCentroid[i];
        dist += log(1 + sqrt(fabs(dk)));

        dk = pCurvatureFlip[i] - pCentroid[i];
        distFlip += log(1 + sqrt(fabs(dk)));
    }

    delete[] pCurvature;
    delete[] pCurvatureFlip;

    return min(dist, distFlip);
}

void ShapeMetric::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    float dk;
    float dist = 0.0, distFlip = 0.0;
    float *pCurvature = new float[N-2];
    float *pCurvatureFlip = new float[N-2];
    curvature( pStreamline, N, D, pCurvature,     false);
    curvature( pStreamline, N, D, pCurvatureFlip, true);

    for( unsigned int i= 0; i < N-2; ++i )
    {
        dk = pCurvature[i] - pCentroid[i];
        dist += log(1 + sqrt(fabs(dk)));

        dk = pCurvatureFlip[i] - pCentroid[i];
        distFlip += log(1 + sqrt(fabs(dk)));
    }

    bool isFlip = dist > distFlip;
    unsigned int idx;

    for( unsigned int i= 0; i < N-2; ++i )
    {
        idx = (isFlip)? (N-3) -i : i;
        pCentroid[i] = ((pCentroid[i] * clusterSize) + pCurvature[idx]) / (clusterSize+1);
    }

    delete[] pCurvature;
    delete[] pCurvatureFlip;
}


/***  OrientationMetric  ***/
vector<float> OrientationMetric::newCentroid( unsigned int N, unsigned int D )
{
    return vector<float>(3, 0.0);
}

float OrientationMetric::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    float v1[3], v2[3];

    v1[0] = pStreamline1[0] - pStreamline1[(N1-1)*D];
    v1[1] = pStreamline1[1] - pStreamline1[(N1-1)*D+1];
    v1[2] = pStreamline1[2] - pStreamline1[(N1-1)*D+2];

    v2[0] = pStreamline2[(N2-1)*D]   - pStreamline2[0];
    v2[1] = pStreamline2[(N2-1)*D+1] - pStreamline2[1];
    v2[2] = pStreamline2[(N2-1)*D+2] - pStreamline2[2];
    
    return cosineDistance( v1, v2, D );
}

float OrientationMetric::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    float v[3];
    v[0] = pStreamline[(N-1)*D]   - pStreamline[0];
    v[1] = pStreamline[(N-1)*D+1] - pStreamline[1];
    v[2] = pStreamline[(N-1)*D+2] - pStreamline[2];

    return cosineDistance( v, &pCentroid[0], D );
}

void OrientationMetric::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    float v[3], vFlip[3];
    v[0] = pStreamline[(N-1)*D]   - pStreamline[0];
    v[1] = pStreamline[(N-1)*D+1] - pStreamline[1];
    v[2] = pStreamline[(N-1)*D+2] - pStreamline[2];

    vFlip[0] = pStreamline[0] - pStreamline[(N-1)*D];
    vFlip[1] = pStreamline[1] - pStreamline[(N-1)*D+1];
    vFlip[2] = pStreamline[2] - pStreamline[(N-1)*D+2];

    float dist     = fabs(v[0]     - pCentroid[0]) + fabs(v[1]     - pCentroid[1]) + fabs(v[2]     - pCentroid[2]);
    float distFlip = fabs(vFlip[0] - pCentroid[0]) + fabs(vFlip[1] - pCentroid[1]) + fabs(vFlip[2] - pCentroid[2]);

    if( dist > distFlip )
    {
        pCentroid[0] = ((pCentroid[0] * clusterSize) + vFlip[0]) / (clusterSize+1);
        pCentroid[1] = ((pCentroid[1] * clusterSize) + vFlip[1]) / (clusterSize+1);
        pCentroid[2] = ((pCentroid[2] * clusterSize) + vFlip[2]) / (clusterSize+1);
    }
    else
    {
        pCentroid[0] = ((pCentroid[0] * clusterSize) + v[0]) / (clusterSize+1);
        pCentroid[1] = ((pCentroid[1] * clusterSize) + v[1]) / (clusterSize+1);
        pCentroid[2] = ((pCentroid[2] * clusterSize) + v[2]) / (clusterSize+1);
    }
}


/***  SpatialMetric  ***/
vector<float> SpatialMetric::newCentroid( unsigned int N, unsigned int D )
{
    return vector<float>(3, 0.0);
}

float SpatialMetric::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    // Midpoint
    int idx1 = int(N1 / 2) * D;
    int idx2 = int(N2 / 2) * D;

    float dx = pStreamline1[idx1]   - pStreamline2[idx2];
    float dy = pStreamline1[idx1+1] - pStreamline2[idx2+1];
    float dz = pStreamline1[idx1+2] - pStreamline2[idx2+2];

    return sqrt( dx*dx + dy*dy + dz*dz );
}

float SpatialMetric::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    // Midpoint
    int idx = int(N / 2) * D;

    float dx = pStreamline[idx]   - pCentroid[0];
    float dy = pStreamline[idx+1] - pCentroid[1];
    float dz = pStreamline[idx+2] - pCentroid[2];

    return sqrt( dx*dx + dy*dy + dz*dz );
}

void SpatialMetric::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    // Midpoint
    int idx = int(N / 2) * D;

    pCentroid[0] = ((pCentroid[0] * clusterSize) + pStreamline[idx])   / (clusterSize+1);
    pCentroid[1] = ((pCentroid[1] * clusterSize) + pStreamline[idx+1]) / (clusterSize+1);
    pCentroid[2] = ((pCentroid[2] * clusterSize) + pStreamline[idx+2]) / (clusterSize+1);
}


/***  LengthMetric  ***/
vector<float> LengthMetric::newCentroid( unsigned int N, unsigned int D )
{
    return vector<float>(1, 0.0);
}

float LengthMetric::distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D )
{
    float length1 = arcLength( pStreamline1, N1, D );
    float length2 = arcLength( pStreamline2, N2, D );
    
    return fabs( length1 - length2 );
}

float LengthMetric::distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D )
{
    float length = arcLength( pStreamline, N, D );
    
    return fabs( length - pCentroid[0] );
}

void LengthMetric::add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D )
{
    float length = arcLength( pStreamline, N, D );

    pCentroid[0] = ((pCentroid[0] * clusterSize) + length) / (clusterSize+1);
}
