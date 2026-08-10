#include <optional>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

using String = std::string;

template <class T>
using Vec = std::vector<T>;

template <class K, class V>
using Map = std::unordered_map<K, V>;

template <class T>
using Maybe = std::optional<T>;

template <class T>
using Box = std::unique_ptr<T>;

template <class T>
using Rc = std::shared_ptr<T>;

template <class T>
using Weak = std::weak_ptr<T>;
