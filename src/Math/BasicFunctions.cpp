#include <Math/BasicFunctions.h>

IMPGEARS_BEGIN

double randHugosElias(int t)
{
	t = (t << 13) ^ t;
	t = (t * (t * t * 15731 + 789221) + 1376312589);
	return 1.0 - (t & 0x7fffffff) / 1073741824.0;
}

void intFrac(double t, int& integer, double& fraction)
{
	integer = (int)t;
	if(t<0)
		integer -= 1;

	fraction = t-integer;
}

double Hermite(double t)
{
	double t2 = t*t;
	return 3*t2 - 2*t*t2;
}

double Quintic(double t)
{
	double t3 = t*t*t;
	double t4 = t*t3;
	return 6*t*t4 - 15*t4 + 10*t3;
}

double Lerp(double a, double b, double t)
{
	return t*(b-a) + a;
}

IMPGEARS_END
