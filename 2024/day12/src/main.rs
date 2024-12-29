use std::env;
use std::fs;
use std::vec::Vec;
use std::collections::HashSet;
use std::cmp;

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

    // pub fn region_kind(&self) -> char {
    //     self.kind
    // }
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

// TODO: Only build adjacency graphs once for the entire grid, rather than once per region.
fn count_edges(region: &HashSet<(usize, usize)>) -> usize {
    // x_max, y_max
    let maxes: (usize, usize) = region.iter()
                                        .fold((0, 0),
                                              |acc, x| (cmp::max(acc.0, x.0), cmp::max(acc.1, x.1)));
    let bounds = (maxes.0 + 1, maxes.1 + 1);

    let mut adjacencies: Vec<Vec<Vec<bool>>> =
        DIRS.iter().map( |_dirs|
            (0..bounds.1).map( |_y|
                (0..bounds.0).map( |_x| false).collect()
            ).collect()
        ).collect();

    // Calculate adjacencies
    for cell in region.iter() {
        for (dir_index, dir) in DIRS.iter().enumerate() {
            // TODO: Clean this up.
            if let Some(adj) = adjacent(*cell, *dir, bounds.0, bounds.1) {
                if !region.contains(&adj) {
                    adjacencies[dir_index][cell.1][cell.0] = true;
                }
            } else {
                adjacencies[dir_index][cell.1][cell.0] = true;
            }
        }
    }


    // for (dir_index, _dir) in DIRS.iter().enumerate() {
    //     for row in adjacencies[dir_index].iter() {
    //         for col in row.iter() {
    //             if *col {
    //                 print!("1 ");
    //             } else {
    //                 print!("0 ");
    //             }
    //         }
    //         print!("\n");
    //     }
    //     print!("\n");
    // }

    let mut side_count = 0;
    for (dir_index, _dir) in DIRS.iter().enumerate() {
        let mut unvisited = region.clone();
        while let Some(c) = unvisited.iter().next() {
            // print!("  Pulling cell ({}, {})\n", c.0, c.1);
            let mut side_length = 0;
            let mut to_visit: Vec<(usize, usize)> = vec![*c];
            while let Some(n) = to_visit.pop() {
                // print!("  Inspecting cell ({}, {})\n", n.0, n.1);
                unvisited.remove(&n);
                if !adjacencies[dir_index][n.1][n.0] {
                    continue;
                }
                side_length += 1;
                for adj in adjacent_cells(n, bounds.0, bounds.1) {
                    if unvisited.contains(&adj) {
                        to_visit.push(adj);
                    }
                }
            }

            if side_length > 0 {
                side_count += 1;
                // print!("  Counted {side_count} sides\n");
            }
        }
    }

    // print!("Total {side_count} sides\n");
    side_count
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

    let mut regions = Regions::new(&grid);

    let mut total_price: usize = 0;

    while let Some(mut region) = regions.next() {
        let mut region_cells: HashSet<(usize, usize)> = HashSet::new();
        let mut area = 0;

        // Can this just be a collect method call followed by a len method call?
        while let Some(cell) = region.next() {
            area += 1;
            region_cells.insert(cell);
        }

        total_price += count_edges(&region_cells) * area;
    }

    println!("{total_price}");
}
