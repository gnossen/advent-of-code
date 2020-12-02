use std::fs::File;
use std::io::{self, BufRead};
// use std::path::Path;
use std::process::abort;
use std::fmt;

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
  // TODO: Get this from the command line.
  let filepath = "in.txt";
  let mut numbers : Vec<u32> = Vec::new();
  match File::open(filepath) {
    Ok(file) => {
      for line_or in io::BufReader::new(file).lines() {
        match line_or {
          Ok(line) => {
            let number_or = line.parse::<u32>();
            match number_or {
              Ok(number) => numbers.push(number),
              Err(_) => {
                println!("Failed to parse \"{}\" as a number", line);
                abort();
              }
            }
          },
          Err(_) => {
            println!("Failed to read line");
            abort();
          },
        }
      }
    },
    Err(_) => {
      println!("Failed to open file {}", filepath);
      abort();
    }
  }

  numbers.sort_by(|a, b| b.cmp(a));
  let (a, b) = find_sum(&numbers[..], numbers.len(), 2020).expect("No solution found.");
  println!("{}", a * b);
}
