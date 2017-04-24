#include <exl/parser/exllexer.hpp>
#include <utils/encoding.hpp>
#include <map>
#include <vector>
#include <utility>
#include <cstring>
#include <cmath>

namespace exl
{
	typedef std::unique_ptr< ExlToken > tokenptr_t;

	namespace lexer
	{
		bool keyword( std::vector< uint8_t > word, ExlTokenType* kw )
		{
			static std::map< std::vector< uint8_t >, ExlTokenType > keywords = {
				{ { 'a', 's' }, ExlTokenType::kwas },
				{ { 'c', 'l', 'a', 's', 's' }, ExlTokenType::kwclass },
				{ { 'c', 'o', 'n', 's', 't' }, ExlTokenType::kwconst },
				{ { 'e', 'l', 's', 'e' }, ExlTokenType::kwelse },
				{ { 'e', 'l', 's', 'e', 'i', 'f' }, ExlTokenType::kwelseif },
				{ { 'e', 'n', 'd' }, ExlTokenType::kwend },
				{ { 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n' },
					ExlTokenType::kwfunction },
				{ { 'i', 'f' }, ExlTokenType::kwif },
				{ { 'i', 'n' }, ExlTokenType::kwin },
				{ { 'i', 'n', 'o', 'u', 't' }, ExlTokenType::kwinout },
				{ { 'l', 'o', 'c', 'a', 'l' }, ExlTokenType::kwlocal },
				{ { 'n', 'i', 'l' }, ExlTokenType::kwnil },
				{ { 'o', 'u', 't' }, ExlTokenType::kwout },
				{ { 't', 'h', 'e', 'n' }, ExlTokenType::kwthen },
				{ { 't', 'y', 'p', 'e' }, ExlTokenType::kwtype },
				{ { 'u', 'n', 'i', 't' }, ExlTokenType::kwunit },
			};
			auto it = keywords.find( word );
			if( it != keywords.end() )
			{
				if( kw )
				{
					*kw = it->second;
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		int decdigit( int ch )
		{
			return ( ch >= '0' && ch <= '9' ) ? ch - '0' : -1;
		}

		int hexdigit( int ch )
		{
			return ( ch >= '0' && ch <= '9' ) ? ch - '0' :
				( ch >= 'A' && ch <= 'F' ) ? ch - ( 'A' - 10 ) :
				( ch >= 'a' && ch <= 'f' ) ? ch - ( 'a' - 10 ) : -1;
		}

		bool letterordigit( int ch )
		{
			return
				( ch >= '0' && ch <= '9' ) ||
				( ch >= 'A' && ch <= 'Z' ) ||
				( ch >= 'a' && ch <= 'z' ) ||
				ch == '_';
		}

		template< int length >
		uint32_t obtaindec( CharStream& stream )
		{
			uint32_t ret = 0;
			for( int i = 0; i < length; ++i )
			{
				int ch = stream.getc();
				int dd = decdigit( ch );
				if( dd != -1 )
				{
					ret = ret * 10 + dd;
				}
				else
				{
					stream.ungetc();
					break;
				}
			}
			return ret;
		}

		template< int length >
		uint32_t obtainhexfixed( CharStream& stream )
		{
			uint32_t ret = 0;
			for( int i = 0; i < length; ++i )
			{
				int ch = stream.getc();
				int hd = hexdigit( ch );
				if( hd != -1 )
				{
					ret = ret * 16 + hd;
				}
				else
				{
					stream.ungetc();
					stream.error( "invalid hex digit", stream.textpos );
				}
			}
			return ret;
		}

		template< int length >
		uint32_t obtainhex( CharStream& stream )
		{
			uint32_t ret = 0;
			for( int i = 0; i < length; ++i )
			{
				int ch = stream.getc();
				int hd = hexdigit( ch );
				if( hd != -1 )
				{
					ret = ret * 16 + hd;
				}
				else
				{
					stream.ungetc();
					if( i == 0 )
					{
						stream.error( "invalid hex digit", stream.textpos );
					}
					else
					{
						break;
					}
				}
			}
			return ret;
		}

		void obtainstringelement(
			CharStream& stream, int first, std::vector< uint8_t >& content )
		{
			TextPos spos = stream.textpos;
			if( first == '\\' )
			{
				int ch = stream.getc();
				switch( ch )
				{
				case 'a':
					content.push_back( '\x07' );
				return;

				case 'b':
					content.push_back( '\x08' );
				return;

				case 'f':
					content.push_back( '\x0c' );
				return;

				case 'n':
					content.push_back( '\x0a' );
				return;

				case 'r':
					content.push_back( '\x0d' );
				return;

				case 't':
					content.push_back( '\x09' );
				return;

				case 'v':
					content.push_back( '\x0b' );
				return;

				case '\\':
					content.push_back( '\\' );
				return;

				case '\"':
					content.push_back( '\"' );
				return;

				case '\'':
					content.push_back( '\'' );
				return;

				case '\n':
					content.push_back( '\n' );
				return;

				case 'x':
				{
					int val = obtainhexfixed< 2 >( stream );
					content.push_back( val );
				}
				return;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					stream.ungetc();
					int val = obtaindec< 3 >( stream );
					if( val > 255 )
					{
						stream.error( "invalid byte value", spos, stream.textpos );
					}
					else
					{
						content.push_back( val );
					}
				}
				return;

				case 'u':
				{
					if( stream.getc() != '{' )
					{
						stream.ungetc();
						stream.error( "{ expected", stream.textpos );
					}
					uint32_t val = obtainhex< 8 >( stream );
					if( stream.getc() != '}' )
					{
						stream.ungetc();
						stream.error( "} expected", stream.textpos );
					}
					uint8_t buf[ 4 ];
					size_t len;
					if( utils::encoding::utf8.encode(
						buf, val, sizeof( buf ), &len ) )
					{
						for( size_t i = 0; i < len; ++i )
						{
							content.push_back( buf[ i ] );
						}
					}
					else
					{
						stream.error( "invalid code point", spos, stream.textpos );
					}
				}
				return;

				case 'z':
					do
					{
						ch = stream.getc();
					}
						while( ch <= 32 );
					stream.ungetc();
				return;

				default:
					stream.error( "unknown escape character", spos, stream.textpos );
				return;
				}
			}
			else
			{
				content.push_back( first );
			}
		}

		tokenptr_t obtainstring( CharStream& stream )
		{
			TextPos spos = stream.textpos;
			int open = stream.getc();
			std::vector< uint8_t > content;
			while( true )
			{
				int ch = stream.getc();
				if( ch == open )
				{
					break;
				}
				else if( ch == '\n' || ch == -1 )
				{
					stream.error( "unterminated string", spos, stream.textpos );
				}
				obtainstringelement( stream, ch, content );
			}
			return tokenptr_t( new ExlToken(
				ExlTokenType::string, spos, stream.textpos,
				DataBuffer::create( content.size(), content.data() ) ) );
		}

		template< bool capture >
		tokenptr_t obtainlongstring( CharStream& stream, int level, TextPos spos )
		{
			std::vector< uint8_t > content;
			int ch = stream.getc();
			if( ch != '\n' )
			{
				stream.ungetc();
			}
			while( true )
			{
				int ch = stream.getc();
				if( ch == ']' )
				{
					for( int i = 0; i < level; ++i )
					{
						ch = stream.getc();
						if( ch != '=' )
						{
							stream.ungetc();
							if( capture )
							{
								content.push_back( ']' );
								for( int j = 1; j < i; ++j )
								{
									content.push_back( '=' );
								}
							}
							ch = -1;
							break;
						}
					}
					if( ch != -1 )
					{
						ch = stream.getc();
						if( ch == ']' )
						{
							break;
						}
						else
						{
							stream.ungetc();
							if( capture )
							{
								content.push_back( ']' );
								for( int j = 0; j < level; ++j )
								{
									content.push_back( '=' );
								}
							}
						}
					}
				}
				else if( ch == -1 )
				{
					stream.error( "unterminated long string", spos, stream.textpos );
				}
				else if( capture )
				{
					content.push_back( ch );
				}
			}
			if( capture )
			{
				return tokenptr_t( new ExlToken(
					ExlTokenType::string, spos, stream.textpos,
					DataBuffer::create( content.size(), content.data() ) ) );
			}
			else
			{
				return tokenptr_t();
			}
		}

		tokenptr_t obtainword( CharStream& stream )
		{
			TextPos spos = stream.textpos;
			std::vector< uint8_t > word;
			while( true )
			{
				int ch = stream.getc();
				if( letterordigit( ch ) )
				{
					word.push_back( ch );
				}
				else
				{
					stream.ungetc();
					break;
				}
			}
			ExlTokenType kw;
			if( keyword( word, &kw ) )
			{
				return tokenptr_t( new ExlToken(
					kw, spos, stream.textpos ) );
			}
			else
			{
				return tokenptr_t( new ExlToken(
					ExlTokenType::identifier, spos, stream.textpos,
					DataBuffer::create( word.size(), word.data() ) ) );
			}
		}

		template< int base, int( *digitmap )( int ),
			char explower, char expupper, int expbase >
		tokenptr_t obtainnumber_base( CharStream& stream, TextPos spos )
		{
			double val = 0;
			int ch = stream.getc();
			bool intpart;
			bool fracpart;
			if( digitmap( ch ) != -1 )
			{
				intpart = true;
			}
			while( true )
			{
				int dmch = digitmap( ch );
				if( dmch != -1 )
				{
					val = val * base + dmch;
				}
				else if( ch == '.' )
				{
					goto fraction;
				}
				else if( ch == explower || ch == expupper )
				{
					goto exponent;
				}
				else if( letterordigit( ch ) )
				{
					goto fail;
				}
				else
				{
					stream.ungetc();
					goto success;
				}
				ch = stream.getc();
			}
		fraction:
			{
				ch = stream.getc();
				if( digitmap( ch ) )
				{
					fracpart = true;
				}
				if( !intpart && !fracpart )
				{
					goto fail;
				}
				double factor = 1.0 / base;
				while( true )
				{
					int dmch = digitmap( ch );
					if( dmch != -1 )
					{
						val = val + dmch * factor;
						factor /= base;
					}
					else if( ch == explower || ch == expupper )
					{
						goto exponent;
					}
					else if( ch == '.' || letterordigit( ch ) )
					{
						goto fail;
					}
					else
					{
						stream.ungetc();
						goto success;
					}
					ch = stream.getc();
				}
			}
		exponent:
			{
				int sign = 1;
				int exp = 0;
				ch = stream.getc();
				if( ch == '-' )
				{
					sign = -1;
					ch = stream.getc();
				}
				else if( ch == '+' )
				{
					ch = stream.getc();
				}
				if( decdigit( ch ) == -1 )
				{
					stream.ungetc();
					goto fail;
				}
				stream.ungetc();
				while( true )
				{
					ch = stream.getc();
					int dmch = decdigit( ch );
					if( dmch != -1 )
					{
						exp = exp * 10 + dmch;
					}
					else if( ch == '.' || letterordigit( ch ) )
					{
						goto fail;
					}
					else
					{
						stream.ungetc();
						val = val * pow( expbase, sign * exp );
						goto success;
					}
				}
			}
		fail:
			stream.error( "invalid number", spos, stream.textpos );
		success:
			return tokenptr_t( new ExlToken(
				ExlTokenType::number, spos, stream.textpos, val ) );
		}

		tokenptr_t obtaindecnumber( CharStream& stream, TextPos spos )
		{
			return obtainnumber_base< 10, decdigit, 'e', 'E', 10 >( stream, spos );
		}

		tokenptr_t obtaindecnumber( CharStream& stream )
		{
			TextPos spos = stream.textpos;
			return obtainnumber_base< 10, decdigit, 'e', 'E', 10 >( stream, spos );
		}

		tokenptr_t obtainhexnumber( CharStream& stream, TextPos spos )
		{
			return obtainnumber_base< 16, hexdigit, 'p', 'P', 2 >( stream, spos );
		}

		tokenptr_t obtaincomment( CharStream& stream, TextPos spos )
		{
			int ch = stream.getc();
			if( ch == '[' )
			{
				int level = 0;
				while( true )
				{
					ch = stream.getc();
					if( ch == '[' )
					{
						obtainlongstring< false >( stream, level, spos );
						return tokenptr_t();
					}
					else if( ch == '=' )
					{
						level += 1;
					}
					else
					{
						stream.ungetc();
						break;
					}
				}
			}
			else
			{
				stream.ungetc();
			}
			do
			{
				ch = stream.getc();
			}
				while( ch != '\n' && ch != -1 );
			stream.ungetc();
			return tokenptr_t();
		}

		template< ExlTokenType type >
		tokenptr_t obtainsymbol( CharStream& stream )
		{
			TextPos spos = stream.textpos;
			stream.getc();
			return tokenptr_t( new ExlToken(
				type, spos, stream.textpos ) );
		}

		template< ExlTokenType basetype, char ch1, ExlTokenType type1 >
		tokenptr_t obtaindigraph( CharStream& stream )
		{
			TextPos spos = stream.textpos;
			stream.getc();
			int ch = stream.getc();
			if( ch == ch1 )
			{
				return tokenptr_t( new ExlToken(
					type1, spos, stream.textpos ) );
			}
			else
			{
				stream.ungetc();
				return tokenptr_t( new ExlToken(
					basetype, spos, stream.textpos ) );
			}
		}

		tokenptr_t( *tokentable[ 256 ] )( CharStream& ) = {
			nullptr, //   0
			nullptr, //   1
			nullptr, //   2
			nullptr, //   3
			nullptr, //   4
			nullptr, //   5
			nullptr, //   6
			nullptr, //   7
			nullptr, //   8
			nullptr, //   9
			obtainsymbol< ExlTokenType::newline >, //  10
			nullptr, //  11
			nullptr, //  12
			nullptr, //  13
			nullptr, //  14
			nullptr, //  15
			nullptr, //  16
			nullptr, //  17
			nullptr, //  18
			nullptr, //  19
			nullptr, //  20
			nullptr, //  21
			nullptr, //  22
			nullptr, //  23
			nullptr, //  24
			nullptr, //  25
			nullptr, //  26
			nullptr, //  27
			nullptr, //  28
			nullptr, //  29
			nullptr, //  30
			nullptr, //  31
			nullptr, //  32 ' '
			obtainsymbol< ExlTokenType::bang >, //  33 '!'
			obtainstring, //  34 '"'
			obtainsymbol< ExlTokenType::pound >, //  35 '#'
			nullptr, //  36 '$'
			nullptr, //  37 '%'
			nullptr, //  38 '&'
			obtainstring, //  39 '''
			obtainsymbol< ExlTokenType::leftroundbracket >, //  40 '('
			obtainsymbol< ExlTokenType::rightroundbracket >, //  41 ')'
			obtaindigraph< ExlTokenType::star,
				'=', ExlTokenType::starequals >, //  42 '*'
			obtaindigraph< ExlTokenType::plus,
				'=', ExlTokenType::plusequals >, //  43 '+'
			obtainsymbol< ExlTokenType::comma >, //  44 ','
			[]( CharStream& stream ) -> tokenptr_t //  45 '-'
			{
				TextPos spos = stream.textpos;
				stream.getc();
				int ch = stream.getc();
				if( ch == '=' )
				{
					return tokenptr_t( new ExlToken(
						ExlTokenType::minusequals, spos, stream.textpos ) );
				}
				else if( ch == '-' )
				{
					obtaincomment( stream, spos );
					return tokenptr_t();
				}
				else
				{
					stream.ungetc();
					return tokenptr_t( new ExlToken(
						ExlTokenType::minus, spos, stream.textpos ) );
				}
			},
			[]( CharStream& stream ) -> tokenptr_t //  46 '.'
			{
				TextPos spos = stream.textpos;
				stream.getc();
				int ch = stream.getc();
				if( ch >= '0' && ch <= '9' )
				{
					stream.ungetc();
					stream.ungetc();
					return obtaindecnumber( stream, spos );
				}
				else if( ch != '.' )
				{
					stream.ungetc();
					return tokenptr_t( new ExlToken(
						ExlTokenType::dot, spos, stream.textpos ) );
				}
				ch = stream.getc();
				if( ch != '.' )
				{
					stream.ungetc();
					return tokenptr_t( new ExlToken(
						ExlTokenType::dotdot, spos, stream.textpos ) );
				}
				else
				{
					return tokenptr_t( new ExlToken(
						ExlTokenType::dotdotdot, spos, stream.textpos ) );
				}
			},
			obtaindigraph< ExlTokenType::slash,
				'=', ExlTokenType::slashequals >, //  47 '/'
			[]( CharStream& stream ) -> tokenptr_t //  48 '0'
			{
				TextPos spos = stream.textpos;
				stream.getc();
				int ch = stream.getc();
				if( ch == 'x' || ch == 'X' )
				{
					return obtainhexnumber( stream, spos );
				}
				else
				{
					stream.ungetc();
					stream.ungetc();
					return obtaindecnumber( stream, spos );
				}
				return tokenptr_t( new ExlToken(
					ExlTokenType::number, spos, stream.textpos, 0 ) );
			},
			obtaindecnumber, //  49 '1'
			obtaindecnumber, //  50 '2'
			obtaindecnumber, //  51 '3'
			obtaindecnumber, //  52 '4'
			obtaindecnumber, //  53 '5'
			obtaindecnumber, //  54 '6'
			obtaindecnumber, //  55 '7'
			obtaindecnumber, //  56 '8'
			obtaindecnumber, //  57 '9'
			obtainsymbol< ExlTokenType::colon >, //  58 ':'
			obtainsymbol< ExlTokenType::semicolon >, //  59 ';'
			nullptr, //  60 '<'
			obtaindigraph< ExlTokenType::equals,
				'=', ExlTokenType::equalsequals >, //  61 '='
			nullptr, //  62 '>'
			nullptr, //  63 '?'
			[]( CharStream& stream ) -> tokenptr_t //  64 '@'
			{
				TextPos spos = stream.textpos;
				stream.getc();
				std::vector< uint8_t > word;
				while( true )
				{
					int ch = stream.getc();
					if( letterordigit( ch ) )
					{
						word.push_back( ch );
					}
					else
					{
						stream.ungetc();
						break;
					}
				}
				return tokenptr_t( new ExlToken(
					ExlTokenType::identifier, spos, stream.textpos,
					DataBuffer::create( word.size(), word.data() ) ) );
			},
			obtainword, //  65 'A'
			obtainword, //  66 'B'
			obtainword, //  67 'C'
			obtainword, //  68 'D'
			obtainword, //  69 'E'
			obtainword, //  70 'F'
			obtainword, //  71 'G'
			obtainword, //  72 'H'
			obtainword, //  73 'I'
			obtainword, //  74 'J'
			obtainword, //  75 'K'
			obtainword, //  76 'L'
			obtainword, //  77 'M'
			obtainword, //  78 'N'
			obtainword, //  79 'O'
			obtainword, //  80 'P'
			obtainword, //  81 'Q'
			obtainword, //  82 'R'
			obtainword, //  83 'S'
			obtainword, //  84 'T'
			obtainword, //  85 'U'
			obtainword, //  86 'V'
			obtainword, //  87 'W'
			obtainword, //  88 'X'
			obtainword, //  89 'Y'
			obtainword, //  90 'Z'
			[]( CharStream& stream ) -> tokenptr_t //  91 '['
			{
				TextPos spos = stream.textpos;
				stream.getc();
				int level = 0;
				while( true )
				{
					int ch = stream.getc();
					if( ch == '[' )
					{
						return obtainlongstring< true >( stream, level, spos );
					}
					if( ch == '=' )
					{
						level += 1;
					}
					else
					{
						stream.ungetc();
						for( int i = 0; i < level; ++i )
						{
							stream.ungetc();
						}
						break;
					}
				}
				return tokenptr_t( new ExlToken(
					ExlTokenType::leftsquarebracket, spos, stream.textpos ) );
			},
			nullptr, //  92 '\'
			obtainsymbol< ExlTokenType::rightsquarebracket >, //  93 ']'
			nullptr, //  94 '^'
			obtainword, //  95 '_'
			nullptr, //  96 '`'
			obtainword, //  97 'a'
			obtainword, //  98 'b'
			obtainword, //  99 'c'
			obtainword, // 100 'd'
			obtainword, // 101 'e'
			obtainword, // 102 'f'
			obtainword, // 103 'g'
			obtainword, // 104 'h'
			obtainword, // 105 'i'
			obtainword, // 106 'j'
			obtainword, // 107 'k'
			obtainword, // 108 'l'
			obtainword, // 109 'm'
			obtainword, // 110 'n'
			obtainword, // 111 'o'
			obtainword, // 112 'p'
			obtainword, // 113 'q'
			obtainword, // 114 'r'
			obtainword, // 115 's'
			obtainword, // 116 't'
			obtainword, // 117 'u'
			obtainword, // 118 'v'
			obtainword, // 119 'w'
			obtainword, // 120 'x'
			obtainword, // 121 'y'
			obtainword, // 122 'z'
			obtainsymbol< ExlTokenType::leftbrace >, // 123 '{'
			nullptr, // 124 '|'
			obtainsymbol< ExlTokenType::rightbrace >, // 125 '}'
			obtaindigraph< ExlTokenType::tilde,
				'=', ExlTokenType::tildeequals >, // 126 '~'
			nullptr, // 127 '⌂'
		};

		tokenptr_t obtaintoken( CharStream& stream )
		{
			bool bislinestart = false;
			while( true )
			{
				int ch = stream.getc();
				if( ch == -1 )
				{
					return tokenptr_t( new ExlToken(
						ExlTokenType::eof, stream.textpos, stream.textpos ) );
				}
				else
				{
					auto tfunc = tokentable[ ch ];
					if( tfunc )
					{
						stream.ungetc();
						tokenptr_t token = tfunc( stream );
						if( token && token->type == ExlTokenType::newline )
						{
							bislinestart = true;
						}
						else if( token )
						{
							token->bislinestart = bislinestart;
							return token;
						}
					}
					else if( ch > 32 )
					{
						stream.ungetc();
						stream.error( "invalid symbol", stream.textpos );
					}
				}
			}
		}
	}

	ExlTokenStream::ExlTokenStream(
		char const* const* source, size_t const* sourcelengths,
		size_t partcount, char const* filename, int tabsize )
	{
		cs.source = ( uint8_t const* const* )source;
		cs.sourcelengths = sourcelengths;
		cs.partcount = partcount;
		cs.filename = filename;
		cs.tabsize = tabsize;
		cs.textpos.col = 1;
		cs.textpos.row = 1;
		this->filename = DataBuffer::create( strlen( filename ), filename );
	}

	ExlTokenStream::~ExlTokenStream()
	{
	}

	std::unique_ptr< ExlToken > ExlTokenStream::gett()
	{
		if( !buffer.empty() )
		{
			std::unique_ptr< ExlToken > token = std::move( buffer.top() );
			buffer.pop();
			return token;
		}
		else
		{
			return lexer::obtaintoken( cs );
		}
	}

	void ExlTokenStream::ungett( std::unique_ptr< ExlToken >&& token )
	{
		buffer.emplace( std::move( token ) );
	}

	std::unique_ptr< ExlToken > const& ExlTokenStream::peekt()
	{
		if( buffer.empty() )
		{
			buffer.emplace( lexer::obtaintoken( cs ) );
		}
		return buffer.top();
	}

	// void ExlTokenStream::error(
		// char const* message, TextPos spos, TextPos epos );
}
