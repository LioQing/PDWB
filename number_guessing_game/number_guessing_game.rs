extern crate rand;

use std::io;
use io::Write;
use rand::Rng;

fn input_guess(lower: i32, upper: i32) -> i32 {
	loop {
		let mut buf = String::new();

		print!("Make a guess({} - {}): ", lower, upper);
		io::stdout()
			.flush()
			.ok()
			.expect("Could not flush stdout");

		match io::stdin().read_line(&mut buf) {
			Err(_) => continue,
			Ok(_) => match buf.trim().parse::<i32>() {
				Ok(x) if x >= lower && x <= upper => return x,
				_ => continue,
			},
		}
	}
}

fn main() {
	let mut rng = rand::thread_rng();

	let mut lower = 1;
	let mut upper = 100;
	let n = rng.gen_range(lower..(upper + 1));

	loop {
		match input_guess(lower, upper) {
			x if x > n => {
				println!("{} is too large", x);
				upper = x - 1;
			},

			x if x < n => {
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