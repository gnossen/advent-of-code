use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

const TOP_COUNT: usize = 3;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn  update_top_n(top: &Vec<i64>, candidate: i64, n: usize) -> Vec<i64> {
    let mut new_top: Vec<i64> = Vec::<i64>::new();
    let mut top_pos: usize = 0;
    let mut candidate_used: bool = false;
    for _ in 0..n {
        // if !candidate_used && top_pos < top.len() && candidate > top[top_pos] {
        if top_pos < top.len() {
            if !candidate_used && candidate > top[top_pos] {
                // Insert the candidate.
                new_top.push(candidate);
                candidate_used = true;
            } else {
                // Insert the existing one.
                new_top.push(top[top_pos]);
                top_pos += 1
            }
        } else if !candidate_used {
            // Insert the candidate.
            new_top.push(candidate);
            candidate_used = true;
        }
    }
    new_top
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut top: Vec<i64> = Vec::<i64>::new();
    let mut current: i64 = 0;
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(calorie_str) = line {
                if calorie_str == "" {
                    top = update_top_n(&top, current, TOP_COUNT);
                    current = 0;
                } else {
                    current += calorie_str.parse::<i64>().unwrap();
                }
            }
        }
    }
    top = update_top_n(&top, current, TOP_COUNT);
    let mut total: i64 = 0;
    for i in 0..TOP_COUNT {
        total += top[i];
    }
    println!("{}", total);
}
