PROG = blobAndConquer
PAKNAME = blobAndConquer.pak
DOCS = doc/*
ICONS = icons/

PAKDIRS = data gfx music sound textures

VERSION = 1.11
RELEASE ?= 0
USEPAK ?= 0
DEV ?= 0

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin/
DATADIR ?= $(PREFIX)/share/games/blobAndConquer/
DOCDIR ?= $(PREFIX)/share/doc/$(PROG)/
ICONDIR ?= $(PREFIX)/share/icons/hicolor/
MENUDIR ?= $(PREFIX)/share/applications/
LOCALEDIR ?= $(PREFIX)/share/locale/

CFLAGS ?= -Ofast -march=native -Wall
CXXFLAGS += -Wformat=2 -Wformat-security -Wstrict-aliasing=2
CXXFLAGS += -Wmissing-format-attribute -Wmissing-noreturn
CXXFLAGS += -Wdisabled-optimization
CXXFLAGS += `sdl2-config --cflags` -DVERSION=$(VERSION) -DRELEASE=$(RELEASE) -DUSEPAK=$(USEPAK) -DDEV=$(DEV)
CXXFLAGS += -DPAKNAME=\"$(PAKNAME)\" -DPAKLOCATION=\"$(DATADIR)\" -DGAMEPLAYMANUAL=\"$(DOCDIR)/index.html\" -Wall
CXXFLAGS += -DLOCALEDIR=\"$(LOCALEDIR)\" -g
CXXFLAGS += $(CFLAGS)
LIBPATH = -L/usr/X11/lib -L/usr/X11R6/lib
LIBS = -lX11 -lGLU -lGL `sdl2-config --libs` -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lz
PAKLIBS = -lz

# Yes, it means compiling is a little slower... but it's less hassle
SEARCHPATH += src/
SEARCHPATH += src/bosses
SEARCHPATH += src/combat
SEARCHPATH += src/cplusplus
SEARCHPATH += src/effects
SEARCHPATH += src/entities
SEARCHPATH += src/hud
SEARCHPATH += src/io
SEARCHPATH += src/misc
SEARCHPATH += src/missions
SEARCHPATH += src/puzzles
SEARCHPATH += src/world
vpath %.cpp $(SEARCHPATH)
vpath %.h $(SEARCHPATH)

OBJS += CAudio.o
OBJS += CBaseWidget.o CBlobDef.o CBoundingBox.o CBoundingBoxManager.o CBoss.o CBossDef.o CBSP.o CBullet.o CButton.o
OBJS += CCamera.o CCheckBox.o CCollision.o CComboBox.o CCrumb.o CCrumbBox.o CCutsceneCamera.o CCutsceneCommand.o
OBJS += CData.o CDecal.o CDecoration.o CDoor.o
OBJS += CEmitter.o CEnemyDef.o CEntity.o CEntityDef.o CEntityManager.o CEngine.o
OBJS += CFace.o CFeature.o CFileData.o CFrustum.o
OBJS += CGame.o CGeometry.o CGLColor.o CGraphics.o CGrid.o
OBJS += CHashtable.o 
OBJS += CInputWidget.o CItem.o CItemDef.o
OBJS += CLabel.o CLinkable.o CList.o CListView.o CListViewItem.o
OBJS += CMap.o CMath.o CMD2.o CMDLModel.o CMission.o CModelManager.o CMouse.o
OBJS += CObjective.o
OBJS += CPak.o CPanel.o CParticle.o CParticleManager.o CPlane.o CPoint.o CProperties.o
OBJS += CQ3Target.o
OBJS += CRandomNameGen.o CReference.o
OBJS += CSaveGameHeader.o CSimpleModel.o CSlider.o CSprite.o CString.o CStructure.o CSwitch.o
OBJS += CTexture.o CTextureManager.o CTextureSpec.o CTextureSpecManager.o CTrace.o CTrap.o CTriangle.o CTrigger.o
OBJS += CUIManager.o CUnit.o
OBJS += CVector.o CVectori.o
OBJS += CWeapon.o CWeaponDef.o

OBJS += ambush.o
OBJS += battle.o bioMechBlobs.o bioTanks.o blobs.o bosses.o bsp.o bullets.o
OBJS += camera.o combat.o controlPanel.o cryptBoss.o cutscene.o
OBJS += darkBioMechs.o decals.o decoration.o dialogs.o doors.o
OBJS += emitters.o enemy.o eyeDroids.o eyeDroidCommander.o explosions.o
OBJS += features.o finalBoss.o
OBJS += game.o
OBJS += init.o items.o
OBJS += laserGrid.o lift.o load.o
OBJS += mastermind.o md2.o misc.o mission.o modelLoader.o
OBJS += opengl.o options.o
OBJS += particles.o player.o propertiesLoader.o
OBJS += resources.o
OBJS += save.o sentryGuns.o shadows.o specialist.o spiderBlobs.o spiderBlobCommander.o status.o structures.o switches.o
OBJS += title.o traps.o triggers.o
OBJS += units.o
OBJS += weapons.o weather.o widgets.o world.o

GAMEOBJS = $(OBJS) main.o
PAKOBJS = CLinkable.o CFileData.o pak.o

LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

ALL = $(PROG) $(LOCALE_MO)
ifeq ($(USEPAK), 1)
	ALL += $(PAKNAME)
endif

# top-level rule to create the program.
all: $(ALL)

%.o: %.cpp %.h data/gameDefs/defines.h defs.h headers.h
	$(CXX) $(CXXFLAGS) -c $<

# linking the program.
$(PROG): $(GAMEOBJS)
	$(CXX) $(LDFLAGS) $(GAMEOBJS) -o $(PROG) $(LIBS)
	
pak: $(PAKOBJS)
	$(CXX) $(LDFLAGS) $(PAKOBJS) -o pak $(PAKLIBS)
	
%.mo: %.po
	msgfmt -c -o $@ $<

$(PAKNAME): pak
	./pak $(PAKDIRS) $(PAKNAME)
	
buildpak: $(PAKNAME)
	
install: $(ALL)
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(DATADIR)
	mkdir -p $(DESTDIR)$(DOCDIR)
	mkdir -p $(DESTDIR)$(ICONDIR)16x16/apps
	mkdir -p $(DESTDIR)$(ICONDIR)32x32/apps
	mkdir -p $(DESTDIR)$(ICONDIR)64x64/apps
	mkdir -p $(DESTDIR)$(MENUDIR)

	install -m 755 $(PROG) $(DESTDIR)$(BINDIR)$(PROG)
ifeq ($(USEPAK), 1)
	install -m 644 $(PAKNAME) $(DESTDIR)$(DATADIR)$(PAKNAME)
else
	cp -pr $(PAKDIRS) $(DESTDIR)$(DATADIR)
endif
	cp -pr $(DOCS) $(DESTDIR)$(DOCDIR)
	cp -p $(ICONS)$(PROG).png $(DESTDIR)$(ICONDIR)32x32/apps/
	cp -p $(ICONS)$(PROG).desktop $(DESTDIR)$(MENUDIR)

	@for f in $(LOCALE_MO); do \
		lang=`echo $$f | sed -e 's/^locale\///;s/\.mo$$//'`; \
		mkdir -p $(DESTDIR)$(LOCALEDIR)$$lang/LC_MESSAGES; \
		echo "cp -p $$f $(DESTDIR)$(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo"; \
		cp -p $$f $(DESTDIR)$(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo; \
	done
	
uninstall:

	rm -f $(DESTDIR)$(BINDIR)/$(PROG)
	rm -rf $(DESTDIR)$(DATADIR)
	rm -rf $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(ICONDIR)32x32/apps/blobAndConquer.png
	rm -f $(DESTDIR)$(MENUDIR)/blobAndConquer.desktop
	
	@for f in $(LOCALE_MO); do \
		lang=`echo $$f | sed -e 's/^locale\///;s/\.mo$$//'`; \
		echo "$(RM) $(DESTDIR)$(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo"; \
		$(RM) $(DESTDIR)$(LOCALEDIR)$$lang/LC_MESSAGES/$(PROG).mo; \
	done

# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(PAKOBJS) pak main.o $(PROG) $(LOCALE_MO)
