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

fn find_sum(sorted_arr : &[u32], len : usize, sum : u32, count: u32) -> Result<Vec<u32>, NoSolutionError> {
  if count == 1 {
    // TODO: Binary search.
    for x in sorted_arr[..len].iter() {
      if x == &sum {
          return Ok(vec![*x]);
      }
    }
    let e = NoSolutionError {};
    return Err(e);
  } else {
    for (lower_idx, lower) in sorted_arr[..(len-1)].iter().enumerate() {
      // TODO: Binary search to find the first element no larger than the sum.
      if lower >= &sum { continue; }
      let result_or = find_sum(&sorted_arr[lower_idx + 1..len],
                               len - lower_idx - 1,
                               sum - lower,
                               count - 1);
      if result_or.is_ok() {
        let result = result_or.unwrap();
        let mut new_result = vec![*lower];
        new_result.extend(result.iter().cloned());
        return Ok(new_result);
      }
    }
    let e = NoSolutionError {};
    return Err(e);
  }
}

fn main() {
  let args : Vec<String> = env::args().collect();
  assert!(args.len() == 4, "Usage: {} FILE COUNT TARGET", args[0]);

  let filepath = &args[1];
  let count = args[2].parse::<u32>().expect("Count must be a number.");
  let target = args[3].parse::<u32>().expect("Target must be a number.");

  let mut numbers : Vec<u32> = Vec::new();
  let file = File::open(filepath).expect("Failed to open file.");
  for line_or in io::BufReader::new(file).lines() {
    let line = line_or.expect("Failed to read line.");
    let number_or = line.parse::<u32>();
    assert!(number_or.is_ok(), "Failed to parse \"{}\" as a number", line);
    numbers.push(number_or.unwrap());
  }

  numbers.sort_by(|a, b| b.cmp(a));
  let result = find_sum(&numbers[..], numbers.len(), target, count).expect("No solution found.");
  println!("{}", result.iter().product::<u32>());
}
