# Quiz2

### **測驗 `1`**

**Goal:** find the closest 2 to the power of n value, which should also be larger than 2.

- `next_pow2(7)` = 8
- `next_pow2(13)` = 16
- `next_pow2(42)` = 64

**Logic: Fill in 1s below the highest significant bit using `x |= x >> 1;`**

```c
x = 0000 0000 0000 0101 (5)
x = 0000 0000 0000 0111 (7) // add 1
x = 0000 0000 0000 1000 (8) // result
```

```c
uint64_t next_pow2(uint64_t x)
{
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> 1;
    x |= x >> AAAA; // AAAA = 8
    x |= x >> 16;
    x |= x >> BBBB; // BBBB = 21
    return CCCC;    // CCCC = x + 1
}
```

******************************************Simplified: one `|` can ensure 2 bits turn into 1, so we don’t need to stick with moving 1 bit at a time.** 

```c
uint64_t next_pow2(uint64_t x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}
```

---

### **測驗 `2`**

LeetCode [1680. Concatenation of Consecutive Binary Numbers](https://leetcode.com/problems/concatenation-of-consecutive-binary-numbers/)

```c
1: 1
2: 10
3: 11
--> 1 10 11
```

First, we need to find the length of the binary representation of the number. Then, we update the result.

1. `(i & (i - 1)) == 0` indicates that this is the power of 2, so we increase the `len`. E.g., 100 & 011 == 000 → 4 is the power of 2
2. update the `ans` to `i | (ans << len)` 
    - E.g., if `ans` = 110, `i` = 11, `len` = 2
    - `(ans << len)` —> 11000
    - `11 | 11000` —> 11011

```c
int concatenatedBinary(int n)
{
    const int M = 1e9 + 7;
    int len = 0; /* the bit length to be shifted */
    /* use long here as it potentially could overflow for int */
    long ans = 0;
    for (int i = 1; i <= n; i++) {
        /* removing the rightmost set bit
         * e.g. 100100 -> 100000
         *      000001 -> 000000
         *      000000 -> 000000
         * after removal, if it is 0, then it means it is power of 2
         * as all power of 2 only contains 1 set bit
         * if it is power of 2, we increase the bit length
         */
        if (!(i & (i - 1)))           // DDDD
            len++;
        ans = (i | (ans << len)) % M; // EEEE
    }
    return ans;
}
```

---

### **測驗 `3`**

```c
size_t swar_count_utf8(const char *buf, size_t len)
{
    const uint64_t *qword = (const uint64_t *) buf;
    const uint64_t *end = qword + len >> 3;

    size_t count = 0;
    for (; qword != end; qword++) {
        const uint64_t t0 = *qword;
        const uint64_t t1 = ~t0;
        const uint64_t t2 = t1 & 0x04040404040404040llu;
        const uint64_t t3 = t2 + t2;
        const uint64_t t4 = t0 & t3;
        count += __builtin_popcountll(t4);
    }

    count = (1 << 3) * (len / 8)  - count;
    count += (len & 7) ? count_utf8((const char *) end, len & 7) : 0;

    return count;
}
```

- `AAAA = 3` 64  / 8 = 8, log2(8) = 3. `1 << 3`
- `BBBB = 7` To check if there is any remaining bit in the 8-byte chunks
- `CCCC = 7` Determine the remainder of the division by 8
- `DDDD = 0` if there’s no remainder

```cpp
// Test Program
#include <stdio.h>

int main() {
    const char *buf = "Hello, 世界!";
    size_t len = sizeof("Hello, 世界!") - 1; // Exclude the null terminator

    size_t utf8_count = swar_count_utf8(buf, len);
    printf("Number of UTF-8 characters: %zu\n", utf8_count);
    // Number of UTF-8 characters: 9
    return 0;
}
```

---

### **測驗 `4`**

It checks if a given 16-bit unsigned integer has a specific bit pattern. If x has a pattern of continuous ones followed by continuous zeros, then return `true`

```cpp
bool is_pattern(uint16_t x)
{
    const uint16_t n = EEEE;
    return (n ^ x) < FFFF;
}
```

```cpp
bool is_pattern(uint16_t x)
{
    const uint16_t n = ~x + 1;
    return (n ^ x) < x;
}
```

**Example:** 

`x`: 0xC000 (1100 0000 0000 0000)

`~x`:  0x3FFF (0011 1111 1111 1111)

`n = ~x + 1`: 0x4FFF (0100 0000 0000 0000)

`n ^ x`: 0xC000 XOR 0x4FFF = 0x8000 (1000 0000 0000 0000)

`0x8000 < 0xC000` thus return `true`