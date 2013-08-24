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

#include "TGAImage.hpp"

#define TGA_RGB 2
#define TGA_A 3
#define TGA_RLE 10

TGAImage::TGAImage( std::string fileName )
	: _fileName( fileName )
{
	_width = 0;
	_height = 0;
	_channels = 0;
}

TGAImage::~TGAImage( void )
{

}

void TGAImage::setData( unsigned int width, unsigned int height, unsigned int channels, const void *data )
{
	_width = width;
	_height = height;
	_channels = channels;
	_data.resize( _width * _height * _channels );
	if ( data != nullptr ) {
		memcpy( &_data[ 0 ], data, sizeof( unsigned char ) * _width * _height * _channels );
	}
}

bool TGAImage::load( void )
{
	int width = 0;
	int height = 0;
	int bpp = 0;
	unsigned char length = 0;
    unsigned char imageType = 0;
    unsigned char bits = 0;
    FILE *file = NULL;
    int stride = 0;
    std::vector< unsigned char > data;

    if ( ( file = fopen( _fileName.c_str(), "rb" ) ) == NULL ) {
    	std::cerr << "File not found " << _fileName << std::endl;
    	return false;
    }

    fread( &length, sizeof( unsigned char ), 1, file );
    fseek( file, 1, SEEK_CUR );
    fread( &imageType, sizeof( unsigned char ), 1, file );
    fseek( file, 9, SEEK_CUR );
    fread( &width, sizeof( unsigned short ), 1, file );
    fread( &height, sizeof( unsigned short ), 1, file );
    fread( &bits, sizeof( unsigned char ), 1, file );

    fseek( file, length + 1, SEEK_CUR );

    if( imageType != TGA_RLE ) {
        if( bits == 24 || bits == 32 ) {
            bpp = bits / 8;
            stride = bpp * width;
			data.resize( stride * height );

            for( unsigned int y = 0; y < height; y++ ) {
                unsigned char *line = &data[ stride * y ];

                fread( line, stride, 1, file );

                // Swap the B and R values since TGA
                // files are stored as BGR instead of RGB
                for( int i = 0; i < stride; i += bpp ) {
                    int temp = line[ i ];
                    line[ i ] = line[ i + 2 ];
                    line[ i + 2 ] = temp;
                }
            }
        }
        else if ( bits == 8 ) {
        	bpp = 1;
			data.resize( width * height );
        	fread( &data[ 0 ], width * height, 1, file );
        }
        else {
        	// invalid format
            std::cerr << "Invalid file format (" << bits << " bits)" << std::endl;
            return false;
        }
    }
    else
    {
        unsigned char rleID = 0;
        int colorsRead = 0;
        bpp = bits / 8;
        stride = bpp * width;

		data.resize( stride * height );
		std::vector< unsigned char > colors( bpp );

        int i = 0;
        while ( i < static_cast< int >( width * height ) ) {
            fread( &rleID, sizeof( unsigned char ), 1, file );

            if ( rleID < 128 ) {
                rleID++;

                while ( rleID ) {
                    fread( &colors[ 0 ], sizeof( unsigned char ) * bpp, 1, file );

                    data[ colorsRead + 0 ] = colors[ 2 ];
                    data[ colorsRead + 1 ] = colors[ 1 ];
                    data[ colorsRead + 2 ] = colors[ 0 ];

                    if ( bits == 32 ) {
                        data[ colorsRead + 3 ] = colors[ 3 ];
                    }

                    i++;
                    rleID--;
                    colorsRead += bpp;
                }
            }
            else {
                rleID -= 127;

                fread( &colors[ 0 ], sizeof( unsigned char ) * bpp, 1, file );

                while ( rleID ) {
                    data[ colorsRead + 0 ] = colors[ 2 ];
                    data[ colorsRead + 1 ] = colors[ 1 ];
                    data[ colorsRead + 2 ] = colors[ 0 ];

                    if ( bits == 32 ) {
                        data[ colorsRead + 3 ] = colors[ 3 ];
                    }

                    i++;
                    rleID--;
                    colorsRead += bpp;
                }
            }
        }
    }

    fclose( file );

    setData( width, height, bpp, &data[ 0 ] );

    return true;
}

bool TGAImage::save( void )
{
	TGAHeader header;
	header.identsize = 0;
    header.colorMapType = 0;
    header.imageType = _channels == 1 ? 3 : 2;
    header.colorMapStart = 0;
    header.colorMapLength = 0;
    header.colorMapBits = 0;
    header.xstart = 0;
    header.ystart = 0;
    header.width = _width;
    header.height = _height;
    header.bits = _channels * 8;
    header.descriptor = 0;

	FILE *out = fopen( _fileName.c_str(), "wb" );

	fwrite( &header.identsize, sizeof( unsigned char ), 1, out );
	fwrite( &header.colorMapType, sizeof( unsigned char ), 1, out );
	fwrite( &header.imageType, sizeof( unsigned char ), 1, out );
	fwrite( &header.colorMapStart, sizeof( unsigned short ), 1, out );
	fwrite( &header.colorMapLength, sizeof( unsigned short ), 1, out );
	fwrite( &header.colorMapBits, sizeof( unsigned char ), 1, out );
	fwrite( &header.xstart, sizeof( unsigned short ), 1, out );
	fwrite( &header.ystart, sizeof( unsigned short ), 1, out );
	fwrite( &header.width, sizeof( unsigned short ), 1, out );
	fwrite( &header.height, sizeof( unsigned short ), 1, out );
	fwrite( &header.bits, sizeof( unsigned char ), 1, out );
	fwrite( &header.descriptor, sizeof( unsigned char ), 1, out );

	fwrite( &_data[ 0 ], _width * _height * _channels, sizeof( unsigned char ), out );

	fclose( out );

	return true;
}

