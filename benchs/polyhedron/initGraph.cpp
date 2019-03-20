
Geometry lineVol = Geometry::cylinder(10,1.0,0.02);
Geometry arrowVol = Geometry::cone(10,0.1,0.05,0.0);
arrowVol += Vec3::Z;

// arrowVol += Vec3::X*0.5;
// lineVol -= Vec3::X*0.5;

Geometry zAxe = arrowVol + lineVol;
Geometry xAxe = zAxe; xAxe.rotY(1.57);
Geometry yAxe = zAxe; yAxe.rotX(1.57);

Geometry coords = xAxe + yAxe + zAxe;
coords.setPrimitive(Geometry::Primitive_Triangles);

Geometry white = Geometry::sphere(4, 0.5);
Geometry blue = coords;//Geometry::cylinder(10, 2.0, 0.3);
blue += Vec3(0.0,0.0,-0.5); // Geometry::snake(blue);
Geometry green = Geometry::sphere(4, 0.5); green += Vec3(-0.2,0.3,0.3);
Geometry red = Geometry::sphere(4, 0.5); red += Vec3(0.3,-0.2,-0.5);

Polyhedron ph_w, ph_b, ph_r, ph_g;
ph_w.set(white._vertices);
ph_b.set(blue._vertices);
ph_r.set(red._vertices);
ph_g.set(green._vertices);


Polyhedron wg = ph_w - ph_g;
Polyhedron wgb = wg - ph_b;
Polyhedron wrgb = wgb - ph_r;

GeoNode::Ptr iRW = GeoNode::create(wrgb, true);
iRW->setColor(Vec3(1.0,1.0,1.0));
iRW->setPosition(Vec3(2.0,0.0,0.0));
graph.add(iRW);

Shader::Ptr mshader = Shader::create(vertexSimple.c_str(),fragSimple.c_str());


// blue.reduceTriangles(0.7);
GeoNode::Ptr blueS = GeoNode::create(blue, false);
blueS->setColor(Vec3(0.3,0.3,1.0));
blueS->setShader(mshader);
graph.add(blueS);

GeoNode::Ptr greenS = GeoNode::create(green, true);
greenS->setColor(Vec3(0.0,1.0,0.0));
graph.add(greenS);

GeoNode::Ptr redS = GeoNode::create(red, true);
redS->setColor(Vec3(1.0,0.0,0.0));
graph.add(redS);

graph.wire(Vec3(-1.0,0.0,0.0), Vec3(1.0,0.0,0.0), Vec3(1.0,0.0,0.0));
graph.point(Vec3(-0.5,0.0,0.0), Vec3(0.0,0.0,1.0));
