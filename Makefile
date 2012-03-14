###########################################
# Makefile template                       #
# Useful for programming courses          #
###########################################
# make help => Display this section and exit
#
# make [all]    => Build barcode maker (barcode) and barcode checker (checkbar)
# make checkbar => Build the barcode checker
# make run      => Build & run final executable
# make build    => Build all intermediates objects
# make test     => if a test.c files exists, build and run it (main in test.c)
# 
# make archive => Build final archive to send
# make backup  => Build archive and send a copy on a candi machine @ montefiore
# 
# make clean    => Delete intermediate objects, temp files and test executable
# make mrproper => Delete all files produced with make
#
# make montefiore => Build final archive, send it on a candi box at montefiore,
#                    compile and run it remotely with SSH and X forwarding
#
# make FILE.png => convert FILE.pbm to png format
###########################################

#Project specific configuration
PROJECT_NO = 4
OBJS       = pbm.o barcode.o file_foreach.o
EXEC       = barcode
EXEC2      = checkbar
ARFILES    = pbm.[hc] barcode.[hc] file_foreach.[hc] main.c checkbar.c Makefile README.md
PKGCONF    = 
RUN_ARGS   = 

#Personal configuration (for backups only)
CANDI_USER = pmli0117
CANDI_HOST = candi04.montefiore.ulg.ac.be
COURSE     = INFO0030
CANDI_PATH = /home/${CANDI_USER}/${COURSE}/project${PROJECT_NO}/

#Common for all projects
ARCHIVE = project${PROJECT_NO}-${CANDI_USER}.tar.gz
CC      = gcc
CCFLAGS = --std=c99 --pedantic -Wall -W -Wmissing-prototypes
LDFLAGS = 
TEST    = test.exe
SSHCMD  = cd ${CANDI_PATH} && tar xf ${ARCHIVE} && make mrproper run
ifneq ($(strip $(PKGCONF)),)
	CCFLAGS += `pkg-config --cflags ${PKGCONF}`
	LDFLAGS += `pkg-config --libs ${PKGCONF}`
endif

all : ${EXEC} ${EXEC2}

run : ${EXEC}
	./${EXEC} ${RUN_ARGS}

test : ${TEST}
	./${TEST}
	
build : ${OBJS}

montefiore : backup
	ssh -X ${CANDI_USER}@${CANDI_HOST} "${SSHCMD}"

archive : ${ARCHIVE}

backup: ${ARCHIVE}
	rsync --backup ${ARCHIVE} ${CANDI_USER}@${CANDI_HOST}:${CANDI_PATH}

clean: 
	rm -f *.o
	rm -f ._* *~ .DS_Store
	rm -f ${TEST}
	
mrproper : clean
	rm -f ${EXEC}
	rm -f ${ARCHIVE}
	rm -f ${EXEC2}
	rm -f *.pbm *.png
	
help : 
	head -23 Makefile

%.o : %.c
	${CC} -c ${CCFLAGS} -o $@ $^ 
	
%.png : %.pbm
	pnmtopng $< > $@

${EXEC2} : ${OBJS} checkbar.o
	${CC} ${LDFLAGS} -o $@ $^
	
${EXEC} : ${OBJS} main.o
	${CC} ${LDFLAGS} -o $@ $^ 
	
${TEST} : ${OBJS} test.o
	${CC} ${LDFLAGS} -o $@ $^

#Avoiding object or temp files in archive for wide wildcards
${ARCHIVE} : ${ARFILES}
	make clean
	tar c $^ | gzip > $@
