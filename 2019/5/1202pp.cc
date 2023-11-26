#include "1202pp.h"

Program::Program(const std::string& filepath) :
  program(program_from_text_filepath(filepath.c_str())) {}

Program::~Program() {
  destroy_program(program);
}

Process::Process(const Program& program) :
  process(instantiate_process(program.program)) {}

Process::~Process() {
  destroy_process(process);
}

void Process::write(int64_t value) {
  buffer_write(process->input, value);
}

int64_t Process::read() {
  return buffer_read(process->output);
}

bool Process::halted() const {
  return process->status == HALTED;
}

// TODO: Rename the C function to process_execute so this can just be execute.
process_status Process::exec() {
  return execute(process);
}

void Process::UntilAwaitingWriteIterator::fetch() {
  if (!is_eof && !value_cached) {
    is_eof = !execute_and_read(process, &value);
    value_cached = true;
  }
}

Process::UntilAwaitingWriteIterator::UntilAwaitingWriteIterator(process_t *process) :
  value(0),
  value_cached(false),
  is_eof(false),
  process(process) {}

bool Process::UntilAwaitingWriteIterator::operator==(const UntilAwaitingWriteIterator& other) {
  if (this->is_eof && other.is_eof) {
    return true;
  }

  // No other form of equality makes sense here.
  return false;
}

bool Process::UntilAwaitingWriteIterator::operator!=(const UntilAwaitingWriteIterator& other) {
  return !(*this == other);
}

Process::UntilAwaitingWriteIterator& Process::UntilAwaitingWriteIterator::operator++() {
  if (is_eof) {
    fprintf(stderr, "Attempt to advance iterator after EOF received.\n");
    exit(1);
  }
  fetch(); // In case operator* was never called.
  value = 0;
  value_cached = false;
  return *this;
}

int64_t Process::UntilAwaitingWriteIterator::operator*() {
  fetch();
  return value;
}

Process::UntilAwaitingWriteIterator Process::UntilAwaitingWriteIterator::end_sentinel() {
  UntilAwaitingWriteIterator it(nullptr);
  it.is_eof = true;
  return it;
}

Process::UntilAwaitingWriteIterator Process::begin() {
  return UntilAwaitingWriteIterator(process);
}

Process::UntilAwaitingWriteIterator Process::end() {
  return UntilAwaitingWriteIterator::end_sentinel();
}
