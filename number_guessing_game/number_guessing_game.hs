import System.Random -- require build-depends on random
import System.IO
import Text.Read

input :: Integer -> Integer -> IO Integer
input lower upper = do
    putStr $ "Make a guess (" ++ show lower ++ " - " ++ show upper ++ "): "
    hFlush stdout
    line <- getLine
    case readMaybe line :: Maybe Integer of
        Nothing -> input lower upper
        Just x | x < lower || x > upper -> input lower upper
        Just x -> return x

loop :: Integer -> Integer -> Integer -> IO ()
loop n lower upper = do
    answer <- input lower upper
    case answer of
        x | x > n -> do
            putStrLn $ show x ++ " is too large"
            loop n lower (x - 1)
        x | x < n -> do
            putStrLn $ show x ++ " is too small"
            loop n (x + 1) upper
        _ -> putStrLn "You are correct"

main :: IO ()
main = do
    rnd <- newStdGen
    let lower = 1 :: Integer
    let upper = 100 :: Integer
    let n = fst $ randomR (lower, upper) rnd
    loop n lower upper