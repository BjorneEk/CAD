/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * selectable type
 *==========================================================*/

#ifndef _SELECTABLE_H_
#define _SELECTABLE_H_
#include "../util/types.h"
#include "selectable_types.h"
#include "../structures/dynamic_array.h"

enum selectable_type {
	POINT,
	LINE,
	CYCLE,
	UNCLOSED,
	PLANE,
	AXIS,
	OBJECT,
	FACE,
};

typedef struct selectable {

	u32_t type;

	char *name;

	bool hovered;
	bool selected;

	struct selectable *parent;

	dla_t *children;

	union {
		v3f_t		*point;
		line_t		*line;
		segment_t	*cycle;
		segment_t	*unclosed;
		plane_t		*plane;
		axis_t		*axis;
		object_t	*object;
		face_t		*face;
	};

} selectable_t;

selectable_t selectable(u32_t type, void *s, selectable_t *parent);
#endif /* _SELECTABLE_H_ */