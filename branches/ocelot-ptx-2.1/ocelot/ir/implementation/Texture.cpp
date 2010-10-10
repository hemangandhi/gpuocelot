/*!
	\file Texture.cpp
	
	\author Gregory Diamos <gregory.diamos@gatech.edu>
	\date Sunday April 5, 2009
	
	\brief The source file for the Texture class
*/

#ifndef TEXTURE_CPP_INCLUDED
#define TEXTURE_CPP_INCLUDED

#include <ocelot/ir/interface/Texture.h>

namespace ir
{
	Texture::Texture(const std::string& n) : name(n), normalize(false), 
		type(Invalid), size( Dim3(0, 0, 0) ), data( 0 ) {
		
	}
	
	std::string Texture::toString() const {
		return ".global .texref " + name + ";";
	}

}

#endif
