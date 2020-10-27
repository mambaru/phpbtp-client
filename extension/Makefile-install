NAME				=	phpbtp-client

INI_DIR				=	/etc/php7/conf.d

EXTENSION_DIR		=	$(shell php-config --extension-dir)

EXTENSION 			=	${NAME}.so

INI 				=	${NAME}.ini

CONF				=	${NAME}.conf

CP					=	cp -f

install:		
						${CP} ${EXTENSION} ${EXTENSION_DIR}
						${CP} ${INI} ${INI_DIR}
						${CP} ${CONF} ${INI_DIR}
