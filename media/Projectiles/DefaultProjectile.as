
ProjectileStats loadStats()
{
	ProjectileStats stats;
	stats.imageFilename = "circle.png";

	Damage dam;
	dam.type = HOT;
	dam.amount = 1.0;
	stats.damages.push_back(dam);

	return stats;
}
