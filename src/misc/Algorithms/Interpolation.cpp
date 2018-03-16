
#include "Interpolation.h"

#include "../../Logger.h"
#include <sstream>
using std::stringstream;

#include <cmath>

bool Interpolation::linear3D(float *curve, unsigned int N, float *newCurve, unsigned int newN)
{
    float* cumlen = new float[N];
    cumlen[0] = 0.0;
    int idx = 0;
    stringstream ss;
    for( unsigned int i = 0; i < (N-1)*3; i+=3 )
    {
        float dx = curve[(i+3)]   - curve[i];
        float dy = curve[(i+3)+1] - curve[i+1];
        float dz = curve[(i+3)+2] - curve[i+2];
        float length = sqrt(dx*dx + dy*dy + dz*dz);
        
        cumlen[idx+1] = cumlen[idx] + length;
        ++idx;
    }

    float step = cumlen[N-1] / float(newN);

    /*ss.str( "" );
    ss << "Step: " << step;
    Logger::getInstance()->print( wxString( ss.str().c_str(), wxConvUTF8 ), LOGLEVEL_MESSAGE );*/

    float nextPoint = 0.0;
    int i = 0;
    int j = 0;
    for( unsigned int k = 0; k < N-1; ++k )
    {
        if (cumlen[k] == nextPoint)
        {

            /*ss.str( "" );
            ss << "i " << i;
            ss << "j " << j;
            Logger::getInstance()->print( wxString( ss.str().c_str(), wxConvUTF8 ), LOGLEVEL_MESSAGE );*/

            newCurve[i]   = curve[j];
            newCurve[i+1] = curve[j+1];
            newCurve[i+2] = curve[j+2];

            i += 3;
            nextPoint += step;
        }
        else if (cumlen[k] > nextPoint)
        {

            /*ss.str( "" );
            ss << "i " << i;
            ss << "j " << j;
            Logger::getInstance()->print( wxString( ss.str().c_str(), wxConvUTF8 ), LOGLEVEL_MESSAGE );*/
            
            float ratio = 1 - ((cumlen[k]-nextPoint) / (cumlen[k]-cumlen[k-1]));

            newCurve[i]   = curve[j-3]     + ratio * (curve[j]   - curve[j-3]);
            newCurve[i+1] = curve[(j-3)+1] + ratio * (curve[j+1] - curve[(j-3)+1]);
            newCurve[i+2] = curve[(j-3)+2] + ratio * (curve[j+2] - curve[(j-3)+2]);

            i += 3;
            nextPoint += step;
        }

        j += 3;

        if( i/3. >= newN )
            break;
    }

    newCurve[(newN-1)*3]   = curve[(N-1)*3];
    newCurve[(newN-1)*3+1] = curve[(N-1)*3+1];
    newCurve[(newN-1)*3+2] = curve[(N-1)*3+2];

    delete[] cumlen;

    return true;
}