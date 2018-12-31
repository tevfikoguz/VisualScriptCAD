#include "Checksum.hpp"
#include <memory.h>
#include <cstddef>

static_assert (sizeof (int) == 4, "");
static_assert (sizeof (unsigned int) == 4, "");
static_assert (sizeof (size_t) == 8, "");
static_assert (sizeof (int64_t) == 8, "");
static_assert (sizeof (float) == 4, "");
static_assert (sizeof (double) == 8, "");

namespace Modeler
{

Checksum::Checksum () :
	checksum (0)
{

}

Checksum::~Checksum ()
{

}

void Checksum::Add (int val)
{
	// BSD checksum algorithm
	checksum = (checksum >> 1) + ((checksum & 1) << 31);
	checksum = (checksum + val) & 0xffffffff;
}

void Checksum::Add (unsigned int val)
{
	Add (int (val));
}

void Checksum::Add (int64_t val)
{
	Add ((int) (val & 0xffffffff));
	Add ((int) ((val >> 32) & 0xffffffff));
}

void Checksum::Add (float val)
{
	int intVal = 0;
	memcpy (&intVal, &val, sizeof (intVal));
	Add (intVal);
}

void Checksum::Add (double val)
{
	int64_t intVal = 0;
	memcpy (&intVal, &val, sizeof (intVal));
	Add (intVal);
}

void Checksum::Add (size_t val)
{
	Add (int64_t (val));
}

void Checksum::Add (const Checksum& val)
{
	Add (val.checksum);
}

bool Checksum::operator== (const Checksum& rhs) const
{
	return checksum == rhs.checksum;
}

bool Checksum::operator!= (const Checksum& rhs) const
{
	return !operator== (rhs);
}

size_t Checksum::GenerateHashValue () const
{
	return std::hash<int> {} (checksum);
}

}
