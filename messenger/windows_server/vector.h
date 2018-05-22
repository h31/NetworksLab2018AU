#ifndef TCP_VECTOR_H
#define TCP_VECTOR_H
#include <cstddef>
#include <algorithm>

namespace elegram {
    template <class T>
    class vector {
    public:
        vector()
            : _capacity(64)
            , _size(0)
            , _data(new T[64]) {

        }

        explicit vector(size_t n)
            : _capacity(n)
            , _size(0)
            , _data(new T[n]) {

        }
        virtual ~vector() {
            delete []_data;
        }

        void push_back(const T& value) {
            if (this->_size == this->_capacity) {
                this->_capacity <<= 1;
                auto * tmp = new T[this->_capacity];
                std::copy(this->_data, this->_data + this->_size, tmp);
            }
            this->_data[this->_size] = value;
            ++this->_size;
        }

        void push_back(T&& value) {
            if (this->_size == this->_capacity) {
                this->_capacity <<= 1;
                auto * tmp = new T[this->_capacity];
                std::move(this->_data, this->_data + this->_size, tmp);
            }
            this->_data[this->_size] = std::move(value);
            ++this->_size;
        }

        void pop_back() {
            if (is_empty()) {
                --this->_size;
            }
        }

        int find(const T& value) {
            for (size_t i = 0; i < this->_size; ++i) {
                if (this->_data[i] == value) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        void erase(size_t index) {
            if (index < this->_size) {
                for (size_t i = index + 1; i < this->_size; ++i) {
                    _data[i - 1] = _data[i];
                }
                --this->_size;
            }
        }

        bool is_empty() const {
            return static_cast<bool>(this->_size);
        }

        size_t size() const {
            return _size;
        }

        T& operator[] (size_t index) const {
            return _data[index];
        }

        T* data() const {
            return _data;
        }

    private:
        T* _data;
        size_t _size;
        size_t _capacity;
    };
}

#endif //TCP_VECTOR_H
