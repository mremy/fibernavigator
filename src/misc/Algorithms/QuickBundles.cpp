#include "QuickBundles.h"

#include "../../Logger.h"
#include "Interpolation.h"
#include <sstream>

#include <algorithm>    // std::random_shuffle

using std::stringstream;

QuickBundles::QuickBundles(const vector<Fibers*>& bundles, Metric *pMetric, float threshold, unsigned int nbDownsamplingPts)
:m_pMetric(pMetric), m_threshold(threshold), m_nbDownsamplingPts(nbDownsamplingPts)
{
    stringstream ss;
    for( vector<Fibers *>::const_iterator itBundle = bundles.begin(); itBundle != bundles.end(); ++itBundle )
    {
        vector<float>* pPoints       = &((*itBundle)->m_pointArray);
        vector<int>*   pLinePointers = &((*itBundle)->m_linePointers);
        vector<float>* pPointsColor  = &((*itBundle)->m_colorArray);

        for( int i=0; i < (*itBundle)->getFibersCount(); ++i )
        {
            int idx = ((*pLinePointers)[i]);
            m_fibersLines.push_back(  &((*pPoints)[idx*3]) );
            m_fibersLength.push_back( (*pLinePointers)[i+1] - idx );
            m_fibersColors.push_back( &((*pPointsColor)[idx*3]) );
        }
    }

    // List of the index of all fibers.
    for( unsigned int i=0; i < m_fibersLines.size(); ++i )
    {
        m_indices.push_back(i);
    }

    // TODO: The downsampling is done each time we want to cluster... ouch!
    // Downsample all streamlines so they have the same number of points.
    m_pPoints = new float[m_fibersLength.size() * m_nbDownsamplingPts * 3];

    for( unsigned int i=0; i < m_fibersLength.size(); ++i )
    {
        m_line2point.push_back(i * m_nbDownsamplingPts * 3);
        Interpolation::linear3D(m_fibersLines[i], m_fibersLength[i], &m_pPoints[m_line2point.back()], m_nbDownsamplingPts);
    }

    cluster();
}

QuickBundles::~QuickBundles()
{
    delete[] m_pPoints;
}

void QuickBundles::cluster()
{
    stringstream ss;
    ss << "Computing QuickBundles using a threshold of " << m_threshold;
    Logger::getInstance()->print( wxString( ss.str().c_str(), wxConvUTF8 ), LOGLEVEL_DEBUG );

    int i_k = -1;
    float dist_min, dist_ck_si;
    float* pStreamline;

    // Shuffle index inplace since QuickBundles is sensitive to initialization
    std::random_shuffle(m_indices.begin(), m_indices.end());

    // Assign each streamlines to a cluster
    for( unsigned int i=0; i < m_indices.size(); ++i )
    {
        unsigned int idx = m_indices[i];
        pStreamline = &m_pPoints[m_line2point[idx]];

        // Find closest cluster
        dist_min = BIGGEST_FLOAT;
        for( unsigned int k=0; k < m_clusters.size(); ++k )
        {
            dist_ck_si = m_clusters[k].distance( pStreamline, m_nbDownsamplingPts );

            // Keep track of the closest cluster
            if( dist_ck_si < dist_min )
            {
                dist_min = dist_ck_si;
                i_k = k;
            }
        }

        // Check if distance with the closest cluster is below some threshold
        if( dist_min < m_threshold )
        {
            m_clusters[i_k].add( idx, pStreamline, m_nbDownsamplingPts );
        }
        else  // If not, add new cluster
        {
            m_clusters.push_back(Cluster( m_pMetric ));
            m_clusters.back().add( idx, pStreamline, m_nbDownsamplingPts );
        }
    }

    ss.str( "" );
    ss << "QuickBundles done... " << m_clusters.size() << " clusters found";
    Logger::getInstance()->print( wxString( ss.str().c_str(), wxConvUTF8 ), LOGLEVEL_DEBUG );
}

vector<float*> QuickBundles::getPoints( unsigned int idx_cluster )
{
    vector<float*> points;
    if( idx_cluster >= m_clusters.size() )
    {
        return points;
    }

    for (vector<unsigned int>::iterator it = m_clusters[idx_cluster].m_indices.begin(); it != m_clusters[idx_cluster].m_indices.end(); ++it)
    {
        points.push_back(m_fibersLines[*it]);
    }

    return points;
}

vector<int> QuickBundles::getLengths( unsigned int idx_cluster )
{
    vector<int> lengths;
    if( idx_cluster >= m_clusters.size() )
    {
        return lengths;
    }

    for (vector<unsigned int>::iterator it = m_clusters[idx_cluster].m_indices.begin(); it != m_clusters[idx_cluster].m_indices.end(); ++it)
    {
        lengths.push_back(m_fibersLength[*it]);
    }

    return lengths;
}

vector<float*> QuickBundles::getColors( unsigned int idx_cluster )
{
    vector<float*> colors;
    if( idx_cluster >= m_clusters.size() )
    {
        return colors;
    }


    for (vector<unsigned int>::iterator it = m_clusters[idx_cluster].m_indices.begin(); it != m_clusters[idx_cluster].m_indices.end(); ++it)
    {
        colors.push_back(m_fibersColors[*it]);
    }

    return colors;
}
