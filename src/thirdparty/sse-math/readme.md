# Simple SSE and SSE2 (and now NEON) optimized sin, cos, log and exp
This readme is taken verbatim from http://gruntthepeon.free.fr/ssemath (alonside the library).

## The story

I have spent quite a while looking for a simple (but fast) SSE version of some basic transcendental functions (sines and exponential). On the mac, you get the [vsinf](http://developer.apple.com/documentation/Performance/Conceptual/vecLib/Reference/reference.html#//apple_ref/doc/c_ref/vsinf) and friends (in the [Accelerate](http://developer.apple.com/releasenotes/Performance/RN-vecLib/index.html) framework) which are nice (there is a ppc version and an intel version, Apple rox) but closed-source, and restricted to macos..

Both Intel and AMD have some sort of vector math library with SIMD sines and cosines, but

* [Intel MKL](http://developer.intel.com/software/products/mkl/) is not free (neither as beer, nor as speech)
* [AMD ACML](http://developer.amd.com/acml) is free, but no source is available. Morever the vector functions are only available in 64bits OSes !
* Would you trust the intel MKL to run at full speed on AMD hardware ?

Some time ago, I found out the [Intel Approximate Math](http://www.intel.com/design/pentiumiii/devtools/AMaths.zip) library. This one is completely free and open-source, and it provides SSE and SSE2 versions of many functions. But it has two drawbacks:

* It is written as inline assembly, MASM style. The source is very targetted for MSVC/ICC so it is painful to use with gcc
* As the name implies, it is approximated. And, well, I had no use for a sine which has garbage in the ten last bits. 

However, it served as a great source of inspiration for the sin_ps, cos_ps, exp_ps and log_ps provided below.

I chose to write them in pure SSE1+MMX so that they run on the pentium III of your grand mother, and also on my brave athlon-xp, since thoses beast are not SSE2 aware. Intel AMath showed me that the performance gain for using SSE2 for that purpose was not large enough (10%) to consider providing an SSE2 version (but it can be done very quickly). __Update__: I finally did that SSE2 version -- see below.

The functions use only the `_mm_` intrinsics , there is no inline assembly in the code. Advantage: easier to debug, works out of the box on 64 bit setups, let the compiler choose what should be stored in a register, and what is stored in memory. Inconvenient: some versions of gcc 3.x are badly broken with certain intrinsic functions ( `_mm_movehl_ps` , `_mm_cmpeq_ps` etc). Mingw's gcc for example -- beware that the brokeness is dependent on the optimization level. A workaround is provided (inline asm replacement for the braindead intrinsics), it is not nice but robust, and broken compilers are detected by the validation program below.

I first tried to improve the AMath functions by using longer minimax polynomial approximations for sine, but of course it failed to achieve full precision because of rounding errors in the polynom, and in the computation of x modulo Pi. So I took a look at the implementation of these functions in the [cephes library](http://www.netlib.org/cephes/), noticed that they were simpler than what I imagined and contained very few branches, and just translated them in SSE intrinsics. The sincos_ps is nice because you get magically a free sine for each cosine you compute, so it is almost as fast as the sin_ps and the cos_ps.

Of course it is not IEEE compliant, but the max absolute error on sines is 2^-24 on the range [-8192,8192]. The SSE implementation matches exactly the cephes one if you do not use the x87 fpu when compiling the test program ( `-mfpmath=sse` with gcc)

## The code

The functions below are licensed under the [zlib](http://en.wikipedia.org/wiki/Zlib_License) license, so you can do basically what you want with them. There is nothing smart in them, it's just a straight translation of the cephes functions in SSE1+MMX. However it took some time to debug and validate (more than I expected at first). If you find a bug, or a way to improve their performances, or if you add other functions (tan, log2, exp2, pow, asin, ...) I'd be glad to know.

* [sse_mathfun.h](sse_mathfun.h) source code for sin_ps, cos_ps, sincos_ps, exp_ps, log_ps, as straight C.
* [sse_mathfun_test.c](sse_mathfun_test.c) Validation+Bench program for those function. Do not forget to run it once.

## SSE2 version (feature added on 2008/12/15)

Ok, I finally added a pure SSE2 version that does not use any mmx intrinsic. The reason is that the 64 bits MSVC 2008 compiler is not able to generate any MMX intrinsics. This compiler is just stupid. So I had no choice but translate mmx stuff into sse2, which proved to be very easy and boring. The perf improvement is below 10%. In order to use the SSE2 version, just define `USE_SSE2` when compiling.

## ARM NEON version (feature added on 2011/05/29)

This is a direct translation of the SSE2 code into NEON intrinsics. I have put it on a separate page, which is [here](http://gruntthepeon.free.fr/ssemath/neon_mathfun.html).

## Performance

If you are compiling with gcc 3.3 or older, and get deceiving performance, you should have a look at the generated assembly because it has some real problems to inline some SSE intrinsics. gcc 4.x should definitively be preferred.. 

### Results on a macbook with 1.83GHz Core 1 Duo (apple gcc 4.0.1)

command line: `gcc -O2 -Wall -W -DHAVE_VECLIB -msse sse_mathfun_test.c -framework Accelerate`

    exp([ -1000,    -100,   100,          1000]) = [            0,             0, 2.4061436e+38, 2.4061436e+38]
    exp([   nan,     inf,  -inf,           nan]) = [2.4061436e+38, 2.4061436e+38,             0, 2.4061436e+38]
    log([     0,     -10, 1e+30, 1.0005271e-42]) = [   -87.336548,    -87.336548,     69.077553,    -87.336548]
    log([   nan,     inf,  -inf,           nan]) = [   -87.336548,     88.722839,    -87.336548,    -87.336548]
    sin([   nan,     inf,  -inf,           nan]) = [          nan,           nan,           nan,           nan]
    cos([   nan,     inf,  -inf,           nan]) = [          nan,           nan,           nan,           nan]
    sin([-1e+30, -100000, 1e+30,        100000]) = [          inf,  -0.035749275,          -inf,   0.035749275]
    cos([-1e+30, -100000, 1e+30,        100000]) = [          nan,    -0.9993608,           nan,    -0.9993608]

            sinf .. ->    3.2M vector evals/s -> 142 cycles/value on a 1830MHz pc
            cosf .. ->    3.2M vector evals/s -> 142 cycles/value on a 1830MHz pc
    sincos (x87) .. ->    2.8M vector evals/s -> 161 cycles/value on a 1830MHz pc
            expf .. ->    3.0M vector evals/s -> 148 cycles/value on a 1830MHz pc
            logf .. ->    3.0M vector evals/s -> 150 cycles/value on a 1830MHz pc
     cephes_sinf .. ->    4.5M vector evals/s -> 100 cycles/value on a 1830MHz pc
     cephes_cosf .. ->    4.9M vector evals/s ->  92 cycles/value on a 1830MHz pc
     cephes_expf .. ->    3.0M vector evals/s -> 151 cycles/value on a 1830MHz pc
     cephes_logf .. ->    2.6M vector evals/s -> 172 cycles/value on a 1830MHz pc
          sin_ps .. ->   18.1M vector evals/s ->  25 cycles/value on a 1830MHz pc
          cos_ps .. ->   18.2M vector evals/s ->  25 cycles/value on a 1830MHz pc
       sincos_ps .. ->   15.0M vector evals/s ->  30 cycles/value on a 1830MHz pc
          exp_ps .. ->   17.6M vector evals/s ->  26 cycles/value on a 1830MHz pc
          log_ps .. ->   15.5M vector evals/s ->  29 cycles/value on a 1830MHz pc
           vsinf .. ->   14.3M vector evals/s ->  32 cycles/value on a 1830MHz pc
           vcosf .. ->   14.4M vector evals/s ->  32 cycles/value on a 1830MHz pc
           vexpf .. ->   12.0M vector evals/s ->  38 cycles/value on a 1830MHz pc
           vlogf .. ->   13.1M vector evals/s ->  35 cycles/value on a 1830MHz pc

### Results on a 2600MHz opteron running linux, with the 64 bits acml math vector lib (gcc 4.2)

command line: `gcc-4.2 -msse -O3 -Wall -W sse_mathfun_test.c -lm -DHAVE_ACML -I /usr/local/acml3.6.0/pathscale64/include /usr/local/acml3.6.0/pathscale64/lib/libacml_mv.a`

            sinf .. ->    6.3M vector evals/s -> 103 cycles/value on a 2600MHz pc
            cosf .. ->    5.6M vector evals/s -> 115 cycles/value on a 2600MHz pc
    sincos (x87) .. ->    4.2M vector evals/s -> 153 cycles/value on a 2600MHz pc
            expf .. ->    1.1M vector evals/s -> 546 cycles/value on a 2600MHz pc
            logf .. ->    4.7M vector evals/s -> 138 cycles/value on a 2600MHz pc
     cephes_sinf .. ->   11.6M vector evals/s ->  56 cycles/value on a 2600MHz pc
     cephes_cosf .. ->    8.7M vector evals/s ->  74 cycles/value on a 2600MHz pc
     cephes_expf .. ->    3.7M vector evals/s -> 172 cycles/value on a 2600MHz pc
     cephes_logf .. ->    5.5M vector evals/s -> 117 cycles/value on a 2600MHz pc
          sin_ps .. ->   26.1M vector evals/s ->  25 cycles/value on a 2600MHz pc
          cos_ps .. ->   26.1M vector evals/s ->  25 cycles/value on a 2600MHz pc
       sincos_ps .. ->   23.7M vector evals/s ->  27 cycles/value on a 2600MHz pc
          exp_ps .. ->   22.9M vector evals/s ->  28 cycles/value on a 2600MHz pc
          log_ps .. ->   21.6M vector evals/s ->  30 cycles/value on a 2600MHz pc
    ml vrs4_sinf .. ->   17.9M vector evals/s ->  36 cycles/value on a 2600MHz pc
    ml vrs4_cosf .. ->   18.3M vector evals/s ->  35 cycles/value on a 2600MHz pc
    ml vrs4_expf .. ->   28.6M vector evals/s ->  23 cycles/value on a 2600MHz pc
    ml vrs4_logf .. ->   23.6M vector evals/s ->  27 cycles/value on a 2600MHz pc

### Results on a 2GHz athlon-xp 2400+ , using mingw (gcc 3.4.5)

command line: `gcc -mfpmath=sse -msse -O2 -Wall -W sse_mathfun_test.c`

            sinf .. ->    3.4M vector evals/s -> 144 cycles/value on a 2000MHz pc
            cosf .. ->    5.1M vector evals/s ->  97 cycles/value on a 2000MHz pc
    sincos (x87) .. ->    2.3M vector evals/s -> 214 cycles/value on a 2000MHz pc
            expf .. ->    1.8M vector evals/s -> 272 cycles/value on a 2000MHz pc
            logf .. ->    2.5M vector evals/s -> 200 cycles/value on a 2000MHz pc
     cephes_sinf .. ->    3.7M vector evals/s -> 132 cycles/value on a 2000MHz pc
     cephes_cosf .. ->    3.2M vector evals/s -> 153 cycles/value on a 2000MHz pc
     cephes_expf .. ->    1.2M vector evals/s -> 407 cycles/value on a 2000MHz pc
     cephes_logf .. ->    1.4M vector evals/s -> 355 cycles/value on a 2000MHz pc
          sin_ps .. ->   17.2M vector evals/s ->  29 cycles/value on a 2000MHz pc
          cos_ps .. ->   17.0M vector evals/s ->  29 cycles/value on a 2000MHz pc
       sincos_ps .. ->   14.7M vector evals/s ->  34 cycles/value on a 2000MHz pc
          exp_ps .. ->   17.2M vector evals/s ->  29 cycles/value on a 2000MHz pc
          log_ps .. ->   14.7M vector evals/s ->  34 cycles/value on a 2000MHz pc

### Results on a 2GHz athlon-xp 2400+ , using cl.exe (visual c++ express 2005)

command line: `cl.exe /arch:SSE /O2 /TP /MD sse_mathfun_test.c`

            sinf .. ->    3.1M vector evals/s -> 160 cycles/value on a 2000MHz pc
            cosf .. ->    3.9M vector evals/s -> 127 cycles/value on a 2000MHz pc
    sincos (x87) .. ->    2.8M vector evals/s -> 175 cycles/value on a 2000MHz pc
            expf .. ->    2.0M vector evals/s -> 239 cycles/value on a 2000MHz pc
            logf .. ->    2.6M vector evals/s -> 192 cycles/value on a 2000MHz pc
     cephes_sinf .. ->    2.5M vector evals/s -> 198 cycles/value on a 2000MHz pc
     cephes_cosf .. ->    2.8M vector evals/s -> 176 cycles/value on a 2000MHz pc
     cephes_expf .. ->    0.9M vector evals/s -> 546 cycles/value on a 2000MHz pc
     cephes_logf .. ->    1.3M vector evals/s -> 370 cycles/value on a 2000MHz pc
          sin_ps .. ->   17.2M vector evals/s ->  29 cycles/value on a 2000MHz pc
          cos_ps .. ->   17.3M vector evals/s ->  29 cycles/value on a 2000MHz pc
       sincos_ps .. ->   15.5M vector evals/s ->  32 cycles/value on a 2000MHz pc
          exp_ps .. ->   17.8M vector evals/s ->  28 cycles/value on a 2000MHz pc
          log_ps .. ->   10.4M vector evals/s ->  48 cycles/value on a 2000MHz pc

### Results on a 1.6GHz atom N270 , using cl.exe (visual c++ express 2010)

command line: `cl.exe /arch:SSE /O2 /TP /MD sse_mathfun_test.c`

            sinf .. ->    1.3M vector evals/s -> 303 cycles/value on a 1600MHz pc
            cosf .. ->    1.3M vector evals/s -> 305 cycles/value on a 1600MHz pc
    sincos (x87) .. ->    1.2M vector evals/s -> 314 cycles/value on a 1600MHz pc
            expf .. ->    1.6M vector evals/s -> 244 cycles/value on a 1600MHz pc
            logf .. ->    1.4M vector evals/s -> 276 cycles/value on a 1600MHz pc
     cephes_sinf .. ->    1.4M vector evals/s -> 280 cycles/value on a 1600MHz pc
     cephes_cosf .. ->    1.5M vector evals/s -> 265 cycles/value on a 1600MHz pc
     cephes_expf .. ->    0.7M vector evals/s -> 548 cycles/value on a 1600MHz pc
     cephes_logf .. ->    0.8M vector evals/s -> 489 cycles/value on a 1600MHz pc
          sin_ps .. ->    9.2M vector evals/s ->  43 cycles/value on a 1600MHz pc
          cos_ps .. ->    9.5M vector evals/s ->  42 cycles/value on a 1600MHz pc
       sincos_ps .. ->    8.8M vector evals/s ->  45 cycles/value on a 1600MHz pc
          exp_ps .. ->    9.8M vector evals/s ->  41 cycles/value on a 1600MHz pc
          log_ps .. ->    8.6M vector evals/s ->  46 cycles/value on a 1600MHz pc

---

update (2008/03/14): If you are looking for extreme performance on SSE2 simple precision logf, have a look at the page of [Hans R. Battenfeld](http://gruntthepeon.free.fr/ssemath/arithmex.com), who is working on a version twice faster than the ones here.

--- 

Last modified: Sun May 29 23:59:10 CEST 2011 

---

UPD(yakvi, 2020-07-13): After I initially installed this library, these are the parameters the benchmark ran with on my machine 

### Results on an i7-3770K 3.90 GHz, using cl.exe (visual c++ community 2019), with `USE_SSE2` defined

command line: `cl.exe /DUSE_SSE2 /O2 /Tp sse_mathfun_test.c`

           sinf .. ->   28.6M vector evals/s ->  34 cycles/value on a 3900MHz pc
           cosf .. ->   30.9M vector evals/s ->  31 cycles/value on a 3900MHz pc
           expf .. ->   34.0M vector evals/s ->  29 cycles/value on a 3900MHz pc
           logf .. ->   35.1M vector evals/s ->  28 cycles/value on a 3900MHz pc
    cephes_sinf .. ->   26.4M vector evals/s ->  37 cycles/value on a 3900MHz pc
    cephes_cosf .. ->   22.2M vector evals/s ->  44 cycles/value on a 3900MHz pc
    cephes_expf .. ->    5.9M vector evals/s -> 163 cycles/value on a 3900MHz pc
    cephes_logf .. ->    5.6M vector evals/s -> 172 cycles/value on a 3900MHz pc
         sin_ps .. ->   45.5M vector evals/s ->  21 cycles/value on a 3900MHz pc
         cos_ps .. ->   45.3M vector evals/s ->  21 cycles/value on a 3900MHz pc
      sincos_ps .. ->   45.5M vector evals/s ->  21 cycles/value on a 3900MHz pc
         exp_ps .. ->   36.1M vector evals/s ->  27 cycles/value on a 3900MHz pc
         log_ps .. ->   33.4M vector evals/s ->  29 cycles/value on a 3900MHz pc
