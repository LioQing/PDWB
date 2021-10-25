import random

random.seed()
lower = 1
upper = 100
n = random.randint(lower, upper)

input_num = 0
while input_num != n:
    while True:
        try:
            input_num = int(input("Make a guess (%d - %d): " % (lower, upper)))
            if input_num < lower or input_num > upper:
                raise ValueError
            break
        except ValueError:
            continue
    
    if input_num > n:
        print(f"{input_num} is too large")
        upper = input_num - 1
    elif input_num < n:
        print(f"{input_num} is too small")
        lower = input_num + 1
    else:
        print("You are correct")
