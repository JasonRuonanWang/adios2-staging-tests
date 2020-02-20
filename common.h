
template <class T>
void GenDataRecursive(std::vector<size_t> start, std::vector<size_t> count,
                      std::vector<size_t> shape, size_t n0, size_t y,
                      std::vector<T> &vec)
{
    for (size_t i = 0; i < count[0]; i++)
    {
        size_t i0 = n0 * count[0] + i;
        size_t z = y * shape[0] + (i + start[0]);

        auto start_next = start;
        auto count_next = count;
        auto shape_next = shape;
        start_next.erase(start_next.begin());
        count_next.erase(count_next.begin());
        shape_next.erase(shape_next.begin());

        if (start_next.size() == 1)
        {
            for (size_t j = 0; j < count_next[0]; j++)
            {
                vec[i0 * count_next[0] + j] =
                    z * shape_next[0] + (j + start_next[0]);
            }
        }
        else
        {
            GenDataRecursive(start_next, count_next, shape_next, i0, z, vec);
        }
    }
}

template <class T>
void GenData(std::vector<T> &vec, const size_t step,
             const std::vector<size_t> &start, const std::vector<size_t> &count,
             const std::vector<size_t> &shape)
{
    size_t total_size = std::accumulate(count.begin(), count.end(), 1,
                                        std::multiplies<size_t>());
    vec.resize(total_size);
    GenDataRecursive(start, count, shape, 0, 0, vec);
}

