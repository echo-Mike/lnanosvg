#ifndef LNANOSVG_H__
#define LNANOSVG_H__

/// LNS
#include "lns_includes.h"
#include "lns_macro.h"

LNS_EXTERN_C

#include "lns_nanosvg.h"

LNS_GEN_STRUCT(image);
LNS_GEN_ITER_STRUCT(shape);
LNS_GEN_ITER_STRUCT(path);
LNS_GEN_STRUCT(paint);
LNS_GEN_STRUCT(gradient);
LNS_GEN_STRUCT(gradientStop);

//! Includes MUST appear in next order 
#include "lns_gradientStop.h"
#include "lns_gradient.h"
#include "lns_paint.h"
#include "lns_path.h"
#include "lns_shape.h"
#include "lns_image.h"

LNS_EXTERN_C_END

#endif // LNANOSVG_H__
