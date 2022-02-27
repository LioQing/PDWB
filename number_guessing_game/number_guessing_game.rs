extern crate rand;

use std::io;
use io::Write;
use std::cmp;
use rand::Rng;

fn input_guess(lower: i32, upper: i32) -> i32 {
    loop {
        let mut buf = String::new();

        print!("Make a guess({} - {}): ", lower, upper);
        io::stdout()
            .flush()
            .expect("Could not flush stdout");

        match io::stdin().read_line(&mut buf) {
            Err(_) => continue,
            Ok(_) => match buf.trim().parse::<i32>() {
                Ok(x) if lower <= x && x <= upper => return x,
                _ => continue,
            },
        }
    }
}

fn main() {
    let mut lower = 1;
    let mut upper = 100;
    let n = rand::thread_rng().gen_range(lower..=upper);

    loop {
        let x = input_guess(lower, upper);
        match x.cmp(&n) {
            cmp::Ordering::Greater => {
                println!("{} is too large", x);
                upper = x - 1;
            },

            cmp::Ordering::Less => {
                println!("{} is too small", x);
                lower = x + 1;
            },

            _ => {
                println!("You are correct");
                break;
            },
        }
    }
}