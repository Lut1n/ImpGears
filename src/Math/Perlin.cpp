#include <Math/Perlin.h>
#include <Math/BasicFunctions.h>

#include <ctime>
#include <cstdlib>
#include <iostream>

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


	// generation
	for(unsigned int i=0; i<uniqueValueCount; ++i)
		_table[i] = i;

	// permutation pseudo random
	for(unsigned int i=0; i<uniqueValueCount; ++i)
	{
		int x = i;
		double r01 = (randHugosElias(_seed+i) + 1) / 2;
		int y = r01 * 255;
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

int* randomGrad(int x, int y, int z)
{
	unsigned char rvalue = PermutationTable::getInstance()->hash(x,y, z);
	return s_grad3[rvalue & 15];
}

double perlinDot(int* vec, double x, double y, double z)
{
	return vec[0]*x + vec[1]*y + vec[2]*z;
}

double perlinMain(double x, double y, double z)
{
	PermutationTable::getInstance()->init(0);

	int X, Y, Z;
	double fx, fy, fz;
	// get integer value into X, Y and Z
	// get fractionnal value in x, y, z.
	intFrac(x, X, fx);
	intFrac(y, Y, fy);
	intFrac(z, Z, fz);

	// do a modulo 256 on X, Y, Z.
	X &= 255;
	Y &= 255;
	Z &= 255;

const  double g000 = perlinDot(randomGrad(X, Y, Z),
fx, fy, fz);
const double g001 = perlinDot(randomGrad(X, Y, Z + 1),
fx, fy, fz - 1.);
const double g010 = perlinDot(randomGrad(X, Y + 1, Z),
fx, fy - 1., fz);
const double g011 = perlinDot(randomGrad(X, Y + 1, Z + 1),
fx, fy - 1., fz - 1.);
const double g100 = perlinDot(randomGrad(X + 1, Y, Z),
fx - 1., fy, fz);
const double g101 = perlinDot(randomGrad(X + 1, Y, Z + 1),
fx - 1., fy, fz - 1.);
const double g110 = perlinDot(randomGrad(X + 1, Y + 1, Z),
fx - 1., fy - 1., fz);
const double g111 = perlinDot(randomGrad(X + 1, Y + 1, Z + 1),
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

double perlinOctave(double x, double y, double z, unsigned int octaveCount, double persistence)
{
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(unsigned int oct=0;oct<octaveCount;oct++) {
        total += perlinMain(x * frequency, y * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}

IMPGEARS_END
