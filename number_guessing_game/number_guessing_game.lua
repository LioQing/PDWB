math.randomseed(os.time())
lower = 1
upper = 100
n = math.random(lower, upper)

input = nil
while true do
    io.write(string.format("Make a guess (%d - %d): ", lower, upper))

    input = tonumber(io.read())
    if input == nil or input < lower or input > upper then
        goto continue
    end

    if input > n then
        print(string.format("%d is too large", input))
        upper = input - 1
    elseif input < n then
        print(string.format("%d is too small", input))
        lower = input + 1
    else
        print("You are correct")
        break
    end

    ::continue::
end