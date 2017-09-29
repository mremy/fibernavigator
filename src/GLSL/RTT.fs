varying vec4 VaryingTexCoord0;

#include functions.fs

uniform float threshold;
uniform bool useAmp;

varying vec4 myColor;



void main()
{
	vec4 cooloor = myColor;
	float value = myColor.r;
    float newVal;
	if (threshold < 1.0)
		newVal = (value - threshold) / (1.0 - threshold);
	else
		newVal = 1.0;

	if (useAmp)
	{
		if ( useColorMap == 1 )
			cooloor.rgb  = colorMap1( newVal );
		else if ( useColorMap == 2 )
			cooloor.rgb  = colorMap2( newVal );
		else if ( useColorMap == 3 )
			cooloor.rgb  = colorMap3( newVal );
		else if ( useColorMap == 4 )
			cooloor.rgb  = colorMap4( newVal );
        else if ( useColorMap == 5 )
			cooloor.rgb  = colorMap5( newVal );
        else if ( useColorMap == 6 )
			cooloor.rgb  = colorMap6( newVal );
        else if ( useColorMap == 7 )
			cooloor.rgb  = colorMap7( newVal );
		else
			cooloor.rgb = defaultColorMap( newVal );
	}

	gl_FragColor = cooloor;
}
