#pragma once
namespace procrock {
namespace utils {
template <typename Vector>
auto splitVector(const Vector& v, unsigned elementsPerVector) {
  using Iterator = typename Vector::const_iterator;
  std::vector<Vector> rtn;
  Iterator it = v.cbegin();
  const Iterator end = v.cend();

  while (it != end) {
    Vector v;
    std::back_insert_iterator<Vector> inserter(v);
    const auto num_to_copy =
        std::min(static_cast<unsigned>(std::distance(it, end)), elementsPerVector);
    std::copy(it, it + num_to_copy, inserter);
    rtn.push_back(std::move(v));
    std::advance(it, num_to_copy);
  }

  return rtn;
}
}  // namespace utils
}  // namespace procrock
