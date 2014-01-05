WeaponStats loadStats()
{
	WeaponStats stats;
	stats.imageFilename = "staff.png";
	stats.projectileType = "Magic";
	//stats.projectileStats = loadProjectilePrototype(stats.projectileType);
	stats.setProjectileStats(loadProjectilePrototype(stats.projectileType));
	stats.coolDownTime = 2.5;

	return stats;
}
