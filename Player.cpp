#include "Player.h"


#ifdef MULTIPLE_PLAYERS
std::string activePlayerName;
std::map<std::string, Player*> playerList;
#endif

Player::Player(std::string name, osg::Vec3 position) : Entity(name, position, DEFAULT_PLAYER_IMAGE)
{
	modelNode->setUpdateCallback(new PlayerNodeCallback(this));
	equipWeapon(new Weapon());
	//equipedWeapon->setPosition(position);
	transformNode->addChild(equipedWeapon->getTransformNode());
}

void Player::setPosition(osg::Vec3 newPosition) {
	//transformNode->setPosition(newPosition);
	Entity::setPosition(newPosition);
	//equipedWeapon->setPosition(newPosition);
}

void Player::processMovementControls(osg::Vec3 controlVector)
{

	osg::Vec3 movementVector = controlVector * maxSpeed * getDeltaTime();

	//this->setPosition(getPosition() + movementVector);

	physicsBody->SetLinearVelocity(maxSpeed * toB2Vec2(controlVector));

	/*
	osg::Vec3 worldMovement = cameraToWorldTranslation(controlVector);
	if ( worldMovement.length() > 1.0f ) worldMovement.normalize();

	double controlAngle =  atan( worldMovement.y()/worldMovement.x());
	if ( worldMovement.x() < 0 )
	{
		controlAngle += pi;
	}
	static float threshold = pi/12.0;
	float angleDistance = (controlAngle + pi/2) - this->getHeading();
	if (abs(angleDistance) > pi)
	{
		if (angleDistance > 0 ) angleDistance -= 2*pi;
		else angleDistance += 2*pi;
	}
	if (worldMovement != osg::Vec3(0,0,0))
	{
		this->setHeading(controlAngle + pi/2);
	}
	this->controller->setWalkDirection(osgbCollision::asBtVector3(worldMovement) * maxSpeed * getDeltaTime() );
	// TODO: should turn around until it reaches the right point, instead of instantly changing.
	*/
}

void Player::checkForInput()
{

}

void PlayerNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
#ifdef MULTIPLE_PLAYERS
	if (player->isActivePlayer())
#endif
	{
		player->checkForInput();
	}

	traverse(node, nv);	// need to call this so scene graph traversal continues.
}

#ifdef MULTIPLE_PLAYERS

bool Player::isActivePlayer() {
	return ( activePlayerName == this->name);
}

Player* getActivePlayer() {
	return playerList[activePlayerName];
}

void setActivePlayer(std::string newActivePlayerName) {
	activePlayerName = newActivePlayerName;
}
void setActivePlayer(Player *player) {
	activePlayerName = player->name;
}

void addNewPlayer(std::string playerName, osg::Vec3 position) {
	playerList[playerName] = new Player(playerName, position);
}

#endif
