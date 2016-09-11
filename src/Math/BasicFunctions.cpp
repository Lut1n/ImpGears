#include <Math/BasicFunctions.h>
#include <cmath>

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

double Sin(double t, double maxPeriodRatio)
{
	const double PI = 3.141592;
	const double PERIOD_LENGTH = 2.0*PI;

	int periodCount;
	double periodPos;
	intFrac( t/PERIOD_LENGTH, periodCount, periodPos );

	if(periodPos <= maxPeriodRatio)
	{
		periodPos = Lerp( 0.0, 0.5, (periodPos/maxPeriodRatio) );
	}
	else
	{
		periodPos = Lerp( 0.5, 1.0, ((periodPos-maxPeriodRatio)/(1.0-maxPeriodRatio)) );
	}

	t = (periodCount + periodPos) * PERIOD_LENGTH;

	return sin(t);
}

double SquareSignal(double t, double maxPeriodRatio)
{
	const double PI = 3.141592;
	const double PERIOD_LENGTH = 2.0*PI;

	int periodCount;
	double periodPos;
	intFrac( t/PERIOD_LENGTH, periodCount, periodPos );

	double result = 1.0;

	if(periodPos > maxPeriodRatio)
		result = -1.0;

	return result;
}

double TriangleSignal(double t, double maxPeriodRatio)
{
	const double PI = 3.141592;
	const double PERIOD_LENGTH = 2.0*PI;

	int periodCount;
	double periodPos;
	intFrac( t/PERIOD_LENGTH, periodCount, periodPos );

	double result = 1.0;

	double minPeriodRatio = 1.0 - maxPeriodRatio;
	double p0 = 0.0;
	double p1 = maxPeriodRatio/2.0;
	double p2 = maxPeriodRatio;
	double p3 = maxPeriodRatio+(minPeriodRatio/2.0);
	double p4 = 1.0;

	if(periodPos <= p1)
		result = Lerp(0.0, 1.0, ( (periodPos-p0)/(p1-p0) ) );
	else if(periodPos <= p2)
		result = Lerp(1.0, 0.0, ( (periodPos-p1)/(p2-p1) ) );
	else if(periodPos <= p3)
		result = Lerp(0.0, -1.0, ( (periodPos-p2)/(p3-p2) ) );
	else
		result = Lerp(-1.0, 0.0, ( (periodPos-p3)/(p4-p3) ) );

	return result;
}

IMPGEARS_END
