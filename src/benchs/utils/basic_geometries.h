#ifndef BASIC_GEOMETRIES_H
#define BASIC_GEOMETRIES_H

Geometry::Ptr buildCoord();

#endif //BASIC_GEOMETRIES_H

#if defined IMPLEMENT_BASIC_GEOMETRIES

Geometry::Ptr buildCoord()
{
    Geometry lineVol = Geometry::cylinder(10,1.0,0.02);
    lineVol.generateNormals();
    lineVol.generateTexCoords();
    Geometry arrowVol = Geometry::cone(10,0.1,0.05,0.0);
    arrowVol.generateNormals();
    arrowVol.generateTexCoords();
    arrowVol += Vec3::Z;
    Geometry zAxe = arrowVol + lineVol;
    Geometry xAxe = zAxe; xAxe.rotY(1.57);
    Geometry yAxe = zAxe; yAxe.rotX(-1.57);
    xAxe.generateColors(Vec3(1.0,0.0,0.0));
    yAxe.generateColors(Vec3(0.0,1.0,0.0));
    zAxe.generateColors(Vec3(0.0,0.0,1.0));
    Geometry::Ptr coords = Geometry::create();
    *coords = xAxe + yAxe + zAxe;
    coords->setPrimitive(Geometry::Primitive_Triangles);
    (*coords) *= 0.2;
    return coords;
}

#endif // IMPLEMENT_BASIC_GEOMETRIES
