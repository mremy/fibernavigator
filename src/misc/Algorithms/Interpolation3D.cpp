
#include "Interpolation3D.h"

#include "../../Logger.h"
#include <sstream>
using std::stringstream;
#include "../IsoSurface/Vector.h"
#include <cmath>
#include <vector>

bool Interpolation3D::linear3D(float *curve, unsigned int N, float *newCurve, unsigned int newN)
{

    float        l_currentFiberRatio    = (float)( N ) / ( newN );
    //Keep first and last points the same
    newCurve[0]              = curve[0];
	newCurve[1]              = curve[1];
	newCurve[2]              = curve[2];
    newCurve[newN*3 - 1] = curve[N*3 - 1] ;
	newCurve[newN*3 - 2] = curve[N*3 - 2] ;
	newCurve[newN*3 - 3] = curve[N*3 - 3] ;

	//For each point of current streamline, resample the fibers to inbPoints
	int cur = 1;
    for( unsigned int j = 3; j < newN*3 - 5; j+=3 )
    {
        float l_currentPointInterpolationRatio = l_currentFiberRatio * cur;
             
        // Calculating the ratio of the interpolation.
        int l_pointBelow = (int)l_currentPointInterpolationRatio;
        l_currentPointInterpolationRatio -= l_pointBelow;
        // Simple interpolation.
        newCurve[j] = ( ( curve[l_pointBelow*3]     * ( 1.0 - l_currentPointInterpolationRatio ) ) + 
                                ( curve[l_pointBelow*3 + 3] *         l_currentPointInterpolationRatio ) ); 
		newCurve[j+1] = ( ( curve[l_pointBelow*3+1]     * ( 1.0 - l_currentPointInterpolationRatio ) ) + 
                                ( curve[l_pointBelow*3 + 4] *         l_currentPointInterpolationRatio ) );  
		newCurve[j+2] = ( ( curve[l_pointBelow*3+2]     * ( 1.0 - l_currentPointInterpolationRatio ) ) + 
                                ( curve[l_pointBelow*3 + 5] *         l_currentPointInterpolationRatio ) );
		cur++;
	}

	//for debug
	//std::vector<Vector> v;
	//v.resize(newN);
	//std::vector<int> id;
	//for(int i=0; i<newN; i++)
	//{
	//	if(newCurve[i*3] > 1000)
	//		id.push_back(i);
	//	v[i] = Vector(newCurve[i*3+0],newCurve[i*3+1],newCurve[i*3+2]);
	//}

    return true;
}