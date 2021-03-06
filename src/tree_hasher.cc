#include "merkletree/tree_hasher.h"

#include <assert.h>

#include "merkletree/serial_hasher.h"

using std::lock_guard;
using std::move;
using std::mutex;
using std::string;
using std::unique_ptr;
using namespace merkletree;

namespace {

const char kLeafPrefix('\x00');
const char kNodePrefix('\x01');

std::string EmptyHash(SerialHasher* hasher) {
  hasher->Reset();
  return hasher->Final();
}

}  // namespace

TreeHasher::TreeHasher(unique_ptr<SerialHasher> hasher)
    : hasher_(move(hasher)), empty_hash_(EmptyHash(hasher_.get())) {
  assert(hasher_);
}

string TreeHasher::HashLeaf(const string& data) const {
  lock_guard<mutex> lock(lock_);
  hasher_->Reset();
  hasher_->Update(string(1, kLeafPrefix));
  hasher_->Update(data);
  return hasher_->Final();
}

string TreeHasher::HashChildren(const string& left_child,
                                const string& right_child) const {
  lock_guard<mutex> lock(lock_);
  hasher_->Reset();
  hasher_->Update(string(1, kNodePrefix));
  hasher_->Update(left_child);
  hasher_->Update(right_child);
  return hasher_->Final();
}
