#include "ImprovedAnimationManager.h"

ImprovedAnimationManager::ImprovedAnimationManager(const AnimationManagerBase& b, const osg::CopyOp& copyop) : BasicAnimationManager(b, copyop)
{
	for (osgAnimation::AnimationList::const_iterator it = getAnimationList().begin(); it != getAnimationList().end(); it++)
		_map[(*it)->getName()] = *it;

	for(osgAnimation::AnimationMap::iterator it = _map.begin(); it != _map.end(); it++)
		_amv.push_back(it->first);

	_focus = 0;
}

ImprovedAnimationManager::~ImprovedAnimationManager()
{
	//dtor
}

bool ImprovedAnimationManager::play(std::string name)
{
	for(unsigned int i = 0; i < _amv.size(); i++)
		if(_amv[i] == name)
			_focus = i;
	std::cout << "Play " << name << std::endl;
	playAnimation(_map[name].get());
	return true;
}
bool ImprovedAnimationManager::play()
{
	if(_focus < _amv.size())
	{
		std::cout << "Play " << _amv[_focus] << std::endl;
		playAnimation(_map[_amv[_focus]].get());
		return true;
	}

	return false;
}

void ImprovedAnimationManager::setPlayMode(osgAnimation::Animation::PlayMode mode)
{
	_map[_amv[_focus]]->setPlayMode(mode);
}
