use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut max: i64 = 0;
    let mut current: i64 = 0;
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(calorie_str) = line {
                if calorie_str == "" {
                    if current > max {
                        max = current;
                    }
                    current = 0;
                } else {
                    current += calorie_str.parse::<i64>().unwrap();
                }
            }
        }
    }
    if current > max {
        max = current;
    }
    println!("{}", max);
}
