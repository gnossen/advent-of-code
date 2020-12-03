use std::fs::File;
use std::io::{self, BufRead};
use std::fmt;
use std::env;

#[derive(Debug)]
pub struct NoSolutionError;

impl fmt::Display for NoSolutionError {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    f.write_str("No solution found.")
  }
}

fn find_sum(sorted_arr : &[u32], len : usize, sum : u32) -> Result<(u32, u32), NoSolutionError> {
  for (lower_idx, lower) in sorted_arr[..(len-1)].iter().enumerate() {
    if lower >= &sum { continue; }
    for higher in sorted_arr[lower_idx + 1..len].iter() {
      if lower + higher == sum {
        return Ok((*lower, *higher));
      }
    }
  }
  let e = NoSolutionError {};
  return Err(e);
}

fn main() {
  let args : Vec<String> = env::args().collect();
  assert!(args.len() == 4, "Usage: {} FILE COUNT TARGET", args[0]);

  let filepath = &args[1];
  let count = args[2].parse::<u32>().expect("Count must be a number.");
  let target = args[3].parse::<u32>().expect("Target must be a number.");

  assert!(count == 2);

  let mut numbers : Vec<u32> = Vec::new();
  let file = File::open(filepath).expect("Failed to open file.");
  for line_or in io::BufReader::new(file).lines() {
    let line = line_or.expect("Failed to read line.");
    let number_or = line.parse::<u32>();
    assert!(number_or.is_ok(), "Failed to parse \"{}\" as a number", line);
    numbers.push(number_or.unwrap());
  }

  numbers.sort_by(|a, b| b.cmp(a));
  let (a, b) = find_sum(&numbers[..], numbers.len(), target).expect("No solution found.");
  println!("{}", a * b);
}
