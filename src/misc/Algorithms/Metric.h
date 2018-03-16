#ifndef DISTANCES_H_
#define DISTANCES_H_

#include "../../dataset/Fibers.h"

#include <fstream>
#include <string>
#include <vector>

class Metric;

class Cluster
{
public:
    Cluster( Metric *pMetric );

    float distance( const float *pStreamline, unsigned int N, unsigned int D=3 );
    void add( unsigned int idx, const float *pStreamline, unsigned int N, unsigned int D=3 );

    std::vector<unsigned int> m_indices;
    std::vector<float> m_centroid;

private:
    Metric *m_pMetric;
    friend class Metric;

};


/**
 * This class represents an abstract distance measure.
 */
class Metric
{
public:
    Metric() {}
    virtual ~Metric() {}

    virtual vector<float> newCentroid( unsigned int N, unsigned int D ) = 0;
    virtual float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D ) = 0;
    virtual float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D ) = 0;
    virtual void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D ) = 0;

private:
    Metric( const Metric & );
    Metric &operator=( const Metric & );    
};

/**
 * This class combines several metrics using a weigthted sum.
 */
class WeightedMetric : public Metric
{
public:
    WeightedMetric() {}
    virtual ~WeightedMetric() {}

    void addMetric( Metric *pMetric, float weight=1.0 );
    void removeMetric( unsigned int idxMetric );
    void updateMetric( unsigned int idxMetric, float weight );

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );

private:
    vector<float>       m_weights;
    vector<Metric*>     m_metrics;
    vector<unsigned int>        m_centroidsStart;
};


/**
 * This class computes the average pointwise distance (aka. MDF) between two streamlines.
 */
class MDF : public Metric
{
public:
    MDF() {}
    virtual ~MDF() {}

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );
};

/**
 * This class computes the geometrical shape distance between two streamlines using
 *  the pointwise angular difference.
 */
class ShapeMetric : public Metric
{
public:
    ShapeMetric() {}
    virtual ~ShapeMetric() {}

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );
};

/**
 * This class computes the orientation distance between two streamlines using
 *  the cosine distance between the two vectors linking streamlines' endpoints.
 */
class OrientationMetric : public Metric
{
public:
    OrientationMetric() {}
    virtual ~OrientationMetric() {}

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );
};

/**
 * This class computes the spatial distance between two streamlines using
 *  the Euclidean distance between streamlines' midpoint.
 */
class SpatialMetric : public Metric
{
public:
    SpatialMetric() {}
    virtual ~SpatialMetric() {}

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );
};

/**
 * This class computes the length distance between two streamlines using
 *  the streamlines' arclength.
 */
class LengthMetric : public Metric
{
public:
    LengthMetric() {}
    virtual ~LengthMetric() {}

    vector<float> newCentroid( unsigned int N, unsigned int D );
    float distance( const float *pStreamline1, unsigned int N1, const float *pStreamline2, unsigned int N2, unsigned int D );
    float distance( float *pCentroid, const float *pStreamline, unsigned int N, unsigned int D );
    void add( float *pCentroid, unsigned int clusterSize, const float *pStreamline, unsigned int N, unsigned int D );
};

#endif /* DISTANCES_H_ */
