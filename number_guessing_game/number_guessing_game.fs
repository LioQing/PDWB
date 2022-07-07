open System

let lower = 1
let upper = 100
let rnd = Random()
let n = rnd.Next(lower, upper + 1)

let rec input lower upper =
    printf $"Make a guess ({lower} - {upper}): "
    match Int32.TryParse (Console.ReadLine()) with
    | true, x when x >= lower && x <= upper -> x
    | _ -> input lower upper

let rec loop lower upper = 
    match input lower upper with
    | x when x > n ->
        printf $"{x} is too large\n"
        loop lower (x - 1)
    
    | x when x < n ->
        printf $"{x} is too small\n"
        loop (x + 1) upper
    
    | _ -> printf "You are correct\n"

loop lower upper