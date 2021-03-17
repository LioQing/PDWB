using System;

namespace NumberGuessingGame
{
    class Program
    {
        private static int lower = 1;
        private static int upper = 100;
        private static readonly Random rnd = new Random();
        
        static void Main(string[] args)
        {
            var n = rnd.Next(lower, upper + 1);

            for (var input = 0; input != n;)
            {
                do
                {
                    Console.Write($"Make a guess ({lower.ToString()} - {upper.ToString()}): ");
                }
                while (int.TryParse(Console.ReadLine(), out input) || input < lower || input > upper);

                if (input > n)
                {
                    Console.WriteLine($"{input.ToString()} is too large");
                    upper = input - 1;
                } 
                else if (input < n) 
                {
                    Console.WriteLine($"{input.ToString()} is too small");
                    lower = input + 1;
                } 
                else 
                {
                    Console.WriteLine("You are correct");
                }
            }
        }
    }
}