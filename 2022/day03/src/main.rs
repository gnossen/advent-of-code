use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

use std::collections::HashSet;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn get_compartment_contents(compartment: &str) -> HashSet<char> {
    let mut contents = HashSet::new();
    for c in compartment.chars() {
        contents.insert(c);
    }
    contents
}

fn ord(c: char) -> u32 {
    c as u32
}

fn get_priority(c: &char) -> u32 {
    let n = ord(c.clone());
    if n >= ord('a') && n <= ord('z') {
        n - ord('a') + 1
    } else if n >= ord('A') && n <= ord('Z') {
        n - ord('A') + 27
    } else {
        panic!("Unknown character {}", c)
    }
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut total_priority: u32 = 0;
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(rucksack_str) = line {
                if (rucksack_str.len() % 2) != 0 {
                    panic!("Rucksack \"{}\" did not have even length.", rucksack_str);
                }
                let compartment_a = &rucksack_str[..(rucksack_str.len()/2)];
                let compartment_b = &rucksack_str[(rucksack_str.len()/2)..];
                let contents_a = get_compartment_contents(compartment_a);
                let contents_b = get_compartment_contents(compartment_b);
                let common: Vec<&char> = contents_a.intersection(&contents_b).collect();
                assert!(common.len() == 1);
                // println!("Common character is {}!", common[0]);
                let c = common[0];
                total_priority += get_priority(c);
            } else {
                panic!("Failed to read line.");
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }
    println!("{}", total_priority)
}
