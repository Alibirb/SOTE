ProjectileStats loadStats()
{
	ProjectileStats stats;
	stats.imageFilename = "Rock.png";

	Damage dam;
	dam.type = IMPACT;
	dam.amount = 2.0;
	stats.damages.push_back(dam);

	return stats;
}

