/// Script for testing purposes.


void run()
{
	print("IT FUCKING WORKS!!\n");
}

void print(Vec3 &in vec)
{
	print(vec.x);
	print(vec.y);
	print(vec.z);
}

Vec3 calc(Vec3 &in a, Vec3 &in b)
{
	return a + b;
}
void calcAndPrint(Vec3 &in a, Vec3 &in b)
{
	print(a + b);
}


Vec3 straightReturn(Vec3 &in a)
{
	return a;
}
Vec3 createWithValues(float x = 1, float y = 2, float z = 3)
{
	return Vec3(x, y, z);
}

float calc(float a, float b)
{
	return a + b;
}
