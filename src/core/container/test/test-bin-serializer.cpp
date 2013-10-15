#include <assert.h>

#include <iostream>

#include <core/container/test/test-bin-serializerBinary.hpp>

using namespace std;
using namespace j_serialization;

enum
{
    HOST_LITTLE_ENDIAN = 0x03020100ul,
    HOST_PDP_ENDIAN = 0x01000302ul,
    HOST_BIG_ENDIAN = 0x00010203ul    
};

enum class Endian: uint8_t{
	LITTLE,
	PDP,
	BIG
};

static const union { unsigned char bytes[4]; uint32_t value; } host_order = { { 0, 1, 2, 3 } };

Endian getEndian(){
	return host_order.value == HOST_LITTLE_ENDIAN ? Endian::LITTLE : (host_order.value == HOST_BIG_ENDIAN ? Endian::BIG : Endian::PDP );
}

int main(){
	TestContainer s;
	s.a = 2;
	s.b = 4;
	s.myVar = "Test";
	s.f = 16.2;
	s.v = {{1}, {2}, {4}, {8}};

	s.pa = & s.a;

	char buf[1000];
	memset(buf, 0, 1000);
	uint64_t pos = 0;

	uint8_t endian = (uint8_t) getEndian();
	serialize(endian, buf, pos);
	serialize(s, buf, pos);

	cout << "Length: " << pos << " " << serializeLen(s) << endl;

	assert(pos == serializeLen(s) + 1);

	TestContainer out;
	uint64_t dpos = 0;

	deserialize(endian, buf, dpos, 1000);
	deserialize(out, buf, dpos, 1000);

	assert(s.a == out.a);
	assert(s.b == out.b);
	assert(s.myVar == out.myVar);
	assert(s.f == out.f);
	cout << *s.pa << " " << *out.pa << endl;
	assert(*s.pa == *out.pa);

	free(out.pa);

	cout << out.v.size() << endl;
	assert(s.v.size() == out.v.size());

	return 0;
}
