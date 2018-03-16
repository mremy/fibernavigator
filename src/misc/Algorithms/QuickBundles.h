#ifndef QUICK_BUNDLES_H_
#define QUICK_BUNDLES_H_

#include "../../dataset/Fibers.h"
#include "Metric.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//#include <fstream>
//#include <string>
#include <vector>

const float BIGGEST_FLOAT = 3.4028235e+38;

/**
 * This class represents a clustering result from using QuickBundles algorithm.
 */
class QuickBundles
{
public:
    QuickBundles( const vector<Fibers*>& bundles, Metric *pMetric, float threshold, unsigned int nbDownsamplingPts);
    virtual ~QuickBundles();

    int getNbClusters() const { return m_clusters.size(); }
    vector<float*>  getPoints( unsigned int no_cluster );
    vector<int>     getLengths( unsigned int no_cluster );
    vector<float*>  getColors( unsigned int no_cluster );

private:
    QuickBundles( const QuickBundles & );
    QuickBundles &operator=( const QuickBundles & );

private:
    void cluster();

private:
    // Variables
    Metric *m_pMetric;
    float m_threshold;
    unsigned int m_nbDownsamplingPts;
    vector<Cluster> m_clusters;

    float *m_pPoints;
    vector<int> m_line2point;
    vector<float*> m_fibersLines;
    vector<int> m_fibersLength;
    vector<float*> m_fibersColors;
    vector<int> m_indices;
    
};

#endif /* QUICK_BUNDLES_H_ */
