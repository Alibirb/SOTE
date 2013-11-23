#include "Tile.h"


/** create quad at specified position. */
osg::Geometry* Tile::createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image)
{
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = corner;
    (*coords)[1] = corner+width;
    (*coords)[2] = corner+width+height;
    (*coords)[3] = corner+height;


    geom->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
    (*norms)[0] = width^height;
    (*norms)[0].normalize();

    geom->setNormalArray(norms, osg::Array::BIND_OVERALL);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f,0.0f);
    (*tcoords)[1].set(1.0f,0.0f);
    (*tcoords)[2].set(1.0f,1.0f);
    (*tcoords)[3].set(0.0f,1.0f);
    geom->setTexCoordArray(0,tcoords);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    if (image)
    {
        osg::StateSet* stateset = new osg::StateSet;
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setImage(image);
        stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
        geom->setStateSet(stateset);
    }

    return geom;
}
