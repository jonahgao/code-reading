#include <iostream>
#include <rocksdb/db.h>

void ingest(rocksdb::DB *db) {
  rocksdb::EnvOptions env_opt;
  rocksdb::Options opt;
  rocksdb::SstFileWriter writer(env_opt, opt);
  auto rs = writer.Open("./ingest.tmp");
  if (!rs.ok()) {
    std::cerr << "open ingest writer error: " << rs.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  writer.Put("a", "value_of_a");
  writer.Put("m", "value_of_m");
//  char key[32] = {'\0'};
//  snprintf(key, 32, "key-%05d", 100);
//  writer.Put(key, "value_of_m");

  rocksdb::ExternalSstFileInfo file_info;
  rs = writer.Finish(&file_info);
  if (!rs.ok()) {
    std::cerr << "finish ingest writer error: " << rs.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }

  rocksdb::IngestExternalFileOptions ingest_opt;
  ingest_opt.move_files = true;
  ingest_opt.write_global_seqno = false;
  auto begin = std::chrono::system_clock::now();
  rs = db->IngestExternalFile({"./ingest.tmp"}, ingest_opt);
  if (!rs.ok()) {
    std::cerr << "ingest file error: " << rs.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  auto end = std::chrono::system_clock::now();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
  std::cout << "ingest elapsed ms: " << elapsed_ms << "ms." << std::endl;

  std::string value;
  rs = db->Get(rocksdb::ReadOptions(), "a", &value);
  if (!rs.ok()) {
    std::cerr << rs.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "value: " << value << std::endl;

  rs = db->Get(rocksdb::ReadOptions(), "m", &value);
  if (!rs.ok()) {
    std::cerr << rs.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "value: " << value << std::endl;
}

int main() {
  rocksdb::Options ops;
  ops.create_if_missing = true;
  ops.write_buffer_size = 128*1024*1024;
  rocksdb::DB *db = nullptr;
  const char* path = "./data";
  auto ret = rocksdb::DB::Open(ops, path, &db);
  if (!ret.ok()) {
    std::cerr << "open db " << path << " failed: " << ret.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 1000 * 8 * 60; ++i) {
    char key[32] = {'\0'};
    snprintf(key, 32, "key-%05d",i);
    std::string val(128, 'v');
    auto s = db->Put(rocksdb::WriteOptions(), key, val);
    if (!s.ok()) {
      std::cerr << s.ToString() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto s = db->Put(rocksdb::WriteOptions(), "g", "value_of_g");
  if (!s.ok()) {
    std::cerr << s.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string value;
  s = db->Get(rocksdb::ReadOptions(), "g", &value);
  if (!s.ok()) {
    std::cerr << s.ToString() << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "value: " << value << std::endl;
//  auto snapshot = db->GetSnapshot();
//  std::cout << snapshot->GetSequenceNumber() << std::endl;

  ingest(db);

  return 0;
}
