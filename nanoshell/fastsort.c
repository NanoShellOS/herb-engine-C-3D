#include <stdint.h>
#include <stddef.h>

static inline void memswap(char *a, char *b, size_t size)
{
	while (size--)
	{
		char t = *a;
		*a++ = *b;
		*b++ = t;
	}
}

static void insertion_sort(
	char *base,
	size_t count,
	size_t size,
	int (*cmp)(const void *, const void *))
{
	for (size_t i = 1; i < count; i++)
	{
		size_t j = i;
		while (j > 0)
		{
			char *a = base + (j - 1) * size;
			char *b = base + j * size;

			if (cmp(a, b) <= 0)
				break;

			memswap(a, b, size);
			j--;
		}
	}
}

static void quicksort_internal(
	char *base,
	size_t count,
	size_t size,
	int (*cmp)(const void *, const void *))
{
	while (count > 16) // insertion sort threshold
	{
		size_t pivot = count / 2;

		char *pivot_ptr = base + pivot * size;
		memswap(pivot_ptr, base + (count - 1) * size, size);

		size_t store = 0;

		for (size_t i = 0; i < count - 1; i++)
		{
			char *elem = base + i * size;

			if (cmp(elem, base + (count - 1) * size) < 0)
			{
				memswap(elem, base + store * size, size);
				store++;
			}
		}

		memswap(base + store * size, base + (count - 1) * size, size);

		size_t left = store;
		size_t right = count - store - 1;

		if (left < right)
		{
			quicksort_internal(base, left, size, cmp);
			base += (store + 1) * size;
			count = right;
		}
		else
		{
			quicksort_internal(base + (store + 1) * size, right, size, cmp);
			count = left;
		}
	}

	insertion_sort(base, count, size, cmp);
}

void FAST_qsort(void *pBase, size_t nCount, size_t nElementSize, int (*pCompare)(const void *, const void *))
{
	if (nCount < 2)
		return;

	quicksort_internal((char *)pBase, nCount, nElementSize, pCompare);
}

/* ---------- qsort_r ---------- */

static void insertion_sort_r(
	char *base,
	size_t count,
	size_t size,
	int (*cmp)(const void *, const void *, void *),
	void *arg
)
{
	for (size_t i = 1; i < count; i++)
	{
		size_t j = i;

		while (j > 0)
		{
			char *a = base + (j - 1) * size;
			char *b = base + j * size;

			if (cmp(a, b, arg) <= 0)
				break;

			memswap(a, b, size);
			j--;
		}
	}
}

static void quicksort_internal_r(
	char *base,
	size_t count,
	size_t size,
	int (*cmp)(const void *, const void *, void *),
	void *arg)
{
	while (count > 16)
	{
		size_t pivot = count / 2;

		char *pivot_ptr = base + pivot * size;
		memswap(pivot_ptr, base + (count - 1) * size, size);

		size_t store = 0;

		for (size_t i = 0; i < count - 1; i++)
		{
			char *elem = base + i * size;

			if (cmp(elem, base + (count - 1) * size, arg) < 0)
			{
				memswap(elem, base + store * size, size);
				store++;
			}
		}

		memswap(base + store * size, base + (count - 1) * size, size);

		size_t left = store;
		size_t right = count - store - 1;

		if (left < right)
		{
			quicksort_internal_r(base, left, size, cmp, arg);
			base += (store + 1) * size;
			count = right;
		}
		else
		{
			quicksort_internal_r(base + (store + 1) * size, right, size, cmp, arg);
			count = left;
		}
	}

	insertion_sort_r(base, count, size, cmp, arg);
}

void FAST_qsortr(
	void *pBase,
	size_t nCount,
	size_t nElementSize,
	int (*pCompareReentrant)(const void *, const void *, void *),
	void *pArgument
)
{
	if (nCount < 2)
		return;

	quicksort_internal_r((char *)pBase, nCount, nElementSize, pCompareReentrant, pArgument);
}
