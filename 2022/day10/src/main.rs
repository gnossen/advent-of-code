use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

const END_CYCLE: usize = 220;

#[derive(Clone,Copy)]
struct Instruction {
    arg0: u64,
    arg1: i64,
}

const NO_OP_OPCODE: u64 = 0;
const ADD_X_OPCODE: u64 = 1;

struct CPU {
    x: i64,
    ip: u64,
    instructions: Vec<Instruction>,
    tick: u64,

    // How long the current instruction has been in progress.
    ticks_in_progress: u64,
}

impl CPU {
    pub fn new() -> Self {
        Self {
            x: 1,
            ip: 0,
            instructions: Vec::new(),
            tick: 1,
            ticks_in_progress: 0,
        }
    }

    pub fn push_instruction(&mut self, inst: Instruction) {
        self.instructions.push(inst)
    }

    fn current_instruction(&self) -> Instruction {
        self.instructions[self.ip as usize]
    }

    pub fn tick(&mut self) {
        let current_instruction = self.current_instruction();
        match current_instruction.arg0 {
            NO_OP_OPCODE => {
                self.ip += 1;
            },
            ADD_X_OPCODE => {
                match self.ticks_in_progress {
                    0 => {
                        self.ticks_in_progress += 1;
                    },
                    1 => {
                        self.ticks_in_progress = 0;
                        self.ip += 1;
                        self.x += current_instruction.arg1;
                    },
                    _ => {
                        assert!(false, "addx called for {}th consecutive tick", self.ticks_in_progress + 1)
                    }
                }
            },
            _ => assert!(false, "Unknown opcode {}", current_instruction.arg0),
        };
        self.tick += 1
    }
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn main() {
    let args: Vec<String>  = env::args().collect();
    assert!(args.len() == 2, "Supply one argument: the text file to read.");
    let mut cpu = CPU::new();
    if let Ok(lines) = read_lines(&args[1]) {
        for line in lines {
            if let Ok(inst_line) = line {
                let components: Vec<&str> = inst_line.split(" ").collect();
                let instruction = match components.len() {
                    1 => match components[0] {
                        "noop" => Instruction {
                            arg0: NO_OP_OPCODE,
                            arg1: 0,
                        },
                        _ => {
                            assert!(false, "Unrecognized instruction {}", components[0]);
                            Instruction{arg0: 0, arg1: 0}
                        },
                    },
                    2 => match components[0] {
                        "addx" => Instruction {
                            arg0: ADD_X_OPCODE,
                            arg1: components[1].parse::<i64>().unwrap(),
                        },
                        _ => {
                            assert!(false, "Unrecognized instruction {}", components[0]);
                            Instruction{arg0: 0, arg1: 0}
                        },
                    },
                    _ => {
                        assert!(false, "Failed to parse instruction '{}'", inst_line);
                        Instruction{arg0: 0, arg1: 0}
                    },
                };
                cpu.push_instruction(instruction);
            } else {
                panic!("Failed to read line.");
            }
        }
    } else {
        panic!("Failed to open {}", args[1]);
    }

    let mut total_signal_strength: i64 = 0;
    for cycle in 1..(END_CYCLE + 1) {
        if cycle >= 20 && ((cycle - 20) % 40) == 0 {
             let signal_strength = (cycle as i64) * cpu.x;
             total_signal_strength += signal_strength;
        }
        //println!("Tick {}: {}", cycle, cpu.x);
        cpu.tick();
    }
    println!("{}", total_signal_strength)
}
