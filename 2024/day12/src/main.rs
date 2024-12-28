use std::env;
use std::fs;
use std::vec::Vec;
use std::collections::HashSet;

const DIRS: &[(i64, i64)] = &[
    (-1, 0),
    (0, -1),
    (1, 0),
    (0, 1),
];

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

fn adjacent(c: (usize, usize), dir: (i64, i64), width: usize, height: usize) -> Option<(usize, usize)> {
    let x = adjacent_dim(c.0, dir.0, width)?;
    let y = adjacent_dim(c.1, dir.1, height)?;
    Some((x, y))
}

fn adjacent_cells(x: (usize, usize), width: usize, height: usize) -> impl Iterator<Item = (usize, usize)> {
    DIRS.iter()
        .map(move |dir| adjacent(x, dir.clone(), width, height))
        .flatten()
}

struct Region<'a> {
    grid: & 'a Vec<Vec<char>>,
    visited: & 'a mut Vec<Vec<bool>>,
    stack: Vec<(usize, usize)>,
    kind: char,
    width: usize,
    height: usize,
}

impl<'a> Region<'a> {
    pub fn new(grid: &'a Vec<Vec<char>>, visited: &'a mut Vec<Vec<bool>>, initial: (usize, usize), kind: char, width: usize, height: usize) -> Region<'a> {
        Region {
            grid,
            visited,
            stack: vec![initial],
            kind,
            width,
            height,
        }

    }

    // TODO: Consider moving to a separate impl for trait Iterator.
    pub fn next(&mut self) -> Option<(usize, usize)> {
        loop {
            if self.stack.is_empty()  {
               return None;
            }

            // Guaranteed to have the proper kind, but not guaranteed to not have been visited.
            let next = self.stack.pop().unwrap();
            if self.visited[next.1][next.0] {
                continue
            }

            // TODO: Figure out how not to duplicate the lookup.
            self.visited[next.1][next.0] = true;

            for candidate in adjacent_cells(next, self.width, self.height) {
                if self.grid[candidate.1][candidate.0] == self.kind && !self.visited[candidate.1][candidate.0] {
                    self.stack.push(candidate);
                }
            }

            return Some(next);
        }
    }

    pub fn region_kind(&self) -> char {
        self.kind
    }
}

struct Regions<'a> {
    grid: &'a Vec<Vec<char>>,
    visited: Vec<Vec<bool>>,
    next: (usize, usize),
    width: usize,
    height: usize,
}

impl<'a> Regions<'a> {

    pub fn new(grid: &Vec<Vec<char>>) -> Regions {
        let grid_width = grid.get(0).unwrap().len();
        Regions {
            grid,
            visited: (0..grid.len())
                        .map(|_x| (0..grid_width).map(|_y| false).collect())
                        .collect(),
            next: (0, 0),
            width: grid_width,
            height: grid.len(),
        }
    }

    // TODO: Consider moving to a separate impl for trait Iterator.
    pub fn next(&mut self) -> Option<Region> {
        loop {

            if self.next_visited() {
                if !self.advance() {
                    return None
                }
                continue;
            }

            let kind = self.grid[self.next.1][self.next.0];
            return Some(Region::new(self.grid, &mut self.visited, self.next, kind, self.width, self.height))
        }
    }

    fn next_visited(&self) -> bool {
        self.visited[self.next.1][self.next.0]
    }

    // returns true if not at end, false otherwise.
    fn advance(&mut self) -> bool {
        if self.next.0 < (self.width - 1) {
            self.next.0 += 1;
            return true;
        } else if self.next.1 < (self.height - 1) {
            self.next.0 = 0;
            self.next.1 += 1;
            return true;
        } else {
            return false;
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        let mut program_name = String::from("unknown");
        if let Some(actual_program_name) = args.get(0) {
            program_name = actual_program_name.clone();
        }
        panic!("Usage: {} FILENAME", program_name);
    }

    let filename = args.get(1).unwrap();

    let grid: Vec<Vec<char>> = fs::read_to_string(filename)
        .unwrap()
        .lines()
        .map(|x| x.trim())
        .map(|x| x.chars().collect())
        .collect();

    let grid_height = grid.len();
    let grid_width = grid[0].len();

    let mut regions = Regions::new(&grid);

    let mut total_price: usize = 0;

    // TODO: Figure out if this can be made better.
    loop {
        if let Some(mut region) = regions.next() {
            let mut region_cells = HashSet::new();
            let mut area = 0;

            loop {
                if let Some(cell) = region.next() {
                    area += 1;
                    region_cells.insert(cell);
                } else {
                    break;
                }
            }

            let perimeter: usize = region_cells.iter()
                                    .map(|cell| 4 - adjacent_cells(cell.clone(), grid_width, grid_height)
                                                    .filter(|adj| region_cells.contains(adj))
                                                    .count())
                                    .sum();

            total_price += area * perimeter;

        } else {
            break;
        }
    }

    println!("{total_price}");
}
