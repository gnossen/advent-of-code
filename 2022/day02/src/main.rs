use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

const ROCK: u32 = 0;
const PAPER: u32 = 1;
const SCISSORS: u32 = 2;

const LOSS_POINTS: u32 = 0;
const DRAW_POINTS: u32 = 3;
const WIN_POINTS: u32 = 6;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

// Returns the score for player b.
fn score_round(play_a: u32, play_b: u32) -> u32 {
    let victory_points: u32;
    if play_a == play_b {
        victory_points = DRAW_POINTS;
    } else {
        if play_b == ROCK {
            if play_a == SCISSORS {
                victory_points = WIN_POINTS;
            } else /* PAPER */ {
                victory_points = LOSS_POINTS;
            }
        } else if play_b == PAPER {
            if play_a == ROCK {
                victory_points = WIN_POINTS;
            } else /* SCISSORS */ {
                victory_points = LOSS_POINTS;
            }
        } else /* SCISSORS */ {
            if play_a == PAPER {
                victory_points = WIN_POINTS;
            } else /* ROCK */ {
                victory_points = LOSS_POINTS;
            }
        }
    }
    victory_points + play_b + 1
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut total: u32 = 0;
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(round_str) = line {
                let (player_a, player_b) = round_str.split_once(" ").unwrap();
                let play_a = (player_a.chars().collect::<Vec<char>>()[0] as u32) - ('A' as u32);
                let play_b = (player_b.chars().collect::<Vec<char>>()[0] as u32) - ('X' as u32);
                // println!("{}", score_round(play_a, play_b))
                total += score_round(play_a, play_b);
            }
        }
    }
    println!("{}", total)
}
