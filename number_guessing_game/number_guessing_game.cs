using System;

var lower = 1;
var upper = 100;
var rnd = new Random();
var n = rnd.Next(lower, upper + 1);

var input = 0;
while (true)
{
    Console.Write($"Make a guess ({lower} - {upper}): ");

    if (!int.TryParse(Console.ReadLine(), out input) || input < lower || input > upper)
        continue;

    if (input > n)
    {
        Console.WriteLine($"{input} is too large");
        upper = input - 1;
    } 
    else if (input < n) 
    {
        Console.WriteLine($"{input} is too small");
        lower = input + 1;
    } 
    else 
    {
        Console.WriteLine("You are correct");
        break;
    }
}