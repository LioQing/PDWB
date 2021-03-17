import random

lower = 1
upper = 100
n = random.randint(lower, upper)

input_num = 0
while input_num != n:
    while True:
        try:
            input_num = int(input("Make a guess (%d - %d): " % (lower, upper)))
        except ValueError:
            continue
        
        if input_num < lower or input_num > upper:
            continue
        else:
            break
    
    if input_num > n:
        print("%d is too large" % input_num)
        upper = input_num - 1
    elif input_num < n:
        print("%d is too small" % input_num)
        lower = input_num + 1
    else:
        print("You are correct")
