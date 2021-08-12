/*
The MIT License (MIT)

Copyright (c) 2017 David Hasenfratz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stddef.h>

template <typename T, size_t Size>
class CircularBuffer {

public:
    CircularBuffer();
    /** Add elemen to buffer */
    void add(T value);
    /** Get element at position pos */
    T get(int pos);
    /** Clear buffer */
    void clear();

private:
    T _buffer[Size];
    size_t _start;
    size_t _end;
    bool _full;
};

template <typename T, size_t Size>
CircularBuffer<T, Size>::CircularBuffer() : _start(0), _end(0), _full(false) {}

template <typename T, size_t Size>
void CircularBuffer<T, Size>::add(T value)
{
    _buffer[_end++] = value;

    if (_end >= Size)
        _end = 0;

    if (_full)
        _start = _end;
    else if (_end == _start)
        _full = true;
}

template <typename T, size_t Size>
T CircularBuffer<T, Size>::get(int pos)
{
  return _buffer[(_start + pos)%Size];
}

template <typename T, size_t Size>
void CircularBuffer<T, Size>::clear()
{
    _full = false;
    _start = 0;
    _end = 0;
}

#endif
