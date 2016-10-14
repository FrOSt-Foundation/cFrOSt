#pragma once

typedef unsigned int u16;
typedef int i16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;

#define NULL ((void *)0)
#define UNUSED(x) UNUSED_##x __attribute__ ((__unused__))
