#ifndef RSBIN_INT_HPP__
#define RSBIN_INT_HPP__ 1

namespace rsbin {
	class beu8 {
	private:
		uint8_t m_data;
		
	public:
		beu8( uint8_t data = 0 ) :
			m_data( data ) {
		}
		
		beu8& operator=( uint8_t data ) {
			m_data = data;
			return *this;
		}
		
		operator uint8_t() const {
			return m_data;
		}
	};
	
	class beu16 {
	private:
		uint8_t m_data[ 2 ];
		
	public:
		beu16( uint16_t data = 0 ) :
			m_data{
				( uint8_t )( data >> 8 ),
				( uint8_t )( data )
			} {
		}
		
		beu16& operator=( uint16_t data ) {
			m_data[ 0 ] = data >> 8;
			m_data[ 1 ] = data;
			return *this;
		}
		
		operator uint16_t() const {
			return
				( uint16_t )m_data[ 0 ] << 8 |
				( uint16_t )m_data[ 1 ];
		}
	};
	
	class beu32 {
	private:
		uint8_t m_data[ 4 ];
		
	public:
		beu32( uint32_t data = 0 ) :
			m_data{
				( uint8_t )( data >> 24 ),
				( uint8_t )( data >> 16 ),
				( uint8_t )( data >> 8 ),
				( uint8_t )( data )
			} {
		}
		
		beu32& operator=( uint32_t data ) {
			m_data[ 0 ] = data >> 24;
			m_data[ 1 ] = data >> 16;
			m_data[ 2 ] = data >> 8;
			m_data[ 3 ] = data;
			return *this;
		}
		
		operator uint32_t() const {
			return
				( uint32_t )m_data[ 0 ] << 24 |
				( uint32_t )m_data[ 1 ] << 16 |
				( uint32_t )m_data[ 2 ] << 8 |
				( uint32_t )m_data[ 3 ];
		}
	};
	
	class beu64 {
	private:
		uint8_t m_data[ 8 ];
		
	public:
		beu64( uint64_t data = 0 ) :
			m_data{
				( uint8_t )( data >> 56 ),
				( uint8_t )( data >> 48 ),
				( uint8_t )( data >> 40 ),
				( uint8_t )( data >> 32 ),
				( uint8_t )( data >> 24 ),
				( uint8_t )( data >> 16 ),
				( uint8_t )( data >> 8 ),
				( uint8_t )( data )
			} {
		}
		
		beu64& operator=( uint64_t data ) {
			m_data[ 0 ] = data >> 56;
			m_data[ 1 ] = data >> 48;
			m_data[ 2 ] = data >> 40;
			m_data[ 3 ] = data >> 32;
			m_data[ 4 ] = data >> 24;
			m_data[ 5 ] = data >> 16;
			m_data[ 6 ] = data >> 8;
			m_data[ 7 ] = data;
			return *this;
		}
		
		operator uint64_t() const {
			return
				( uint64_t )m_data[ 0 ] << 56 |
				( uint64_t )m_data[ 1 ] << 48 |
				( uint64_t )m_data[ 2 ] << 40 |
				( uint64_t )m_data[ 3 ] << 32 |
				( uint64_t )m_data[ 4 ] << 24 |
				( uint64_t )m_data[ 5 ] << 16 |
				( uint64_t )m_data[ 6 ] << 8 |
				( uint64_t )m_data[ 7 ];
		}
	};
}

#endif
