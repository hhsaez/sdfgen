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

