#include <nsstandard.h>
#include "game.h"

// useful double functions
double __sin(double x)
{
    double result;
    __asm__ volatile("fsin" : "=t"(result) : "0"(x));
    return result;
}
//double __cos(double x)
//{
//    double result;
//    __asm__ volatile("fcos" : "=t"(result) : "0"(x));
//    return result;
//}

float SinCosTable[65536];

void InitSinCosTable()
{
	for (int i = 0; i < 65536; i++) {
		SinCosTable[i] = __sin(i * 2 * PI / 65536.0);
	}
}

float sin(float x)
{
	x /= 2 * PI;
	x *= 65535;
	return SinCosTable[(int)x & 0xFFFF];
}

float cos(float x)
{
	x /= 2 * PI;
	x *= 65535;
	return SinCosTable[(int)(x + 16384) & 0xFFFF];
}

double sqrt(double x)
{
    double result;
    __asm__ volatile("fsqrt" : "=t"(result) : "0"(x));
    return result;
}

//trims off the bit over 1
//so if you have a number like -1.007, it would return -0.007, and if you
//have a number like 1455.4559, it would return 0.4559
double Trim(double b)
{
	int intpart = (int)(b);
	return b - intpart;
}

double Abs (double b)
{
	if (b < 0) return -b;
	return b;
}

//shamelessly stolen from https://github.com/managarm/mlibc/blob/master/options/ansi/musl-generic-math/fmod.c#L4
// NOTE: fpclassify always returns exactly one of those constants
// However making them bitwise disjoint simplifies isfinite() etc.
int Classify(double x)
{
	union {double f; uint64_t i;} u = {x};
	int e = u.i >> 52 & 0x7ff;
	if (!e)
		return u.i << 1 ? FP_SUBNORMAL : FP_ZERO;
	if (e == 0x7ff)
		return u.i << 12 ? FP_NAN : FP_INFINITE;
	return FP_NORMAL;
}
double fmod(double x, double y)
{
	union {double f; uint64_t i;} ux = {x}, uy = {y};
	int ex = ux.i>>52 & 0x7ff;
	int ey = uy.i>>52 & 0x7ff;
	int sx = ux.i>>63;
	uint64_t i;

	/* in the followings uxi should be ux.i, but then gcc wrongly adds */
	/* float load/store to inner loops ruining performance and code size */
	uint64_t uxi = ux.i;

	if (uy.i<<1 == 0 || isnan(y) || ex == 0x7ff)
		return (x*y)/(x*y);
	if (uxi<<1 <= uy.i<<1) {
		if (uxi<<1 == uy.i<<1)
			return 0*x;
		return x;
	}

	/* normalize x and y */
	if (!ex) {
		for (i = uxi<<12; i>>63 == 0; ex--, i <<= 1);
		uxi <<= -ex + 1;
	} else {
		uxi &= -1ULL >> 12;
		uxi |= 1ULL << 52;
	}
	if (!ey) {
		for (i = uy.i<<12; i>>63 == 0; ey--, i <<= 1);
		uy.i <<= -ey + 1;
	} else {
		uy.i &= -1ULL >> 12;
		uy.i |= 1ULL << 52;
	}

	/* x mod y */
	for (; ex > ey; ex--) {
		i = uxi - uy.i;
		if (i >> 63 == 0) {
			if (i == 0)
				return 0*x;
			uxi = i;
		}
		uxi <<= 1;
	}
	i = uxi - uy.i;
	if (i >> 63 == 0) {
		if (i == 0)
			return 0*x;
		uxi = i;
	}
	for (; uxi>>52 == 0; uxi <<= 1, ex--);

	/* scale result */
	if (ex > 0) {
		uxi -= 1ULL << 52;
		uxi |= (uint64_t)ex << 52;
	} else {
		uxi >>= -ex + 1;
	}
	uxi |= (uint64_t)sx << 63;
	ux.i = uxi;
	return ux.f;
}

int floor(float x) {
	int i = (int)x;
	if (x < 0 && x != i) {
		i -= 1;
	}
	return i;
}

int ceil(float x) {
	int i = (int)x;
	if (x > 0 && x != i) {
		i += 1;
	}
	return i;
}

float fmin(float a, float b) {
	return a < b ? a : b;
}

float fmax(float a, float b) {
	return a > b ? a : b;
}