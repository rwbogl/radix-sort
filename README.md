# The Radix Sort

This is an implementation of the radix sort described in _The Art of Computer
Programming: Sorting and Searching_ section 5.2.5. The radix sort is
interesting in that it may be as close as sorts come to operating in O(n) time.
For a general overview, see [notes.pdf](./notes.pdf). For a in-depth
explanation, see the chapter in TAOCP.

Here are example runs on sorting random data compared to the C library's
`qsort` function. The first set only differs in the radix used:

    ./radix
    radix: 10
    n: 100000
    range: [0, 100000)
    length in base-10: 5
    radix sort time: 60.000000ms
    qsort time: 38.000000ms

    ./radix -r 2
    radix: 2
    n: 100000
    range: [0, 100000)
    length in base-2: 17
    radix sort time: 206.000000ms
    qsort time: 38.000000ms

    ./radix -r 100
    radix: 100
    n: 100000
    range: [0, 100000)
    length in base-100: 3
    radix sort time: 38.000000ms
    qsort time: 38.000000ms

    ./radix -r 1000
    radix: 1000
    n: 100000
    range: [0, 100000)
    length in base-1000: 2
    radix sort time: 26.000000ms
    qsort time: 38.000000ms


And now with much shorter numbers:

    ./radix -r 100 -l 3
    radix: 100
    n: 100000
    range: [0, 1000)
    length in base-100: 2
    radix sort time: 24.000000ms
    qsort time: 37.000000ms

    ./radix -r 10 -l 3
    radix: 10
    n: 100000
    range: [0, 1000)
    length in base-10: 3
    radix sort time: 35.000000ms
    qsort time: 37.000000ms

    ./radix -r 2 -l 2
    radix: 2
    n: 100000
    range: [0, 100)
    length in base-2: 7
    radix sort time: 65.000000ms
    qsort time: 36.000000ms

    ./radix -r 10 -l 2
    radix: 10
    n: 100000
    range: [0, 100)
    length in base-10: 2
    radix sort time: 22.000000ms
    qsort time: 36.000000ms
