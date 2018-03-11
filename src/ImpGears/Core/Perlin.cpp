#include <Core/Perlin.h>
#include <Core/Math.h>

#include <ctime>
#include <cstdlib>
#include <iostream>

/**
 * Simplex noise source : "Simplex noise demystified - Stefan Gustavson - 2005" (http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf)
 */

IMPGEARS_BEGIN

class IMP_API PermutationTable
{
public:
	PermutationTable()
		: _table(0)
		, _size(0)
		, _initialized(false)
	{}

	virtual ~PermutationTable()
	{
		if(_table)
			delete [] _table;
	}

	void init(int seed = 0, unsigned int uniqueValueCount = 256, unsigned int repeat = 2);

	unsigned char hash(int x)
	{
		return _table[x];
	}

	unsigned char hash(int x, int y)
	{
		return _table[ y + _table[x] ];
	}

	unsigned char hash(int x, int y, int z)
	{
		return _table[ z + _table[ y + _table[x] ] ];
	}

	static PermutationTable* getInstance()
	{
		if(_instance == 0)
			_instance = new PermutationTable();

		return _instance;
	}
	
	template<typename T>
	static void swap(T* a, T* b)
	{
		T t = *a;
		*a = *b;
		*b = t;
	}

private:

	static PermutationTable* _instance;

	unsigned char* _table;
	int _seed;
	int _size;
	bool _initialized;
};


PermutationTable* PermutationTable::_instance = 0;


void PermutationTable::init(int seed, unsigned int uniqueValueCount, unsigned int repeat)
{
	if(_initialized)
		return;

	if(_table)
		delete [] _table;

	_size = uniqueValueCount * repeat;
	_table = new unsigned char[_size];
	_seed = seed;


	std::srand(_seed);
	
	// generation
	for(unsigned int i=0; i<uniqueValueCount; ++i)
		_table[i] = i;

	// permutation pseudo random
	for(unsigned int i=0; i<uniqueValueCount; ++i)
	{
		int x = i;
		int y = Rand(0.0, 1.0) * 255;
		swap( &(_table[x]), &(_table[y]) );
	}

	// repeat
	for(unsigned int i=0; i<uniqueValueCount; ++i)
	{
		// for(int j=1; j<repeat; ++j)
			_table[i+1*uniqueValueCount] = _table[i];
	}

	_initialized = true;
}


