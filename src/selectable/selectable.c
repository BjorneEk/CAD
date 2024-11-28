/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * selectable type
 *==========================================================*/

#include "selectable.h"

selectable_t selectable(u32_t type, void *s, selectable_t *parent)
{
	selectable_t res;

	res.type = type;
	res.hovered = false;
	res.selected = false;
	res.parent = parent;
	res.children = DLA_new(sizeof(selectable_t*), 2);

	switch(type) {
		case POINT:	res.point	= s; break;
		case LINE:	res.line	= s; break;
		case CYCLE:	res.cycle	= s; break;
		case PLANE:	res.plane	= s; break;
		case AXIS:	res.axis	= s; break;
		case OBJECT:	res.object	= s; break;
		case FACE:	res.face	= s; break;
	}
	return res;
}