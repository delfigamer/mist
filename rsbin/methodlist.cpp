#include "methodlist.hpp"
#include "common.hpp"
#include "fileio.hpp"
#include "fsthread.hpp"
#include "pngreader.hpp"
#include "pngwriter.hpp"

namespace rsbin {
	MethodList const* getmethodlist() {
		static MethodList const List = {
			rsbin_fileio_new,
			rsbin_fileio_startread,
			rsbin_fileio_startwrite,
			rsbin_fileio_setsize,
			rsbin_fstask_promote,
			rsbin_fstask_isfinished,
			rsbin_fstask_getresult,
			rsbin_fstask_geterror,
			rsbin_pngreader_new,
			rsbin_pngreader_delete,
			rsbin_pngreader_feed,
			rsbin_pngreader_isfinished,
			rsbin_pngreader_getwidth,
			rsbin_pngreader_getheight,
			rsbin_pngreader_getdata,
			rsbin_pngwriter_new,
			rsbin_pngwriter_delete,
			rsbin_pngwriter_grab,
			rsbin_pngwriter_isfinished,
			rsbin_fileexists,
			rsbin_filesize,
		};
		return &List;
	}
}
