#include <iostream>
#include <rocksdb/db.h>

int main() {
  rocksdb::Options ops;
  ops.create_if_missing = true;
  //ops.error_if_exists = tru;
  rocksdb::DB *db = nullptr;
  const char* path = "./data";
  auto ret = rocksdb::DB::Open(ops, path, &db);
  if (!ret.ok()) {
    std::cerr << "open db " << path << " failed: " << ret.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto s = db->Put(rocksdb::WriteOptions(), "a", "1");
  if (!s.ok()) {
    std::cerr << s.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string value;
  s = db->Get(rocksdb::ReadOptions(), "a", &value);
  if (!s.ok()) {
    std::cerr << s.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "value: " << value << std::endl;

  return 0;
}