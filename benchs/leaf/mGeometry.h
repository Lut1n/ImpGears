#ifndef LEAF_N_COORDS_H
#define LEAF_N_COORDS_H

Geometry::Ptr generateLeaf();

#endif //LEAF_N_COORDS_H

#if defined IMPLEMENT_LEAF_N_COORDS

Geometry::Ptr generateLeaf()
{
    float freq = 10.0;
    float ampl = 0.5;

    Path path = Path::circle(100,100);

    for(int i=0;i<path.count();++i)
    {
        Vec3 p = path.vertex(i); p.normalize();
        float r = p.angleFrom(Vec3::Y);
        float v = std::sin(r * freq);
        path.vertex(i) = p * (1.0+v*ampl);
    }

    imp::Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::extrude(path, 0.1);
    *geometry = geometry->subdivise(4);
    geometry->generateColors(Vec3(0.1,0.6,0.2));

    const float max_d = 1.0+ampl;
    const float PI_2 = 2.0 * 3.141592;
    float freq2 = 0.5;
    float ampl2 = 0.2;
    for(int i=0;i<geometry->size();++i)
    {
        Vec3 p = geometry->_vertices[i];
        float d = Vec3(p[0],p[1],0.0).length();
        geometry->_vertices[i] += Vec3::Z * std::sin(d*PI_2/max_d * freq2) * ampl2;
    }
    Geometry::intoCCW( *geometry );
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry->interpolateNormals();
    // geometry.generateTexCoords(Geometry::TexGenMode_Cubic,2.0);
    geometry->generateTexCoords(Geometry::TexGenMode_Spheric);

    geometry->rotX(-3.1415 * 0.5);

    return geometry;
}

#endif
