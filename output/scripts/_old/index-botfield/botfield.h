
typedef struct {
} Botfield_Field;

char const* botfield_cbase_geterror();
char const* botfield_cbase_seterror( char const* error );
void botfield_cbase_seterrorfuncs(
	char const*( *geterror )(),
	char const*( *seterror )( char const* ) );
void botfield_cbase_setlogfunc(
	bool( *log )( char const* ) );
Botfield_Field* botfield_field_new( int width, int height, uint8_t const* matrix, const char* script );
bool botfield_field_delete( Botfield_Field* field );
float const* botfield_field_getbotpos( Botfield_Field* field );
bool botfield_field_setbotpos( Botfield_Field* field, float x, float y, float yaw );
bool botfield_field_setenginepos( Botfield_Field* field, float dx, float dy );
bool botfield_field_setsensorpos( Botfield_Field* field, float lx, float ly, float rx, float ry );
bool botfield_field_tick( Botfield_Field* field, float dt );
