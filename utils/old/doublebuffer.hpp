#ifndef UTILS_DOUBLEBUFFER_HPP__
#define UTILS_DOUBLEBUFFER_HPP__ 1

namespace utils
{
	template< typename T >
	class DoubleBuffer
	{
	private:
		T m_buf[ 2 ];
		int m_pointer;

	public:
		DoubleBuffer()
			: m_pointer( 0 )
		{
		}

		DoubleBuffer( T const& value )
			: m_pointer( 0 )
			, m_buf{ value }
		{
		}

		~DoubleBuffer()
		{
		}

		T& front()
		{
			return m_buf[ m_pointer ];
		}

		T& back()
		{
			return m_buf[ m_pointer ^ 1 ];
		}

		void swap()
		{
			m_pointer ^= 1;
		}
	};
}

#endif
