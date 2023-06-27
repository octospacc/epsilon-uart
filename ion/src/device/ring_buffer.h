#ifndef ION_DEVICE_RING_BUFFER_H
#define ION_DEVICE_RING_BUFFER_H

template<typename T, int N>
struct RingBuffer {
  T m_buffer[N];
  int m_start;
  int m_end;

  RingBuffer() :
    m_start(0),
    m_end(0)
  {}

  inline int next(int index) const volatile {
    return (index + 1) % N;
  }

  inline bool empty() volatile {
    return m_end == m_start;
  }

  inline bool full() volatile {
    return next(m_end) == m_start;
  }

  inline void push(T c) volatile {
    m_buffer[m_end] = c;
    m_end = next(m_end);
  }

  inline T shift() volatile {
    T c = m_buffer[m_start];
    m_start = next(m_start);
    return c;
  }
};

#endif