#pragma once

#define static_assert(expr, msg) _Static_assert(expr, msg)

#define sizeof_member(type, member) sizeof(((type *)0)->member)

#define static_assert_sizeof(type, size) \
    static_assert(sizeof(type) == size, "sizeof(" #type ") is not equal to " #size)

#define static_assert_sizeof_member(type, member, size) \
    static_assert(sizeof_member(type, member) == size, "sizeof(" #type "->" #member ") is not equal to " #size)

#define __packed __attribute__((packed))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))