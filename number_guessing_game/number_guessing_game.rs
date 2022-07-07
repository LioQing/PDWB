extern crate rand;

use std::io;
use std::cmp;
use rand::Rng;

fn main() {
    let mut lower = 1;
    let mut upper = 100;
    let n = rand::thread_rng().gen_range(lower..=upper);

    loop {
        let mut input = String::new();
        match io::stdin().read_line(&mut buffer) {
            Err(_) => continue,
            _ => {},
        }

        match input.trim().parse::<i32>() {
            Err(_) => continue,
            Ok(x) if x > n => {
                println!("{x} is too large");
                upper = x - 1;
            },
            Ok(x) if x < n => {
                println!("{x} is too small");
                lower = x + 1;
            },
            Ok(x) => {
                println!("You are correct");
                break;
            }
        }
    }
}