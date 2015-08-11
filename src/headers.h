/*
Copyright (C) 2006 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#define GL_GLEXT_PROTOTYPES

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "stdarg.h"
#include "ctype.h"

#include "zlib.h"

#include "libintl.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_endian.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include <GL/glext.h>

#include "defs.h"
#include "../data/gameDefs/defines.h"

#include "cplusplus/CLinkable.h"

#include "cplusplus/CString.h"

#include "cplusplus/CPoint.h"
#include "cplusplus/CVector.h"
#include "cplusplus/CVectori.h"

#include "cplusplus/CMath.h"

#include "cplusplus/CGLColor.h"

#include "cplusplus/CReference.h"
#include "cplusplus/CList.h"

#include "cplusplus/CData.h"
#include "cplusplus/CHashtable.h"
#include "cplusplus/CProperties.h"

#include "cplusplus/CPlane.h"
#include "cplusplus/CFace.h"
#include "cplusplus/CBoundingBox.h"

#include "cplusplus/CTrace.h"

#include "cplusplus/CGeometry.h"

#include "cplusplus/CSprite.h"
#include "cplusplus/CTexture.h"
#include "cplusplus/CTextureSpec.h"

#include "cplusplus/CTriangle.h"
#include "cplusplus/CSimpleModel.h"
#include "cplusplus/CMDLModel.h"
#include "cplusplus/CMD2.h"

#include "cplusplus/CCrumb.h"
#include "cplusplus/CCrumbBox.h"

#include "cplusplus/CGrid.h"
#include "cplusplus/CBaseWidget.h"
#include "cplusplus/CPanel.h"
#include "cplusplus/CLabel.h"
#include "cplusplus/CButton.h"
#include "cplusplus/CCheckBox.h"
#include "cplusplus/CListViewItem.h"
#include "cplusplus/CListView.h"
#include "cplusplus/CSlider.h"
#include "cplusplus/CComboBox.h"
#include "cplusplus/CInputWidget.h"

#include "cplusplus/CEntityDef.h"
#include "cplusplus/CItemDef.h"
#include "cplusplus/CWeaponDef.h"
#include "cplusplus/CBlobDef.h"
#include "cplusplus/CEnemyDef.h"
#include "cplusplus/CFeatureDef.h"
#include "cplusplus/CBossDef.h"

#include "cplusplus/CEntity.h"
#include "cplusplus/CItem.h"
#include "cplusplus/CWeapon.h"
#include "cplusplus/CStructure.h"
#include "cplusplus/CDoor.h"
#include "cplusplus/CSwitch.h"
#include "cplusplus/CUnit.h"
#include "cplusplus/CBullet.h"
#include "cplusplus/CBoss.h"
#include "cplusplus/CDecoration.h"
#include "cplusplus/CFeature.h"
#include "cplusplus/CEmitter.h"
#include "cplusplus/CTrap.h"

#include "cplusplus/CQ3Target.h"
#include "cplusplus/CCutsceneCamera.h"

#include "cplusplus/CBSP.h"
#include "cplusplus/CBoundingBoxManager.h"

#include "cplusplus/CDecal.h"

#include "cplusplus/CEntityManager.h"

#include "cplusplus/CRandomNameGen.h"

#include "cplusplus/CFrustum.h"
#include "cplusplus/CCamera.h"

#include "cplusplus/CParticle.h"

#include "cplusplus/CModelManager.h"
#include "cplusplus/CTextureManager.h"
#include "cplusplus/CParticleManager.h"
#include "cplusplus/CTextureSpecManager.h"

#include "cplusplus/CFileData.h"
#include "cplusplus/CPak.h"

#include "cplusplus/CSaveGameHeader.h"

#include "cplusplus/CMouse.h"
#include "cplusplus/CEngine.h"
#include "cplusplus/CAudio.h"
#include "cplusplus/CGraphics.h"
#include "cplusplus/CUIManager.h"
#include "cplusplus/CCollision.h"

#include "cplusplus/CGame.h"

#include "cplusplus/CObjective.h"
#include "cplusplus/CTrigger.h"
#include "cplusplus/CMission.h"

#include "cplusplus/CMap.h"

#include "cplusplus/CCutsceneCommand.h"
