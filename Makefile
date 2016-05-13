# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2011/July/02
#

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

now := $(shell date --utc "+%Y-%m-%d-%H%M%S")

SUBDIR_INSTALL=3333
SUBDIR_INSTALL=4
WWWINSTDISR=${HOME}/public_html/${SUBDIR_INSTALL}

default:
	@echo Please specify explicit target 1>&2
	@false

consts.py: FORCE common-consts.py
	echo '# Do NOT edit directly' > $@
	cat common-consts.py >> $@
	echo version = \"${now}\"';' >> $@

LEVEL1_3333 = \
	3333.css \
	m3333.css \
	help.html \
	favicon.ico

BIN_3333 = 3333-server.py consts.py watchdog.py
TGZFILES = \
	Makefile \
	3333.js \
	common-consts.py \
	${LEVEL1_3333} \
	${BIN_3333}

MAINJS=${WWWINSTDISR}/3333.js
# Insert Python constants after maguic line in Javascript
${MAINJS}: 3333.js consts.py Makefile
	(N=$$(grep -n consts.py < 3333.js | cut -d: -f1); \
	head -n $${N} 3333.js > $@; \
	sed -e 's/ =/:/g' -e 's/;/,/g' -e 's:#://:g' < consts.py >> $@; \
	echo '// Do NOT edit directly' >> $@ \
	N=$$((N+1)); \
	tail -n +$${N} 3333.js >> $@)
	wc -l 3333.js common-consts.py $@

STEP2 = ${MAINJS}

# { Static mobile navbars
MPAGES = club table players help
MFOOTERS = $(foreach p, $(MPAGES), footer-${p}.html)
mfooters: ${MFOOTERS}

${MFOOTERS}: footer-all.html Makefile
	@echo $@
	for p in ${MPAGES}; do \
	  grep -v $$p footer-all.html > footer-$$p.html; \
	done

ifneq ($(MAKECMDGOALS),3333-install)
m3333.html: m3333-0.html ${MFOOTERS} Makefile
	gpp $< > $@
endif

# } 

ifeq ($(LOCAL),1)
TGZFILES += 3333.html m3333.html
IHTML=${WWWINSTDISR}/3333.html
IMHTML=${WWWINSTDISR}/m3333.html
z${IHzTML}z: 3333.html Makefile
	sed \
	  -e 's=http://ajax.googleapis.com/ajax/libs/=../jq/=g' \
	  < $< > $@

${IHTML}: 3333.html Makefile
	sed -e 's=http://code.jquery.com/=../jq/=g' < $< > $@

${IMHTML}: m3333.html Makefile
	sed -e 's=http://code.jquery.com/=../jq/=g' < $< > $@

STEP2 += ${IHTML} ${IMHTML}
else
  LEVEL1_3333 += 3333.html m3333.html
endif

3333-install-step1: ${LEVEL1_3333} ${BIN_3333} FORCE 
	rm -rf ${WWWINSTDISR} ${HOME}/${SUBDIR_INSTALL}
	mkdir -p ${WWWINSTDISR}/cgi-bin
	mkdir -p ${HOME}/${SUBDIR_INSTALL}/bin ${HOME}/${SUBDIR_INSTALL}/var
	cp ${LEVEL1_3333} ${WWWINSTDISR}
	(cd ${WWWINSTDISR}; ln -s 3333.html index.html)
	cp ${BIN_3333} ${HOME}/${SUBDIR_INSTALL}/bin

3333-install-step2: ${STEP2}

3333-install: 3333-install-step1 3333-install-step2


.PHONY: consts FORCE

consts: consts.py

TGZ = 3333.tgz
${TGZ}: ${TGZFILES}
	tar czf $@ ${TGZFILES}
	tar tvzf $@; ls -lG $@

FORCE:

release: ${TGZ}

SRCS = \
	Makefile common-consts.py ${LEVEL1_3333}\
	3333.html \
	3333.js \
	m3333-0.html \
	footer-all.html \
	debug.js ${BIN_3333}

STGZ = /tmp/s3333.tgz
${STGZ}: ${SRCS}
	tar czf $@ ${SRCS}
	tar tvzf $@; ls -lG $@

save: ${STGZ}

TOF = help.html taboverflow.css taboverflow.html taboverflow.js
TOFZIP = /tmp/tof-${now}.zip
${TOFZIP}: ${TOF}
	zip ${TOFZIP} ${TOF}
tof: ${TOFZIP}

