#include <iostream>
#include <random>
#include "leveldb/db.h"

using namespace std;
using namespace leveldb;

void randomString(size_t length, string& out) {
    static const char* charPool = "01234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
    thread_local static std::default_random_engine engine(std::random_device{}());
    thread_local static std::uniform_int_distribution<int> dist(0, strlen(charPool)-1);

    out.reserve(length);
    for (size_t i = 0; i < length; i++) {
        out.push_back(dist(engine));
    }
}

int main() {
    DB *db = nullptr;
    Options ops;
    ops.create_if_missing = true;
    auto status = DB::Open(ops, "/tmp/leveldb_test", &db);
    if (!status.ok()) {
        cerr << "open db failed: " << status.ToString() << endl;
        return -1;
    }

//    for (int i = 0; i < 10000; i++) {
//        string key;
//        string value;
//        randomString(128, key);
//        randomString(2048, value);
//        WriteOptions wops;
//        auto s = db->Put(wops, key, value);
//        if (!s.ok()) {
//            cerr << "write error: " << s.ToString() << endl;
//            return -1;
//        }
//    }

    Slice s("\x00"), e("\xFF");
    db->CompactRange(&s, &e);

    getchar();
}
