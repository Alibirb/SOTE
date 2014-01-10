FighterStats loadStats()
{
	FighterStats stats;
	stats.modelFilename = "Golem.osg";
	stats.setResistance(HOT, .75);
	stats.setResistance(COLD, 1.0);
	stats.setResistance(ACID, 1.0);
	stats.maxHealth = 5.0;
	stats.weaponType = "Throwing";
	stats.setWeaponStats(loadWeaponPrototype(stats.weaponType));
	stats.weaponStats.setProjectileStats(loadProjectilePrototype("Rock"));
	return stats;
}

