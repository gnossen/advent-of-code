#ifndef __1202PP__
#define __1202PP__

#include "1202.h"
#include <string>

// C++ convenience wrappers.

class Program {
public:
  program_t program;

  Program(const std::string& filepath);
  ~Program();
};

class Process {
private:

  class UntilAwaitingWriteIterator {
  private:
    int64_t value;
    bool value_cached;
    bool is_eof;
    process_t *process;

    void fetch();

  public:
    UntilAwaitingWriteIterator(process_t *process);

    bool operator==(const UntilAwaitingWriteIterator& other);
    bool operator!=(const UntilAwaitingWriteIterator& other);
    UntilAwaitingWriteIterator& operator++();
    int64_t operator*();

    static UntilAwaitingWriteIterator end_sentinel();
  };

public:
  process_t *process;

  Process(const Program& program);
  ~Process();

  void write(int64_t value);
  int64_t read();
  process_status exec();

  bool halted() const;

  UntilAwaitingWriteIterator begin();
  UntilAwaitingWriteIterator end();
};
#endif // __1202PP__
