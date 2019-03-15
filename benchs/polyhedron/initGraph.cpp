
/*
Vec3 center1(0.3,0.0,0.0);
Vec3 center2(-0.3,0.0,0.0);

Spheroid::Ptr geo1 = Spheroid::create(10, 0.5, 0.0, true); geo1->setPos(center1);
Spheroid::Ptr geo2 = Spheroid::create(10, 0.4, 0.0, true); geo2->setPos(center1);

Spheroid::Ptr geo3 = Spheroid::create(10, 0.5, 0.0, true); geo3->setPos(center2);
Spheroid::Ptr geo4 = Spheroid::create(10, 0.4, 0.0, true); geo4->setPos(center2);

Polyhedron ph_1; ph_1.set(geo1->geo._vertices);
Polyhedron ph_2; ph_2.set(geo2->geo._vertices);
Polyhedron ph_3; ph_3.set(geo3->geo._vertices);
Polyhedron ph_4; ph_4.set(geo4->geo._vertices);

Polyhedron ph12 = ph_1 - ph_2;
Polyhedron ph34 = ph_3 - ph_4;

Polyhedron ph_res = ph12 * ph34;

Spheroid::Ptr iRW = Spheroid::create(ph_res, true);
iRW->setColor(Vec3(1.0,1.0,1.0));
iRW->reduceEachTriangle(0.5);
graph.add(iRW);
*/



Spheroid::Ptr white = Spheroid::create(8, 0.5, 0.0, true); white->setPos(Vec3(0.0,0.0,0.0)); white->setColor(Vec3(1.0,1.0,1.0));
Spheroid::Ptr blue = Spheroid::create(6, 0.5, 0.0, true); blue->setPos(Vec3(0.4,0.0,0.5)); blue->setColor(Vec3(0.0,0.0,1.0));
Spheroid::Ptr green = Spheroid::create(4, 0.3, 0.0, true); green->setPos(Vec3(-0.2,0.3,0.3)); green->setColor(Vec3(0.0,1.0,0.0));
Spheroid::Ptr red = Spheroid::create(5, 0.4, 0.0, true); red->setPos(Vec3(0.3,-0.2,-0.5)); red->setColor(Vec3(1.0,0.0,0.0));

Polyhedron ph_w, ph_b, ph_r, ph_g;
ph_w.set(white->geo._vertices);
ph_b.set(blue->geo._vertices);
ph_r.set(red->geo._vertices);
ph_g.set(green->geo._vertices);


Polyhedron wg = ph_w - ph_g;
Polyhedron wgb = wg - ph_b;
Polyhedron wrgb = wgb - ph_r;

Spheroid::Ptr iRW = Spheroid::create(wrgb, false);
iRW->setColor(Vec3(1.0,1.0,1.0));
graph.add(iRW);

// reducepolyh(blue->geo._vertices, 0.7);
graph.add(blue);
// reducepolyh(green->geo._vertices, 0.7);
graph.add(green);
// reducepolyh(red->geo._vertices, 0.7);
graph.add(red);
