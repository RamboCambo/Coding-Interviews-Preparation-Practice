sum = 2

second_last = 1
last = 2

while True:
    next = second_last + last
    
    if next > 4000000:
        break
    
    if next % 2 == 0:
        sum += next
    
    second_last = last
    last = next

print(sum)