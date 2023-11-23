use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::fmt;

#[derive(Default)]
struct StackArrangement {
    stacks: Vec<Vec<char>>,
}

impl StackArrangement {
    fn new() -> Self {
        Default::default()
    }

    fn add_stack(& mut self, stack: Vec<char>) {
        self.stacks.push(stack.clone())
    }

    fn height(&self) -> usize {
        let mut max: usize = 0;
        for i in 0..self.stacks.len() {
            if self.stacks[i].len() > max {
                max = self.stacks[i].len();
            }
        }
        max
    }

    fn move_one(& mut self, from: usize, to: usize) {
        let c: char = self.stacks[from].pop().unwrap();
        self.stacks[to].push(c)
    }

    fn move_multiple(& mut self, count: usize, from: usize, to: usize) {
        let mut tmp: Vec<char> = Vec::new();
        for _ in 0..count {
            tmp.push(self.stacks[from].pop().unwrap());
        }
        tmp.reverse();
        for i in 0..count {
            self.stacks[to].push(tmp[i]);
        }
    }

    fn tops(&self) -> String {
        let mut s: String = String::new();
        for i in 0..self.stacks.len() {
            if self.stacks[i].len() == 0 {
                continue;
            }
            s.push(self.stacks[i].last().unwrap().clone());
        }
        s
    }
}

impl fmt::Display for StackArrangement {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let height = self.height();
        for cur_height in (0..height).rev() {
            for stack_index in 0..self.stacks.len() {
                if cur_height >= self.stacks[stack_index].len() {
                    write!(f, "   ")?;
                } else {
                    write!(f, "[{}]", self.stacks[stack_index][cur_height])?;
                }
                write!(f, " ")?;
            }
            write!(f, "\n")?;
        }
        for stack_index in 0..self.stacks.len() {
            write!(f, " {}  ", stack_index + 1)?;
        }
        write!(f, "\n")?;
        Ok(())
    }
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn parse_command(cmd_str: &str) -> (usize, usize, usize) {
    let tokens: Vec<&str> = cmd_str.split(" ").collect();
    assert!(tokens.len() == 6, "Failed to parse command '{}'", cmd_str);
    let count: usize = tokens[1].parse::<usize>().unwrap();
    let from: usize = tokens[3].parse::<usize>().unwrap();
    let to: usize = tokens[5].parse::<usize>().unwrap();
    (count, from, to)
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut stacks: Vec<Vec<char>> = Vec::new();
    let mut stack_width: usize = 0;
    let mut stacks_read: bool = false;
    let mut sa: StackArrangement = StackArrangement::new();
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(stack_line) = line {
                if !stacks_read {
                    // Read in the stacks.
                    if stack_width == 0 {
                        assert!((stack_line.len() + 1) % 4 == 0, "Line width {} + 1 is not divisible by 4.", stack_line.len());
                        stack_width = (stack_line.len() + 1) / 4;
                        for _ in 0..stack_width {
                            stacks.push(Vec::new());
                        }
                    }

                    // Now, determine whether this is the label line.
                    if !stack_line.contains("[") {
                        // Create the StackArrangement.
                        for i in 0..stack_width {
                            stacks[i].reverse();
                            sa.add_stack(stacks[i].clone());
                        }
                        stacks_read = true;
                    }

                    let line_bytes = stack_line.as_bytes();

                    for i in 0..stack_width {
                        let cursor = (i*4) + 1;
                        assert!(cursor < line_bytes.len());
                        let c = line_bytes[cursor] as char;
                        if c != ' ' {
                            stacks[i].push(c);
                        }
                    }
                } else {
                    // Now read instructions.

                    // Tolerate newlines.
                    if stack_line.len() == 0 {
                        continue;
                    }

                    let (count, from, to) = parse_command(&stack_line);
                    sa.move_multiple(count, from - 1, to - 1);
                    println!("{}", sa)
                }
            } else {
                panic!("Failed to read line.");
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }
    println!("{}", sa);
    println!("{}", sa.tops());
}
