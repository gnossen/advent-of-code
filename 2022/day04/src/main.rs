use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;


fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn get_range(range_str: &str) -> (u32, u32) {
    let (a, b) = range_str.split_once("-").unwrap();
    (a.parse::<u32>().unwrap(), b.parse::<u32>().unwrap())
}

fn point_contained(range_lo: u32, range_hi: u32, point: u32) -> bool {
    point >= range_lo && point <= range_hi
}

fn overlaps(a_lo: u32, a_hi: u32, b_lo: u32, b_hi: u32) -> bool {
    (point_contained(a_lo, a_hi, b_lo) || point_contained(a_lo, a_hi, b_hi) ||
     point_contained(b_lo, b_hi, a_lo) || point_contained(b_lo, b_hi, a_hi))
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut overlaps_count = 0;
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(pair_line) = line {
                let (elf1, elf2) = pair_line.split_once(",").unwrap();
                let (lower1, higher1) = get_range(elf1);
                let (lower2, higher2) = get_range(elf2);
                if overlaps(lower1, higher1, lower2, higher2) {
                    overlaps_count += 1;
                }
            } else {
                panic!("Failed to read line.");
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }
    println!("{}", overlaps_count);
}
