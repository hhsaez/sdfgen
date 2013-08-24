/**
 * Copyright (c) 2013, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SDF_GEN_TGA_IMAGE_
#define SDF_GEN_TGA_IMAGE_

#include <iostream>
#include <string>
#include <vector>

class TGAImage {
public:
	TGAImage( std::string fileName );
	virtual ~TGAImage( void );

	bool load( void );
	bool save( void );

	std::string getFileName( void ) const { return _fileName; }
	unsigned int getWidth( void ) const { return _width; }
	unsigned int getHeight( void ) const { return _height; }
	unsigned int getChannels( void ) const { return _channels; }
	const unsigned char *getData( void ) const { return &_data[ 0 ]; }

	void setData( unsigned int width, unsigned int height, unsigned int channels, const void *data );

private:
	struct TGAHeader {
		unsigned char identsize;
	    unsigned char colorMapType;
	    unsigned char imageType;	// 2 - rgb, 3 - greyscale
	    unsigned short colorMapStart;
	    unsigned short colorMapLength;
	    unsigned char colorMapBits;
	    unsigned short xstart;
	    unsigned short ystart;
	    unsigned short width;
	    unsigned short height;
	    unsigned char bits;
	    unsigned char descriptor;
	};

	std::string _fileName;
	unsigned int _width;
	unsigned int _height;
	unsigned int _channels;
	std::vector< unsigned char > _data;
};

#endif

