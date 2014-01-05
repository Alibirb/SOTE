ProjectileStats loadStats()
{
	ProjectileStats stats;
	stats.imageFilename = "Fireball.png";

	Damage dam;
	dam.type = HOT;
	dam.amount = 2.0;
	stats.damages.push_back(dam);

	return stats;
}
