#include "State.h"

namespace ss{

void State::init()
{
	flags = 0;
	cellIndex = 0;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	pivotX = 0.0f;
	pivotY = 0.0f;
	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;
	scaleX = 1.0f;
	scaleY = 1.0f;
	opacity = 255;
	size_X = 1.0f;
	size_Y = 1.0f;
	uv_move_X = 0.0f;
	uv_move_Y = 0.0f;
	uv_rotation = 0.0f;
	uv_scale_X = 1.0f;
	uv_scale_Y = 1.0f;
	boundingRadius = 0.0f;
	colorBlendFunc = 0;
	colorBlendType = 0;
	flipX = false;
	flipY = false;
	isVisibled = false;
	memset(&quad, 0, sizeof(quad));
	texture.handle = 0;
	texture.size_w = 0;
	texture.size_h = 0;
	rect = SSRect(0, 0, 0, 0);
	blendfunc = 0;
	memset(&mat, 0, sizeof(mat));
	instanceValue_curKeyframe = 0;
	instanceValue_startFrame = 0;
	instanceValue_endFrame = 0;
	instanceValue_loopNum = 0;
	instanceValue_speed = 0;
	instanceValue_loopflag = 0;
	effectValue_curKeyframe = 0;
	effectValue_startTime = 0;
	effectValue_speed = 0;
	effectValue_loopflag = 0;

	Calc_rotationX = 0.0f;
	Calc_rotationY = 0.0f;
	Calc_rotationZ = 0.0f;
	Calc_scaleX = 1.0f;
	Calc_scaleY = 1.0f;
	Calc_opacity = 255;

}


} //namespace ss
