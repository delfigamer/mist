#include "shape.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	bool Shape::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	Shape::Shape() {
	}
	
	Shape::~Shape() {
	}
	
	void Shape::paint() {
	}
	
	void Shape::advance() {
	}
	
	extern "C" {
	}
}
