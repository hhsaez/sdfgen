#include <iostream>
#include <cmath>

#include "TGAImage.hpp"

class Grid {
public:
	struct Point {
		int dx;
		int dy;

		int distSqr( void ) const { return dx * dx + dy * dy; }
	};

	Grid( unsigned int width, unsigned int height )
		: _width( width ),
		  _height( height ),
		  _data( _width * _height )
	{

	}

	virtual ~Grid( void )
	{

	}

	void put( int x, int y, const Point &point )
	{
		_data[ y * _width + x ] = point;
	}

	Point &get( int x, int y )
	{
		return _data[ y * _width + x ];
	}

	void generateSDF( void )
	{
		// pass 1
		for ( int y = 0; y < _height; y++ ) {
			for ( int x = 0; x < _width; x++ ) {
				Point p = get( x, y );
				compare( p, x, y, -1, 0 );
				compare( p, x, y, 0, -1 );
				compare( p, x, y, -1, -1 );
				compare( p, x, y, 1, -1 );
				put( x, y, p );
			}

			for ( int x = _width - 1; x >= 0; x-- ) {
				Point p = get( x, y );
				compare( p, x, y, 1, 0 );
				put( x, y, p );
			}
		}

		// pass 2
		for ( int y = _height - 1; y >= 0; y-- ) {
			for ( int x = _width - 1; x >= 0; x-- ) {
				Point p = get( x, y );
				compare( p, x, y, 1, 0 );
				compare( p, x, y, 0, 1 );
				compare( p, x, y, -1, 0 );
				compare( p, x, y, 1, 1 );
				put( x, y, p );
			}

			for ( int x = 0; x < _width; x++ ) {
				Point p = get( x, y );
				compare( p, x, y, -1, 0 );
				put( x, y, p );
			}
		}
	}

private:
	void compare( Point &p, int x, int y, int offsetX, int offsetY )
	{
		if ( x < 0 || x + offsetX >= _width ) {
			return;
		}

		if ( y < 0 || y + offsetY >= _height ) {
			return;
		}
		
		Point other = get( x + offsetX, y + offsetY );
		other.dx += offsetX;
		other.dy += offsetY;

		if ( other.distSqr() < p.distSqr() ) {
			p = other;
		}
	}

	unsigned int _width;
	unsigned int _height;
	std::vector< Point > _data;
};

int main( int argc, char **argv )
{
	TGAImage input( "LucidaGrande.tga" );
	if ( !input.load() ) {
		return -1;
	}

	std::cout << input.getWidth() << "x" << input.getHeight() << "x" << input.getChannels() << std::endl;

	Grid grid1( input.getWidth(), input.getHeight() );
	Grid grid2( input.getWidth(), input.getHeight() );

	Grid::Point inside;
	inside.dx = 0;
	inside.dy = 0;

	Grid::Point outside;
	outside.dx = 999;
	outside.dy = 999;

	std::vector< unsigned char > sdf( input.getWidth() * input.getHeight() );
	for ( int y = 0; y < input.getHeight(); y++ ) {
		for ( int x = 0; x < input.getWidth(); x++ ) {
			unsigned char color = input.getData()[ y * ( input.getWidth() * input.getChannels() ) + x * input.getChannels() ];
			grid1.put( x, y, color < 128 ? inside : outside );
			grid2.put( x, y, color < 128 ? outside : inside );
		}
	}

	grid1.generateSDF();
	grid2.generateSDF();

	std::vector< unsigned char > data( input.getWidth() * input.getHeight() );
	for ( int y = 0; y < input.getHeight(); y++ ) {
		for ( int x = 0; x < input.getWidth(); x++ ) {
			// Calculate the actual distance from the dx/dy
			int dist1 = sqrt( grid1.get( x, y ).distSqr() );
			int dist2 = sqrt( grid2.get( x, y ).distSqr() );
			int dist = dist1 - dist2;

			// Clamp and scale it, just for display purposes.
			int c = dist * 3 + 128;
			if ( c < 0 ) c = 0;
			if ( c > 255 ) c = 255;

			data[ y * input.getWidth() + x ] = c;
		}
	}

	TGAImage output( "LucidaGrande_sdf.tga" );
	output.setData( input.getWidth(), input.getHeight(), 1, &data[ 0 ] );
	output.save();

#ifdef __APPLE__
	system( "open LucidaGrande.tga" );
	system( "open LucidaGrande_sdf.tga" );
#endif

	return 0;
}

