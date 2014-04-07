

#ifndef __OSGWTOOLS_VERSION_H__
#define __OSGWTOOLS_VERSION_H__ 1

#include "Export.h"
#include <osg/Version>
#include <string>


namespace osgwTools {


/** \defgroup Version Version utilities */
/*@{*/

#define OSGWORKS_MAJOR_VERSION (@OSGWORKS_MAJOR_VERSION@)
#define OSGWORKS_MINOR_VERSION (@OSGWORKS_MINOR_VERSION@)
#define OSGWORKS_SUB_VERSION (@OSGWORKS_SUB_VERSION@)

/** \brief osgWorks version number as an integer

C preprocessor integrated version number
The form is Mmmss, where:
\li M is the major version
\li mm is the minor version (zero-padded)
\li ss is the sub version (zero padded)

Use this in version-specific code, for example:
\code
#if( OSGWORKS_VERSION < 10500 )
... code specific to releases before v1.05
#endif
\endcode
*/
#define OSGWORKS_VERSION ( \
( OSGWORKS_MAJOR_VERSION * 10000 ) + \
( OSGWORKS_MINOR_VERSION * 100 ) + \
OSGWORKS_SUB_VERSION )

/** \brief OSG version number as an integer

This macro provides the same functionality as
OSGWORKS_VERSION, but instead encodes the OSG version
number as an integer.
OSG didn't provide a useful compile-time version
comparison macro until after the 2.9.6 and 2.8.2 releases.
\see OSGWORKS_VERSION */
#define OSGWORKS_OSG_VERSION ( \
( OPENSCENEGRAPH_MAJOR_VERSION * 10000 ) + \
( OPENSCENEGRAPH_MINOR_VERSION * 100 ) + \
OPENSCENEGRAPH_PATCH_VERSION )

/** \brief Run-time access to the osgWorks version number

Returns OSGWORKS_VERSION, the osgWorks version number as an integer
\see OSGWORKS_VERSION
*/
unsigned int OSGWTOOLS_EXPORT getVersionNumber();

/** \brief osgWorks version number as a string

Example:
\code
osgWorks version 1.1.0 (10100)
\endcode
*/
std::string OSGWTOOLS_EXPORT getVersionString();

/*@}*/


// namespace osgwTools
}

// __OSGWTOOLS_VERSION_H__
#endif
