math.randomseed(os.time())
lower = 1
upper = 100
n = math.random(lower, upper)

input_num = nil
while input_num ~= n do
    while not input_num or input_num < lower or input_num > upper do
        print(string.format("Make a guess (%d - %d): ", lower, upper))
        input_num = io.read("n")
    end

    if input_num > n then
        print(string.format("%d is too large", input_num))
        upper = input_num - 1
    elseif input_num < n then
        print(string.format("%d is too small", input_num))
        lower = input_num + 1
    else
        print("You are correct")
    end
end