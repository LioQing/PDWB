open System

let rnd = Random()

let inputPrompt lower upper =
    printf "Make a guess (%i - %i): " lower upper
    Console.ReadLine()

let rec inputLoop lower upper (input: string) =
    match Int32.TryParse input with
    | true, x when x >= lower && x <= upper -> x
    | _ -> inputPrompt lower upper |> inputLoop lower upper
    

let inputGuess lower upper =
    inputPrompt lower upper
    |> inputLoop lower upper

[<EntryPoint>]
let main args =
    
    let initLower = 1
    let initUpper = 100
    let n = rnd.Next(initLower, initUpper + 1)
        
    let rec mainLoop lower upper =
        match inputGuess lower upper with
        | x when x > n ->
            Console.WriteLine($"{x.ToString()} is too large");
            mainLoop lower (x - 1)
            
        | x when x < n ->
            Console.WriteLine($"{x.ToString()} is too small");
            mainLoop (x + 1) upper
            
        | _ -> Console.WriteLine("You are correct")
        
    mainLoop initLower initUpper
    
    0