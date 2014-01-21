#ifndef IMPROVEDANIMATIONMANAGER_H
#define IMPROVEDANIMATIONMANAGER_H

#include <osgAnimation/BasicAnimationManager>


/// Inspired by osganimationviewer/AnimtkViewer.
class ImprovedAnimationManager : public osgAnimation::BasicAnimationManager
{
private:
	osgAnimation::AnimationMap _map;
	std::vector<std::string> _amv;
	unsigned int _focus;
public:
	ImprovedAnimationManager(const AnimationManagerBase& b, const osg::CopyOp& copyop= osg::CopyOp::SHALLOW_COPY);
	virtual ~ImprovedAnimationManager();
	bool play(std::string name);
	bool play();
	void setPlayMode(osgAnimation::Animation::PlayMode mode);
protected:
private:
};

#endif // IMPROVEDANIMATIONMANAGER_H
