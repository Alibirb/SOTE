#include "Level2D.h"


Level2D::Level2D(std::string mapFilename)
{
	tiledMap = new Tmx::Map();
	tiledMap->ParseFile(mapFilename);
	sizeX = tiledMap->GetWidth();
	sizeY = tiledMap->GetHeight();
	numLayers = tiledMap->GetNumLayers();
	usesTmx = true;

	std::string imageFilename = tiledMap->GetTileset(0)->GetImage()->GetSource();
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

	// Set up the Tile array with the proper values.
	tiles.resize(numLayers);
	for (int i = 0; i < numLayers; ++i)
	{
		tiles[i].resize(sizeY);
		for (int y = 0; y < sizeY; ++y)
		{
			tiles[i][y].resize(sizeX);
			for (int x = 0; x < sizeX; ++x)
			{
				int gid = tiledMap->GetLayer(i)->GetTileId(x, y);
				if (gid > 0)	// Don't create a tile for blank ones (gid 0).
				{
					tiles[i][y][x] = createTile( osg::Vec3(x, y, -i), 1.0f, 1.0f, gid);
					geode->addDrawable( tiles[i][y][x]->geometry);
				}
			}
		}
	}
	root->addChild(geode);

	gravity = b2Vec2(0.0, -10.0);
	physicsWorld = new b2World(gravity);
	debugDrawer = new DebugDrawer;
	physicsWorld->SetDebugDraw(debugDrawer);

	setCurrentLevel(this);
}


/// Returns the texture coordinates for the given gid
osg::Vec2Array* Level2D::getTextureCoordinates(int gid)
{
	float imageHeight = tiledMap->GetTileset(0)->GetImage()->GetHeight();
	float imageWidth = tiledMap->GetTileset(0)->GetImage()->GetWidth();
	float tileHeight = tiledMap->GetTileset(0)->GetTileHeight();
	float tileWidth = tiledMap->GetTileset(0)->GetTileWidth();
	double texWidthIncrement = tileWidth / imageWidth;
	double texHeightIncrement = tileHeight / imageHeight;
	int tilesPerRow = imageWidth / tileWidth;
	int tilesPerColumn = imageHeight / tileHeight;

	// Obtain x,y coordinates of tile in the tileset
	int x = gid % tilesPerRow;
	int y = ( gid - x) / tilesPerRow;

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(texWidthIncrement * x, 1.0 - texHeightIncrement * y);
	(*tcoords)[1].set(texWidthIncrement * (x + 1), 1.0 - texHeightIncrement * y);
	(*tcoords)[2].set(texWidthIncrement * (x + 1), 1.0 - texHeightIncrement * (y + 1));
	(*tcoords)[3].set(texWidthIncrement * x, 1.0 - texHeightIncrement * (y + 1));
	return tcoords;
}


Tile* Level2D::createTile(const osg::Vec3& corner, float width, float height, int gid)
{
    // set up the Geometry.
    osg::Geometry* geometry = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = corner;
    (*coords)[1] = corner+osg::Vec3(width, 0.0, 0.0);
    (*coords)[2] = corner+osg::Vec3(width, height, 0.0);
    (*coords)[3] = corner+osg::Vec3(0.0, height, 0.0);
    geometry->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0] = osg::Vec3(0.0, 0.0, 1.0);

    geometry->setNormalArray(norms, osg::Array::BIND_OVERALL);

	osg::Vec2Array* tcoords = getTextureCoordinates(gid);
	geometry->setTexCoordArray(0,tcoords);

    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

	geometry->setStateSet(this->state);

    Tile* tile = new Tile(geometry, corner.x(), corner.y(), corner.z());

    return tile;
}


Level2D* currentLevel;

Level2D* getCurrentLevel() {
	return currentLevel;
}
void setCurrentLevel(Level2D* newLevel) {
	currentLevel = newLevel;
}
