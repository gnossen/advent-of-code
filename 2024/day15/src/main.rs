use std::fmt;
use std::env;
use std::fs;
use std::io::{ Error, ErrorKind };

const DIRS: [[i64; 2]; 4] = [
    [0, -1],
    [1, 0],
    [0, 1],
    [-1, 0],
];

const DIR_CHARS: [char; 4] = ['^', '>', 'v', '<'];

fn command_char_to_dir(c: char) -> Option<usize> {
    for (i, rc) in DIR_CHARS.iter().enumerate() {
        if c == *rc {
            return Some(i);
        }
    }
    None
}

fn add_dir(pos: [usize; 2], dir: [i64; 2]) -> [usize; 2] {
    let mut new_pos: [usize; 2] = [0, 0];
    for i in 0..2 {
        new_pos[i] = ((pos[i] as i64) + dir[i]) as usize;
    }
    new_pos
}

#[derive(Eq, PartialEq, Copy, Clone)]
enum GridState {
    Empty,
    Box,
    Wall,
    Robot,
}

struct Warehouse {
    bounds: [usize; 2],
    grid: Vec<Vec<GridState>>,

    // Technically, this is redundant since there should always be exactly one
    // grid cell of type GridState::Robot.
    robot: [usize; 2],
}

impl Warehouse {
    // TODO: Custom error type.
    fn new<'a>(s: impl AsRef<str>) -> Result<Warehouse, Error> {
        let mut w = Warehouse {
            bounds: [0, 0],
            grid: Vec::new(),
            robot: [0, 0],
        };

        let mut robot_populated = false;

        for (y, line) in s.as_ref().split("\n").enumerate() {
            let mut row = Vec::<GridState>::new();
            for (x, c) in line.chars().enumerate() {
                // TODO: Dedupe with the inverse mapping below.
                let state = match c {
                    '.' => GridState::Empty,
                    'O' => GridState::Box,
                    '#' => GridState::Wall,
                    '@' => {
                        if !robot_populated {
                            w.robot = [x, y];
                            robot_populated = true;
                        } else {
                            return Err(Error::new(ErrorKind::Other, "multiple robots detected"))
                        }
                        GridState::Robot
                    },
                    _ => {
                        return Err(Error::new(ErrorKind::Other, format!("unknown character {} detected", c)))
                    }
                };
                row.push(state);
            }
            if y == 0 {
                w.bounds[1] = row.len();
            } else {
                if row.len() != w.bounds[1] {
                    return Err(Error::new(ErrorKind::Other, "differing row widths detected"))
                }
            }
            w.grid.push(row);
        }

        w.bounds[0] = w.grid.len();

        Ok(w)
    }

    // Returns the new position.
    fn attempt_push(&mut self, pos: [usize; 2], dir: [i64; 2]) -> Option<[usize; 2]> {
        // Cannot push off the grid.
        for i in 0..2 {
            if pos[i] == 0 && dir[i] == -1 {
                return None;
            }
            if pos[i] == (self.bounds[i] - 1) && dir[i] == 1 {
                return None;
            }
        }
        let this_state = self.grid[pos[1]][pos[0]];
        if this_state == GridState::Wall {
            return None;
        }
        if this_state == GridState::Empty {
            return Some(pos);
        }
        let target = add_dir(pos, dir);
        if let Some(_new_pos) = self.attempt_push(target, dir) {
            self.grid[target[1]][target[0]] = this_state;
            self.grid[pos[1]][pos[0]] = GridState::Empty;
            return Some(target);
        } else {
            return None;
        }
    }

    fn update(&mut self, dir_index: usize) {
        let dir = DIRS[dir_index];
        if let Some(new_pos) = self.attempt_push(self.robot, dir) {
            self.robot = new_pos;
        }
    }
}

impl fmt::Debug for Warehouse {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let mut s: String = String::from("");
        for row in &self.grid {
            for col in row {
                match col {
                    GridState::Empty => s.push_str("."),
                    GridState::Box => s.push_str("O"),
                    GridState::Wall => s.push_str("#"),
                    GridState::Robot => s.push_str("@"),
                };
            }
            s.push_str("\n");
        }
        f.write_str(s.as_str())
    }
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        panic!("Usage: {} FILENAME", args[0]);
    }

    let filename = args[1].clone();

    let contents = fs::read_to_string(filename)?;

    let parts: Vec<&str> = contents.split("\n\n").collect();
    if parts.len() != 2 {
        return Err(Error::new(ErrorKind::Other, "unable to find two distinct text file regions"));
    }

    let mut warehouse = Warehouse::new(parts[0])?;
    // print!("{:?}\n", warehouse);

    let commands: Vec<usize> = parts[1].chars()
                                .filter(|c| *c != '\n')
                                .map(command_char_to_dir)
                                .map(|d| d.unwrap())
                                .collect();

    // print!("{:?}\n", commands);

    for command in commands {
        warehouse.update(command);
        // print!("{:?}\n", warehouse);
    }

    let score: usize = (0..warehouse.bounds[0]).map(
        move |x| (0..warehouse.bounds[1]).map(move |y| (x, y))
    ).flatten()
        .filter(|pos| warehouse.grid[pos.1][pos.0] == GridState::Box)
        .map(|pos| pos.1 * 100 + pos.0)
        .sum();

    print!("{}\n", score);

    Ok(())
}
