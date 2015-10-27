#include "shape.hpp"
#include <utils/cbase.hpp>

namespace graphics {
	void Shape::doadvance( IDirect3DDevice9* device, int framecount ) {
	}
	
	bool Shape::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}
	
	Shape::Shape() {
	}
	
	Shape::~Shape() {
	}
	
	void Shape::paint( IDirect3DDevice9* device ) {
	}
	
	extern "C" {
	}
}
