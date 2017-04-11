#include <iostream>
#include <random>
#include "leveldb/db.h"
#include "leveldb/cache.h"

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

void testDeleter(const Slice& key, void *value) {
    std::string *s = reinterpret_cast<string*>(value);
    delete s;
}

void testLRU() {
    auto c = NewLRUCache(100);
    assert(c != NULL);

    auto v1 = new string("value1");
    auto h1 = c->Insert(Slice("a"), v1, 40, testDeleter);
    assert(h1 != NULL);

    auto v2 = new string("value2");
    auto h2 = c->Insert(Slice("b"), v2, 40, testDeleter);
    assert(h2 != NULL);

    auto v3 = new string("value3");
    auto h3 = c->Insert(Slice("c"), v3, 40, testDeleter);
    assert(h3 != NULL);

    cout << c->TotalCharge() << endl;
}

int main() {
    testLRU();
    return 0;

    DB *db = nullptr;
    Options ops;
    ops.create_if_missing = true;
    auto status = DB::Open(ops, "/tmp/leveldb_test", &db);
    if (!status.ok()) {
        cerr << "open db failed: " << status.ToString() << endl;
        return -1;
    }

    for (int i = 0; i < 10000; i++) {
        string key;
        string value;
        randomString(128, key);
        randomString(2048, value);
        WriteOptions wops;
        auto s = db->Put(wops, key, value);
        if (!s.ok()) {
            cerr << "write error: " << s.ToString() << endl;
            return -1;
        }
    }

    Slice s("\x00"), e("\xFF");
    db->CompactRange(&s, &e);

    getchar();
}
