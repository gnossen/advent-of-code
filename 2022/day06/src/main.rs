use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

use std::collections::HashSet;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn get_char_count(s: &str) -> usize {
    let mut chars: HashSet<char> = HashSet::new();
    for c in s.chars() {
        chars.insert(c);
    }
    chars.len()
}

fn get_packet_index(s: &str, packet_size: usize) -> usize {
    for i in packet_size..s.len() {
        let packet = &s[i-packet_size..i];
        if get_char_count(packet) == packet_size {
            return i;
        }
    }
    assert!(false);
    0
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(packet_line) = line {
                println!("{}", get_packet_index(&packet_line, 14))
            } else {
                panic!("Failed to read line.");
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }
}
