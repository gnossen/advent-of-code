use std::env;
use std::fs;
use regex::Regex;

#[derive(Debug)]
struct Button {
    movement: (usize, usize),
    cost: usize,
}

#[derive(Debug)]
struct Machine {
    a_button: Button,
    b_button: Button,
    prize_location: (usize, usize),
}

impl Machine {
    fn winning_moves(&self) -> Option<(usize, usize)> {
        let denominator = ((self.a_button.movement.0 * self.b_button.movement.1) as i64) - ((self.a_button.movement.1 * self.b_button.movement.0) as i64);
        if denominator == 0 {
            return None
        }

        let a_numerator = ((self.b_button.movement.1 * self.prize_location.0) as i64) - ((self.b_button.movement.0 * self.prize_location.1) as i64);
        if a_numerator % denominator != 0 {
            return None;
        }

        let a_movements_signed = a_numerator / denominator;
        if a_movements_signed < 0 {
            return None;
        }

        let b_numerator = ((self.a_button.movement.0 * self.prize_location.1) as i64) - ((self.a_button.movement.1 * self.prize_location.0) as i64);
        if b_numerator % denominator != 0 {
            return None;
        }

        let b_movements_signed = b_numerator / denominator;
        if b_movements_signed < 0 {
            return None;
        }

        Some((
            a_movements_signed as usize,
            b_movements_signed as usize,
        ))
    }

    fn winning_cost(&self) -> Option<usize> {
        let moves = self.winning_moves()?;
        Some(moves.0 * self.a_button.cost + moves.1 * self.b_button.cost)
    }
}

fn parse_button<'a>(name: impl AsRef<str>, cost: usize, lines: &mut impl Iterator<Item = &'a str>) -> Option<Button> {
    // TODO: Move to static scope.
    let button_line = Regex::new(r"^Button (.): X\+([0-9]+), Y\+([0-9]+)$").unwrap();
    let (_, [button_name, x, y])  = button_line.captures(lines.next()?).unwrap().extract();
    assert_eq!(button_name, name.as_ref());
    Some(Button {
        movement: (x.parse().unwrap(), y.parse().unwrap()),
        cost,
    })
}

fn parse_prize<'a>(lines: &mut impl Iterator<Item = &'a str>) -> Option<(usize, usize)> {
    // TODO: Move to static scope.
    let button_line = Regex::new(r"^Prize: X=([0-9]+), Y=([0-9]+)$").unwrap();
    let (_, [x, y])  = button_line.captures(lines.next()?).unwrap().extract();
    let raw_x: usize = x.parse().unwrap();
    let raw_y: usize = y.parse().unwrap();
    Some((raw_x + 10000000000000, raw_y + 10000000000000))
}

fn parse_machine<'a>(lines: &mut impl Iterator<Item = &'a str>) -> Option<Machine> {
    let a_button = parse_button("A", 3, lines)?;
    let b_button = parse_button("B", 1, lines).unwrap();
    let prize_location = parse_prize(lines).unwrap();
    let _new_line = lines.next();
    // TODO: Add assertion that new_line is in fact an empty new line.
    Some(Machine {
        a_button,
        b_button,
        prize_location,
    })
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        panic!("Usage: {} FILENAME", args[0]);
    }

    let filename = args[1].clone();

    let contents = fs::read_to_string(filename).unwrap();
    let mut lines = contents.lines();

    let mut machines = Vec::<Machine>::new();

    // TODO: Can we simplify this with iterator chaining?
    while let Some(machine) = parse_machine(&mut lines) {
        machines.push(machine);
    }

    // print!("{:#?}\n", machines)
    // for machine in machines {
    //     print!("Winning moves: {:#?}\n", machine.winning_moves());
    //     print!("Cost: {:#?}\n", machine.winning_cost())
    // }

    let total_cost: usize = machines.iter()
                                .map(|m| m.winning_cost() )
                                .flatten()
                                .sum();
    print!("{total_cost}\n");
}
