void run()
{
	print("IT FUCKING WORKS!!\n");
	print(calc(9,4));
}

void main()
{
	print("IT FUCKING WORKS!!\n");
}

void test(Vec3 vec)
{
	if(vec.x != 0.0)
	{
		print("Okay. Got a value for x.");
		print(vec.x);
	}
	else
		print("Nope. Still not working.");
}

float calc(float a, float b)
{
	return a + b;
}
