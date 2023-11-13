var x = 600851475143;

while(x % 2 == 0)
{
    x /= 2;
}

for (var i = 3; i * i <= x; i += 2)
{
    while (x % i == 0)
    {
        x /= i;
    }
}

Console.WriteLine(x);