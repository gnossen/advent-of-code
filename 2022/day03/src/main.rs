use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

use std::clone::Clone;
use std::hash::Hash;
use std::collections::hash_map::RandomState;
use std::collections::HashSet;
use std::collections::hash_set::Intersection;


const GROUP_SIZE: usize = 3;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn get_string_contents(s: &str) -> HashSet<char> {
    let mut contents = HashSet::new();
    for c in s.chars() {
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

fn intersection_to_hashset<T: Eq + Hash + Clone>(s: Intersection<T, RandomState>) -> HashSet<T> {
    let mut res: HashSet<T> = HashSet::new();
    for v in s {
        res.insert(v.clone());
    }
    res
}

fn multi_intersection<T: Eq + Hash + Clone>(s: &Vec<HashSet<T>>) -> HashSet<T> {
    assert!(s.len() >= 1, "Must supply at least one set to perform intersection on.");
    let mut overall_intersection = s[0].clone();
    for i in 1..s.len() {
        overall_intersection = intersection_to_hashset(overall_intersection.intersection(&s[i]));
    }
    overall_intersection
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut total_priority: u32 = 0;
    let mut elf_counter: usize = 0;
    let mut rucksack_contents: Vec<HashSet<char>> = Vec::new();
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(rucksack_str) = line {
                rucksack_contents.push(get_string_contents(&rucksack_str))
            } else {
                panic!("Failed to read line.");
            }
            if elf_counter == GROUP_SIZE - 1 {
                let intersection = multi_intersection(&rucksack_contents);
                assert!(intersection.len() == 1);
                let c = intersection.iter().next().unwrap();
                total_priority += get_priority(c);
                elf_counter = 0;
                rucksack_contents.clear();
            } else {
                elf_counter += 1;
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }
    assert!(elf_counter == 0, "Found a number of lines not divisible by 3.");
    println!("{}", total_priority);
}
