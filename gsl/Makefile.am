
header-links: remove-links
	HEADERLIST="$(top_srcdir)/gsl*.h $(top_srcdir)/*/gsl*.h"; \
	for h in $$HEADERLIST; do \
	  BASENAME=`basename $$h`; \
	  test -r $$BASENAME || $(LN_S) $$h $$BASENAME; \
	done
	    
remove-links: 
	rm -f gsl*.h
	    
	    
all: all-am header-links
	
clean-local: clean-am remove-links
distclean-local: distclean-am remove-links
	-rm -f Makefile
