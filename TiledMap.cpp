#include "TiledMap.h"

TiledMap::TiledMap(std::string mapFilename)
{
	mapData = new Tmx::Map();
	mapData->ParseFile(mapFilename);
	int sizeX = mapData->GetWidth();
	int sizeY = mapData->GetHeight();
	int numLayers = mapData->GetNumLayers();

	std::string imageFilename = mapData->GetTileset(0)->GetImage()->GetSource();
	osg::Image* image = osgDB::readImageFile(imageFilename);
	if (!image)
		std::cout << "could not load image";
	geode = new osg::Geode();

	// Set up the StateSet for this map.
	state = new osg::StateSet;
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setImage(image);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	state->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);	// Apply the texture
	state->setRenderBinDetails(1, "transparent");	// Use a different rendering bin that gets rendered after the default one
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);	// Tell OpenGL that this is a transparent bin, and thus everything should be rendered back-to-front

	for (int i = 0; i < numLayers; ++i)
	{
		for (int y = 0; y < sizeY; ++y)
		{
			for (int x = 0; x < sizeX; ++x)
			{
				int gid = mapData->GetLayer(i)->GetTileId(x, y);
				if (gid > 0)	// Don't create a tile for blank ones (gid 0).
				{
					geode->addDrawable( createTile(osg::Vec3(x, -y, i), 1.0f, 1.0f, gid));
				}
			}
		}
	}
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(geode);
	root->addChild(transformNode);
}

/// Returns the texture coordinates for the given gid
osg::Vec2Array* TiledMap::getTextureCoordinates(int gid)
{
	float imageHeight = mapData->GetTileset(0)->GetImage()->GetHeight();
	float imageWidth = mapData->GetTileset(0)->GetImage()->GetWidth();
	float tileHeight = mapData->GetTileset(0)->GetTileHeight();
	float tileWidth = mapData->GetTileset(0)->GetTileWidth();
	double texWidthIncrement = tileWidth / imageWidth;
	double texHeightIncrement = tileHeight / imageHeight;
	int tilesPerRow = imageWidth / tileWidth;
	int tilesPerColumn = imageHeight / tileHeight;

	// Obtain x,y coordinates of tile in the tileset
	int x = gid % tilesPerRow;
	int y = ( gid - x) / tilesPerRow;

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(texWidthIncrement * x, 1.0 - texHeightIncrement * (y + 1));
	(*tcoords)[1].set(texWidthIncrement * (x + 1), 1.0 - texHeightIncrement * (y + 1));
	(*tcoords)[2].set(texWidthIncrement * (x + 1), 1.0 - texHeightIncrement * y);
	(*tcoords)[3].set(texWidthIncrement * x, 1.0 - texHeightIncrement * y);
	return tcoords;
}

/// Creates a Geometry for a tile with the given position, size, and gid.
osg::Geometry* TiledMap::createTile(const osg::Vec3& corner, float width, float height, int gid)
{
    osg::Geometry* tile = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = corner;
    (*coords)[1] = corner+osg::Vec3(width, 0.0, 0.0);
    (*coords)[2] = corner+osg::Vec3(width, height, 0.0);
    (*coords)[3] = corner+osg::Vec3(0.0, height, 0.0);
    tile->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0] = osg::Vec3(0.0, 0.0, 1.0);

    tile->setNormalArray(norms, osg::Array::BIND_OVERALL);

	osg::Vec2Array* tcoords = getTextureCoordinates(gid);
	tile->setTexCoordArray(0,tcoords);

    tile->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

	tile->setStateSet(this->state);

    return tile;
}

void TiledMap::setPosition(osg::Vec3 position)
{
	transformNode->setPosition(position);
}
osg::Vec3 TiledMap::getPosition()
{
	return transformNode->getPosition();
}

TiledMap::~TiledMap()
{
	//dtor
}
