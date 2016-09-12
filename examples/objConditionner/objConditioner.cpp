#include <System/Parser.h>

using namespace imp;

//--------------------------------------------------------------
void processPoint(const String& point, Uint32& v, Uint32& vt, Uint32& vn)
{
	Uint32 occ = point.countOccurrence('/');

	switch(occ)
	{
	case 0:
		sscanf(point.getValue(), "%u", &v);
		vt = 1;
		vn = 1;
	break;
	case 1:
		sscanf(point.getValue(), "%u/%u", &v, &vt);
		vn = 1;
	break;
	case 2:
		if(point.getChar(point.find('/')+1) == '/') // find "//" pattern
		{
			sscanf(point.getValue(), "%u//%u", &v, &vn);
			vt = 1;
		}
		else
		{
			sscanf(point.getValue(), "%u/%u/%u", &v, &vt, &vn);
		}
	break;
	default:
		fprintf(stdout, "wut?\n");
	break;
	}
}

//--------------------------------------------------------------
void processFace(const imp::String& line, imp::Parser& outputFile)
{
	char faceBuffer[512];

	String* points = IMP_NULL;
	Uint32 pointCount = 0;

	line.split(' ', &points, pointCount);

	// p=0 : "f_"
	// p=1 : A
	// p=2 : B
	// p=3 : C

	Uint32 vA,vtA,vnA, vB,vtB,vnB, vC,vtC,vnC;

	for(Uint32 p=3; p<pointCount; ++p)
	{
		processPoint(points[1], vA, vtA, vnA);
		processPoint(points[p-1], vB, vtB, vnB);
		processPoint(points[p], vC, vtC, vnC);

		sprintf(faceBuffer, "f %u/1/%u %u/2/%u %u/3/%u", vA,vnA, vB,vnB, vC,vnC);
		outputFile.writeLine(faceBuffer);
	}
}

//--------------------------------------------------------------
int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		fprintf(stdout, "impError : missing argument. \"objConditioner [input file] [output file]\"\n");
		return 0;
	}
	else if(argc > 3)
	{
		fprintf(stdout, "impError : too many argument. \"objConditioner [input file] [output file]\"\n");
		return 0;
	}

	imp::Parser inputFile(argv[1], imp::Parser::FileType_Text, imp::Parser::AccessMode_Read);
	imp::Parser outputFile(argv[2], imp::Parser::FileType_Text, imp::Parser::AccessMode_Write);

	imp::String line;
	while(!inputFile.isEnd())
	{
		inputFile.readLine(line);
		if(line.getValue()[0] != 'f')
			outputFile.writeLine(line);
		else
			processFace(line, outputFile);
	}

	return 0;
}
