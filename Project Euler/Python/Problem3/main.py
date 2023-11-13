x = 600851475143

while x % 2 == 0:
    x = x // 2

i = 3
while i * i <= x:
    while x % i == 0:
        x = x // i
    i = i + 2

print(x)