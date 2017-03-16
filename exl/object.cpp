#include <exl/object.hpp>

namespace exl
{
	Object::Object()
	{
	}

	Object::~Object()
	{
	}

	ptrdiff_t Object::AddRef()
	{
		return RefObject::AddRef();
	}

	ptrdiff_t Object::Release()
	{
		return RefObject::Release();
	}
}
