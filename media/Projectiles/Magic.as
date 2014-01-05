ProjectileStats loadStats()
{
	ProjectileStats stats;
	stats.imageFilename = "circle.png";

	Damage dam;
	dam.type = PURE_MAGIC;
	dam.amount = 2.0;
	stats.damages.push_back(dam);
	return stats;
}