// gradient array
static int s_grad3[16][3] = {
	// cube
	{1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,1},
	{1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
	{0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1},
	// + 1 tetraedron
	{1,1,0}, {-1,1,0}, {0,-1,1}, {0,-1,-1}
};

int fastfloor(double x)
{
	int xi = (int)x;
	return x<xi ? xi-1 : xi;
}

int* randomGrad(int x, int y, int z)
{
	unsigned char rvalue = PermutationTable::getInstance()->hash(x,y, z);
	return s_grad3[rvalue % 12];
}

double perlinDot(int* vec, double x, double y)
{
	return vec[0]*x + vec[1]*y;
}

double perlinDot(int* vec, double x, double y, double z)
{
	return vec[0]*x + vec[1]*y + vec[2]*z;
}

double perlinDot(int* vec, double x, double y, double z, double w)
{
	return vec[0]*x + vec[1]*y + vec[2]*z + vec[3]*w;
}

double perlinMain(double x, double y, double z, int tileSize)
{
	PermutationTable::getInstance()->init(0);
	
	int X = std::floor(x);
	int Y = std::floor(y);
	int Z = std::floor(z);
	
	double fx = Frac(x);
	double fy = Frac(y);
	double fz = Frac(z);

	// do a modulo 256 on X, Y, Z.
	X &= 255;
	Y &= 255;
	Z &= 255;
	
const  double g000 = perlinDot(randomGrad(X%tileSize, Y%tileSize, Z%tileSize),
fx, fy, fz);
const double g001 = perlinDot(randomGrad(X%tileSize, Y%tileSize, (Z + 1)%tileSize),
fx, fy, fz - 1.);
const double g010 = perlinDot(randomGrad(X%tileSize, (Y + 1)%tileSize, Z%tileSize),
fx, fy - 1., fz);
const double g011 = perlinDot(randomGrad(X%tileSize, (Y + 1)%tileSize, (Z + 1)%tileSize),
fx, fy - 1., fz - 1.);
const double g100 = perlinDot(randomGrad((X + 1)%tileSize, Y%tileSize, Z%tileSize),
fx - 1., fy, fz);
const double g101 = perlinDot(randomGrad((X + 1)%tileSize, Y%tileSize, (Z + 1)%tileSize),
fx - 1., fy, fz - 1.);
const double g110 = perlinDot(randomGrad((X + 1)%tileSize, (Y + 1)%tileSize, Z%tileSize),
fx - 1., fy - 1., fz);
const double g111 = perlinDot(randomGrad((X + 1)%tileSize, (Y + 1)%tileSize, (Z + 1)%tileSize),
fx - 1., fy - 1., fz - 1.);

	double u = Quintic(fx);
	double v = Quintic(fy);
	double w = Quintic(fz);

	double X00 = Lerp(g000, g100, u);
	double X10 = Lerp(g010, g110, u);
	double X01 = Lerp(g001, g101, u);
	double X11 = Lerp(g011, g111, u);

	double xy0 = Lerp(X00, X10, v);
	double xy1 = Lerp(X01, X11, v);

	double xyz = Lerp(xy0, xy1, w);

	return xyz;
}

// simplex 2D
double simplexMain(double x, double y, int tileSize)
{
	PermutationTable::getInstance()->init(0);
	
	// determine which simplex cell we are in
	double F2 = 0.5*(sqrtf(3.0)-1.0);
	double s = (x+y)*F2;
	int i = fastfloor(x+s);
	int j = fastfloor(y+s);
	
	double G2 = (3.0-sqrtf(3.0))/6.0;
	double t = (i+j)*G2;
	double X0 = i-t;
	double Y0 = j-t;
	double x0 = x-X0;
	double y0 = y-Y0;
	
	int i1,j1;
	if(x0>y0){i1=1; j1=0;}
	else {i1=0; j1=1;}
	
	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0*G2;
	double y2 = y0 - 1.0 + 2.0*G2;
	
	int ii = i & 255;
	int jj = j & 255;
	
	// contributions of 3 corners (simplex 2D = triangle)
	double contrib0, contrib1, contrib2;
	double t0 = 0.5 - x0*x0-y0*y0;
	if(t0<0) contrib0 = 0.0;
	else {
		t0 *= t0;
		contrib0 = t0 * t0 * perlinDot( randomGrad(ii%tileSize,jj%tileSize,0), x0, y0 );
	}
	
	double t1 = 0.5 - x1*x1-y1*y1;
	if(t1<0) contrib1 = 0.0;
	else {
		t1 *= t1;
		contrib1 = t1 * t1 * perlinDot( randomGrad((ii+i1)%tileSize,(jj+j1)%tileSize,0), x1, y1 );
	}
	
	double t2 = 0.5 - x2*x2-y2*y2;
	if(t2<0) contrib2 = 0.0;
	else {
		t2 *= t2;
		contrib2 = t2 * t2 * perlinDot( randomGrad((ii+1)%tileSize,(jj+1)%tileSize,0), x2, y2 );
	}
	
	// add contributions for final value (result is scaled to be int the interval [-1;1])
	return 70.0 * (contrib0 + contrib1 + contrib2);
}

// simple 3D
double simplexMain(double x, double y, double z, int tileSize)
{
	return 0.0;
}

// simplex 4D
double simplexMain(double x, double y, double z, double w, int tileSize)
{
	return 0.0;
}

double perlinOctave(double x, double y, double z, unsigned int octaveCount, double persistence, double freq, double tiles)
{
    double total = 0;
    double frequency = freq;
    double amplitude = 1;
    double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(unsigned int oct=0;oct<octaveCount;oct++) {
		
		int tileSize = frequency*tiles;
		if(tileSize <= 0)tileSize = 1;
        total += perlinMain(x * frequency, y * frequency, z * frequency, tileSize) * amplitude;
		
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}

double simplexOctave(double x, double y, double z, unsigned int octaveCount, double persistence, double freq, double tiles)
{
    double total = 0;
    double frequency = freq;
    double amplitude = 1;
    double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(unsigned int oct=0;oct<octaveCount;oct++) {
		
		int tileSize = frequency*tiles;
		if(tileSize <= 0)tileSize = 1;
        total += simplexMain(x * frequency, y * frequency, tileSize) * amplitude;
		
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}



IMPGEARS_END
