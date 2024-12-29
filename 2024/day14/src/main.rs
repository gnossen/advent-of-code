use std::env;
use std::fs;
use std::io::{ BufReader, BufRead };
use regex::Regex;
use std::collections::HashMap;

const DIRS: &[(i64, i64)] = &[
    (-1, 0),
    (0, -1),
    (1, 0),
    (0, 1),
];


#[derive(Debug)]
struct Robot {
    position: (usize, usize),
    velocity: (i64, i64),
}

impl Robot {
    fn forecast(&self, bounds: (usize, usize), seconds: usize) -> (usize, usize) {
        let signed_position = (
            ((self.position.0 as i64) + self.velocity.0 * (seconds as i64)) % (bounds.0 as i64),
            ((self.position.1 as i64) + self.velocity.1 * (seconds as i64)) % (bounds.1 as i64),
        );
        (
            (if signed_position.0 >= 0 { signed_position.0 } else { signed_position.0 + (bounds.0 as i64) } ) as usize,
            (if signed_position.1 >= 0 { signed_position.1 } else { signed_position.1 + (bounds.1 as i64) } ) as usize,
        )
        // print!("Bounds: {:#?}\n", bounds);
        // print!("Velocity: {:#?}\n", self.velocity);
        // print!("Delta: {:#?}\n\n", delta);
    }
}

fn parse_robot(line: String) -> Robot {
    let robot_line = Regex::new(r"^p=([0-9]+),([0-9]+) v=(-?[0-9]+),(-?[0-9]+)$").unwrap();
    let (_, [raw_pos_x, raw_pos_y, raw_vel_x, raw_vel_y]) = robot_line.captures(&line).unwrap().extract();
    Robot {
        position: (raw_pos_x.parse().unwrap(), raw_pos_y.parse().unwrap()),
        velocity: (raw_vel_x.parse().unwrap(), raw_vel_y.parse().unwrap()),
    }
}

fn get_quad(pos: (usize, usize), bounds: (usize, usize)) -> Option<(usize, usize)> {
    let halves = (
        (bounds.0 - 1) / 2,
        (bounds.1 - 1) / 2,
    );

    if bounds.0 % 2 == 1 && pos.0 == halves.0 {
        return None;
    }

    if bounds.1 % 2 == 1 && pos.1 == halves.1 {
        return None;
    }

    Some((
        if pos.0 < halves.0 { 0 } else { 1 },
        if pos.1 < halves.1 { 0 } else { 1 },
    ))
}

fn grid_counts(pos: &Vec<(usize, usize)>, bounds: (usize, usize)) -> Vec<Vec<usize>> {
    let mut counts: Vec<Vec<usize>> = (0..bounds.1).map(
                                        |_y| (0..bounds.0).map(|_x| (0 as usize)).collect()
                                      ).collect();
    for p in pos.iter() {
        counts[p.1][p.0] += 1;
    }

    counts
}

fn print_pos(counts: &Vec<Vec<usize>>, bounds: (usize, usize)) {
    for row in counts {
        for col in row {
            if *col == 0 {
                print!(".")
            } else {
                print!("{}", col)
            }
        }
        print!("\n")
    }
    print!("\n")
}

fn adjacent_dim(us: usize, s: i64, max: usize) -> Option<usize> {
    let cs = (us as i64) + s;
    if cs < 0 {
        return None;
    }
    let cu = cs as usize;
    if cu >= max {
        return None;
    }
    Some(cu)
}

fn adjacent(c: (usize, usize), dir: (i64, i64), bounds: (usize, usize)) -> Option<(usize, usize)> {
    let x = adjacent_dim(c.0, dir.0, bounds.0)?;
    let y = adjacent_dim(c.1, dir.1, bounds.1)?;
    Some((x, y))
}

fn adjacent_cells(x: (usize, usize), bounds: (usize, usize)) -> impl Iterator<Item = (usize, usize)> {
    DIRS.iter()
        .map(move |dir| adjacent(x, dir.clone(), bounds.clone()))
        .flatten()
}

fn count_adjacencies(counts: &Vec<Vec<usize>>, bounds: (usize, usize)) -> usize {
    let mut adjacencies: usize = 0;
    for y in 0..bounds.1 {
        for x in 0..bounds.0 {
            if counts[y][x] > 0 {
                for adj in adjacent_cells((x, y), bounds) {
                    if counts[adj.1][adj.0] > 0 {
                        adjacencies += 1;
                    }
                }
            }
        }
    }
    adjacencies
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        panic!("Usage: {} FILENAME WIDTH HEIGHT", args[0]);
    }

    let filename = args[1].clone();

    let width: usize = args[2].parse().unwrap();
    let height: usize = args[3].parse().unwrap();
    let bounds = (width, height);

    let f = fs::File::open(filename)?;
    let reader = BufReader::new(f);

    let robots: Vec<Robot> = reader.lines()
                                .flatten()
                                .map(parse_robot)
                                .collect();

    let mut best_iteration = 0;
    let mut best_adjacencies = 0;

    // TODO: This can be made faster by reusing the grid across iterations.
    for i in 0..10000 {
        // print!("Seconds: {}\n", i);
        let updated: Vec<(usize, usize)> = robots.iter()
                                                .map(|r| r.forecast(bounds, i))
                                                .collect();
        let counts = grid_counts(&updated, bounds);
        let adjacencies = count_adjacencies(&counts, bounds);
        if adjacencies > best_adjacencies {
            best_iteration = i;
            best_adjacencies = adjacencies;
        }
        // print_pos(&updated, bounds);
    }

    let updated: Vec<(usize, usize)> = robots.iter()
                                            .map(|r| r.forecast(bounds, best_iteration))
                                            .collect();
    let counts = grid_counts(&updated, bounds);
    print!("Seconds: {}\n", best_iteration);
    print_pos(&counts, bounds);

    // let updated: Vec<(usize, usize)> = robots.iter()
    //                                         .map(|r| r.forecast(bounds, 100))
    //                                         .collect();
    // // print_pos(&updated, bounds);

    // // print!("Updated: {:#?}\n", updated);
    // let quadrants: Vec<(usize, usize)> = (0..2).map(move |x| (0..2).map(move |y| (x, y))).flatten().collect();
    // // print!("Quadrants: {:#?}\n", quadrants);
    // let mut quadrant_count: HashMap<(usize, usize), usize> = quadrants.iter()
    //                                         .fold(HashMap::<(usize, usize), usize>::new(),
    //                                             |mut acc, quad| { acc.entry(*quad).or_insert(0); acc } );

    // let quads: Vec<Option<(usize, usize)>> = updated.iter()
    //     .map(|pos| get_quad(pos.clone(), bounds) )
    //     .collect();
    // // print!("Quads: {:#?}\n", quads);


    // updated.iter()
    //     .map(|pos| get_quad(pos.clone(), bounds) )
    //     .flatten()
    //     .for_each(|quad| { quadrant_count.entry(quad).and_modify(|q| *q += 1); });

    // // print!("Quadrant counts: {:#?}\n", quadrant_count);

    // print!("{}\n", quadrant_count.values().product::<usize>());

    Ok(())
}
