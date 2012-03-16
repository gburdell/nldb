
DEPS := VnlParser.java VnlLexer.java

${DEPS}: Vnl.g
	antlr ${<}

.phony: clean
clean: 
	-rm -f ${DEPS}
