
noinst_DATA = joyce.lyx joyce.pdf joyce.txt \
              edops.lyx edops.pdf edops.txt \
	      anne.lyx  anne.pdf anne.txt \
	      hardware.lyx hardware.pdf hardware.txt \
	      pcwkey.ps

EXTRA_DIST = $(noinst_DATA)

#
# At some point LyX silently switched over to outputting files ending .text
# rather than .txt. So rename back to .txt.
#
%.txt:	%.lyx
	lyx -e text $<
	mv `basename $@ .txt`.text $@	

%.tex:	%.lyx
	lyx -e latex $<

%.pdf:	%.lyx
	lyx -e pdf $<
