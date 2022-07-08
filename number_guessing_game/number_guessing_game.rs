use std::{io, io::Write};
use rand::Rng; // require dependencies on rand

fn main() {
    let mut lower = 1;
    let mut upper = 100;
    let n = rand::thread_rng().gen_range(lower..=upper);

    loop {
        print!("Make a guess ({lower} - {upper}): ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        match io::stdin().read_line(&mut input) {
            Err(_) => continue,
            _ => {},
        }

        match input.trim().parse::<i32>() {
            Err(_) => continue,
            Ok(x) if x < lower || x > upper => continue,
            Ok(x) if x > n => {
                println!("{x} is too large");
                upper = x - 1;
            },
            Ok(x) if x < n => {
                println!("{x} is too small");
                lower = x + 1;
            },
            Ok(_) => {
                println!("You are correct");
                break;
            },
        }
    }
}