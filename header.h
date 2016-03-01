#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <iostream>
#include <time.h>
#include <boost/smart_ptr/shared_ptr.hpp>
//#include <boost/filesystem.hpp>
#include "ogl_tools.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/intersect.hpp>

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::multimap;
using std::pair;

using boost::shared_ptr;
//using namespace boost::filesystem;

using jep::ogl_context;
using jep::ogl_camera;
using jep::ogl_camera_free;
using jep::static_text;
using jep::text_handler;
using jep::key_handler;
using jep::texture_handler;
using jep::text_character;
using jep::mesh_data;
using jep::material_data;

using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;

#endif
