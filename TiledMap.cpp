#include "TiledMap.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>

#include "Box2DIntegration.h"
#include "Level2D.h"




TiledMap::TiledMap(std::string mapFilename)
{
	mapData = new Tmx::Map();
	mapData->ParseFile(mapFilename);
	int sizeX = mapData->GetWidth();
	int sizeY = mapData->GetHeight();
	tileWidth = mapData->GetTileWidth();
	tileHeight = mapData->GetTileHeight();
	int numLayers = mapData->GetNumLayers();
	int numObjectGroups = mapData->GetNumObjectGroups();

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

	for(int i = 0; i < numLayers; ++i)
	{
		for(int y = 0; y < sizeY; ++y)
		{
			for(int x = 0; x < sizeX; ++x)
			{
				int gid = mapData->GetLayer(i)->GetTileId(x, y);
				if(gid > 0)	// Don't create a tile for blank ones (gid 0).
				{
					geode->addDrawable( createTile(osg::Vec3(x, -y - 1, i), 1.0f, 1.0f, gid));
				}
			}
		}
	}

	for(int i = 0; i < numObjectGroups; ++i)
	{
		const Tmx::ObjectGroup *objectGroup = mapData->GetObjectGroup(i);
		for(int j = 0; j < objectGroup->GetNumObjects(); ++j)
		{
			physicsBodies.resize(j+1);
			const Tmx::Object* object = objectGroup->GetObject(j);
			const Tmx::Polygon* polygon = object->GetPolygon();
			b2Vec2 *vertices;
			vertices = new b2Vec2[100];		/// Will crash if the polygon has too many points. Find better solution.
			for(int k = 0; k < polygon->GetNumPoints(); ++k)
			{
				vertices[k] = b2Vec2((polygon->GetPoint(k).x + object->GetX())/ tileWidth, - (polygon->GetPoint(k).y + object->GetY()) / tileHeight);	// TMX file reports positions in pixels, y-down coordinate system.
			}
			b2ChainShape shape;
			shape.CreateLoop(vertices, polygon->GetNumPoints());

			//b2Body *physicsBody;
			b2BodyDef bodyDef;
			bodyDef.type = b2_staticBody;
			//bodyDef.position.Set(object->GetX() / tileWidth, - object->GetY() / tileHeight);
			bodyDef.position.Set(0,0);
			physicsBodies[j] = getCurrentLevel()->getPhysicsWorld()->CreateBody(&bodyDef);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			physicsBodies[j]->CreateFixture(&fixtureDef);
			Box2DUserData *userData = new Box2DUserData;
			userData->owner = this;
			userData->ownerType = "Tilemap";
			physicsBodies[j]->SetUserData(userData);
		}
	}

	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(geode);
	root->addChild(transformNode);
}


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
	for(int i = 0; i < physicsBodies.size(); ++i)
		physicsBodies[i]->SetTransform(toB2Vec2(position), physicsBodies[i]->GetAngle());

}
osg::Vec3 TiledMap::getPosition()
{
	return transformNode->getPosition();
}

TiledMap::~TiledMap()
{
	//dtor
}
