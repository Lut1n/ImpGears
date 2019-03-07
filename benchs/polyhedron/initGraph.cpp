

Spheroid::Ptr white = Spheroid::create(2, 0.5, 0.0, true); white->setPos(Vec3(0.0,0.0,0.0)); white->setColor(Vec3(1.0,1.0,1.0));
Spheroid::Ptr blue = Spheroid::create(2, 0.5, 0.0, true); blue->setPos(Vec3(0.4,0.0,0.5)); blue->setColor(Vec3(0.0,0.0,1.0));
Spheroid::Ptr green = Spheroid::create(1, 0.3, 0.0, true); green->setPos(Vec3(-0.2,0.3,0.3)); green->setColor(Vec3(0.0,1.0,0.0));
Spheroid::Ptr red = Spheroid::create(1, 0.4, 0.0, true); red->setPos(Vec3(0.3,-0.2,-0.5)); red->setColor(Vec3(1.0,0.0,0.0));

Vec3 pt1(0.3,0.3,0.7), pt2(0.7,0.3,0.0);
graph.point(pt1, Vec3(1.0,1.0,0.5));
graph.point(pt2, Vec3(0.0,1.0,1.0));

if(contains(blue->geo._vertices, pt1))
	std::cout << "yes" << std::endl;
else 
	std::cout << "nop" << std::endl;


if(contains(blue->geo._vertices, pt2))
	std::cout << "yes" << std::endl;
else
	std::cout << "nop" << std::endl;

GeoStruct ray; ray.set(pt1, pt2);
Vec3 ipt;
if( rayPolyhedron(ray, blue->geo._vertices, ipt) )
{
	graph.wire(pt1, pt2, Vec3(1.0,0.0,0.0));
	graph.point(ipt, Vec3(1.0,1.0,1.0));
	std::cout << "yes" << std::endl;
}
else
{
	std::cout << "nop" << std::endl;
}

BufV3 interPh,interPh2,interPh3,interPh4,interPh5, interPh6;

std::cout << "compute : white - red" << std::endl;
diffPolyhedron2(white->geo._vertices, red->geo._vertices, interPh, false);

std::cout << "compute : red - white" << std::endl;
diffPolyhedron2(red->geo._vertices, white->geo._vertices, interPh6, true);
for(auto v : interPh6) interPh.push_back(v);
// interPh = white->geo._vertices;

std::cout << "compute : (white - red) - blue" << std::endl;
diffPolyhedron2(interPh, blue->geo._vertices, interPh2, false);

std::cout << "compute : blue - (white - red)" << std::endl;
diffPolyhedron2(blue->geo._vertices, interPh, interPh3, true);
for(auto v : interPh3) interPh2.push_back(v);

std::cout << "compute : (white - red - blue) - green" << std::endl;
diffPolyhedron2(interPh2, green->geo._vertices, interPh4, false);

std::cout << "compute : green - (white - red - blue)" << std::endl;
diffPolyhedron2(green->geo._vertices, interPh2, interPh5, true);
for(auto v : interPh5) interPh4.push_back(v);
// reducepolyh(interPh, 0.7);

Spheroid::Ptr iRW = Spheroid::create(interPh4, false);
iRW->setColor(Vec3(1.0,1.0,1.0));
// graph.add(white);
graph.add(iRW);

reducepolyh(blue->geo._vertices, 0.7);
graph.add(blue);
reducepolyh(green->geo._vertices, 0.7);
graph.add(green);
reducepolyh(red->geo._vertices, 0.7);
graph.add(red);

// graph.add(red);
